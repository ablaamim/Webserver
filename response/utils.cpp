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
    if (this->kwargs["root"].empty())
        this->serveERROR("404", "Not Found");
    this->resourceFullPath = pathJoin(this->kwargs["root"][0], _req.path);
    this->resourceType = getResourceType(); 
}

void lookForIndex(Response &resp)
{
    bool indexFound = false;
    std::string index_path = "";

    resp.indexChecked = true;
    std::map<std::string, std::vector<std::string> >::iterator it = resp.kwargs.find("index");
    if (it != resp.kwargs.end())
    {
        std::vector<std::string> index_pages = it->second;
        for (std::vector<std::string>::iterator it2 = index_pages.begin();
             it2 != index_pages.end(); it2++)
        {
            index_path = pathJoin(resp.resourceFullPath, *it2);
            if (fileExists(index_path.c_str()))
            {
                resp.resourceFullPath = index_path;
                resp.resourceType = FILE;
                return;
            }
        }
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
    {
        std::cout << "auto_index: " << resp.kwargs["auto_index"][0] << std::endl;
        resp.serveERROR("403", "Forbidden");
    }
        
}