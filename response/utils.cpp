#include "Response.hpp"

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

void    Response::setResourceInfo()
{
    if (this->kwargs.find("root") == this->kwargs.end())
    {
        //std::cout << COLOR_BLUE << "root not found" << COLOR_RESET << std::endl;
        this->serveERROR("404", "Not Found");
    }
    std::cout << "ROOT BEFORE CONCATENATION = " << this->kwargs["root"][0] << std::endl;
    this->resourceFullPath = pathJoin(this->kwargs["root"][0], _req.path);
    this->resourceType = getResourceType(); 
}

void lookForIndex(Response &resp)
{
    std::string index_path = "";

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
        if (resp.kwargs["auto_index"][0] != "on")
            resp.serveERROR("403", "Forbidden");
    }
    else if (resp.isCGI == false && resp.method == GET)
    {
        index_path = pathJoin(resp.resourceFullPath, "index.html");
        if (fileExists(index_path.c_str()))
        {
            resp.resourceFullPath = index_path;
            resp.resourceType = FILE;
            return;
        }
    }
    if (resp.kwargs["auto_index"][0] != "on" && resp.isCGI == false)
        resp.serveERROR("403", "Forbidden");
        
}

std::string getFileExtension(const std::string path)
{
    // get ride of query string
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
            resp.status = std::make_pair("301", "Moved Permanently");
            resp.sendResponse(HEADERS_ONLY);
            return true;
        }
    }
    return false;
}