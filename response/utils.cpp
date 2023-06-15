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
    if (this->kwargs.find("cgi-bin") != this->kwargs.end())
        this->isCGI = true;
    if (this->kwargs.find("return") != this->kwargs.end())
        return REDIRECT;
    if (isDirectory(this->resourceFullPath.c_str()))
        return DIRECTORY;
    return FILE;
}

void    setQueryParams(Response& resp)
{
    resp.queryParams = "";
    std::string::size_type pos = resp.resourceFullPath.find("?");
    if (pos != std::string::npos)
        resp.queryParams = resp.resourceFullPath.substr(pos + 1);
}

void    setCleanURI(Response& resp)
{
    std::string::size_type pos = resp.resourceFullPath.find("?");
    if (pos != std::string::npos)
        resp.resourceFullPath.erase(pos);
}

void    Response::setResourceInfo()
{
    if (this->kwargs.find("root") == this->kwargs.end())
        this->serveERROR(_CS_404, _CS_404_m);
    this->resourceFullPath = pathJoin(this->kwargs["root"][0], _req.path);
    setQueryParams(*this);
    setCleanURI(*this);
    this->resourceType = getResourceType(); 
}

void lookForIndex(Response &resp)
{
    std::string index_path = "";
    resp.print_kwargs();
    resp.indexChecked = true;
    std::map<std::string, std::vector<std::string> >::iterator it = resp.kwargs.find("index");
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
        resp.isCGI = false; /* since the resource is a directory, and none of index files were found, we will serve the directory or index.html no need for CGI, that is/ */
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

std::string getFileExtension(const std::string path)
{
    path.substr(0, path.find("?"));
    std::cout << "path: " << path << std::endl;
    size_t pos = path.find_last_of(".");
    if (pos != std::string::npos)
        return path.substr(pos);
    return "";
}

std::string getInterpreter(Response &resp, const std::string &fileExtension)
{
    std::map<std::string, std::vector<std::string> >::iterator directive = resp.kwargs.find("cgi-bin");
    std::vector<std::string> directiveValues = directive->second;
    std::vector<std::string>::iterator it = std::find(directiveValues.begin(), directiveValues.end(), fileExtension);
    if (it != directiveValues.end())
        return (*(it + 1));
    return "";
}

std::string extractQueryParams(std::string &path)
{
    std::string query_string = "";
    size_t pos = path.find("?");
    if (pos != std::string::npos)
        return (path.substr(pos + 1));
    return query_string;
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
        if (resp._req.path[resp._req.path.length() - 1] != '/')
        {
            resp._req.path.append("/");
            resp.headers["Location"] = resp._req.path;
            resp.status = std::make_pair(_CS_301, _CS_301_m);
            resp.sendResponse(HEADERS_ONLY);
            return true;
        }
    }
    return false;
}