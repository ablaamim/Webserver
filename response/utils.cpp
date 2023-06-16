#include "../MainInc/main.hpp"

bool    isDirectory(std::string path)
{
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else
        return false;
}

std::string getFileExtension(std::string path)
{
    std::string::size_type pos = path.rfind(".");
    if (pos != std::string::npos)
        return path.substr(pos);
    return "";
}

std::string getTodayDate()
{
    time_t now = time(0);
    char* date_time = ctime(&now);
    return (std::string(date_time, strlen(date_time) - 1));
}

bool fileExists(const char *path)
{
    std::ifstream f(path);
    bool exists = false;

    exists = f.good();
    f.close();
    return exists;
}


std::string     pathJoin(std::string path1, std::string path2)
{
    if (path1[path1.length() - 1] != '/')
        path1.append("/");
    if (path2[0] == '/')
        path2.erase(0, 1);
    path1.append(path2);
    return (path1);
}

int     Response::getResourceType()
{
    if (this->kwargs.find("return") != this->kwargs.end())
        return REDIRECT;
    if (isDirectory(this->resourceFullPath.c_str()))
        return DIRECTORY;
    return FILE;
}

void    setQueryParams(Response& resp)
{
    resp.queryParams = "";
    std::string::size_type pos = resp._req.path.find("?");
    if (pos != std::string::npos)
        resp.queryParams = resp._req.path.substr(pos + 1);
}

void    setCleanPath(Response& resp)
{
    resp.cleanPath = resp._req.path;
    std::string::size_type pos = resp.cleanPath.find("?");
    if (pos != std::string::npos)
        resp.cleanPath.erase(pos);
}

void    Response::setResourceInfo()
{
    if (this->kwargs.find("root") == this->kwargs.end())
        this->serveERROR(_CS_404, _CS_404_m);
    this->resourceFullPath = pathJoin(this->kwargs["root"][0], this->cleanPath);
    this->resourceType = getResourceType(); 
}

void    setExtension(Response& resp)
{
    std::string extension = resp.resourceFullPath;
    std::string::size_type pos = extension.find_last_of(".");
    if (pos != std::string::npos)
        resp.fileExtension = extension.substr(pos);
    else
        resp.fileExtension = "";
}

void    setInterpreter(Response& resp)
{
    resp.cgiInterpreter = "";
    std::cout << "setInterpreter" << std::endl;
    resp.print_kwargs();
    std::map<std::string, std::vector<std::string> >::iterator directive = resp.kwargs.find("cgi-bin");
    if (directive == resp.kwargs.end())
        return ;
    std::vector<std::string> directiveValues = directive->second;
    std::vector<std::string>::iterator it = std::find(directiveValues.begin(), directiveValues.end(), resp.fileExtension);
    if (it != directiveValues.end())
        resp.cgiInterpreter = *(it + 1);
}

void    lookForCGI(Response& resp)
{
    try
    {
        if (resp.resourceType == FILE && resp.method != DELETE)
        {
            setExtension(resp);
            setInterpreter(resp);
            if (resp.cgiInterpreter.empty() == false)
            {
                if (resp.fileExtension == ".php" || resp.fileExtension == ".py" || resp.fileExtension == ".sh")
                    resp.isCGI = true;
                else
                    throw std::runtime_error("CGI interpreter not supported");
            }
        }
    }
    catch(const std::exception& e)
    {
        resp.serveERROR(_CS_501, _CS_501_m);
    }
}

void lookForIndex(Response &resp)
{
    resp.indexChecked = true;
    if (resp.resourceType == DIRECTORY && resp.method != DELETE)
    {
        std::string                                                 index_path = "";
        std::map<std::string, std::vector<std::string> >::iterator  it = resp.kwargs.find("index");
        if (it != resp.kwargs.end())
        {
            std::vector<std::string> index_pages = it->second;
            for (std::vector<std::string>::iterator it2 = index_pages.begin(); it2 != index_pages.end(); it2++)
            {
                index_path = pathJoin(resp.resourceFullPath, *it2);
                if (fileExists(index_path.c_str()))
                {
                    resp.resourceFullPath = index_path;
                    resp.resourceType = FILE;
                    return;
                }
            }
            resp.isCGI = false;
            if (resp.kwargs["auto_index"][0] != "on")
                resp.serveERROR(_CS_403, _CS_403_m);
        }
        else
        {
            resp.isCGI = false;
            if (resp.method == GET)
            {
                index_path = pathJoin(resp.resourceFullPath, "index.html");
                if (fileExists(index_path.c_str()))
                {
                    resp.resourceFullPath = index_path;
                    resp.resourceType = FILE;
                    return;
                }
            }
        }
        if (resp.kwargs["auto_index"][0] != "on")
            resp.serveERROR(_CS_403, _CS_403_m);
    }
}

bool    needsRedirection(Response& resp)
{
    if (resp.resourceType == REDIRECT)
    {
        resp.serveRedirect();
        return true;
    }
    else if (resp.resourceType == DIRECTORY)
    {
        if (resp.cleanPath[resp.cleanPath.length() - 1] != '/')
        {
            resp.cleanPath.append("/");
            if (resp.queryParams.empty() == false)
            {
                resp.cleanPath.append("?");
                resp.cleanPath.append(resp.queryParams);
            }
            resp.headers["Location"] = resp.cleanPath;
            resp.status = std::make_pair(_CS_301, _CS_301_m);
            resp.sendResponse(HEADERS_ONLY);
            return true;
        }
    }
    return false;
}