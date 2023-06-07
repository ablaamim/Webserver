#include "Response.hpp"

int     Response::getResourceType()
{
    if (this->kwargs.find("cgi-bin") != this->kwargs.end())
        return CGI;
    if (isDirectory(this->resourceFullPath.c_str()))
        return DIRECTORY;
    return FILE;
}

bool    fileExists(const char *path)
{
    bool    exists = false;
    std::ifstream   f(path);
    exists = f.good();
    f.close();
    return exists;
}

bool    indexExists(Response& resp)
{
    std::map<std::string, std::vector<std::string> >::iterator it = resp.kwargs.find("index");
    if (it != resp.kwargs.end())
    {
        std::vector<std::string> index_pages = it->second;
        for (std::vector<std::string>::iterator it2 = index_pages.begin(); it2 != index_pages.end(); it2++)
        {
            std::string index_path = resp.kwargs["root"][0] + "/";
            index_path.append(*it2);
            if (fileExists(index_path.c_str()))
            {
                resp.resourceFullPath = index_path;
                return true;
            }
        }
        /*
            If we get here, it means that none of the index pages exist
            this->referer = ERROR; means that we will serve the 403 error page directly without trying to find custom error page
            this is the behaviour of nginx
        */
        if (resp.referer == NONE)
            resp.referer = INDEX_EXISTS;
        resp.serveERROR("403", "Forbidden");
    }
    return false;
}

void    Response::setResourceInfo()
{
    this->initialized = true;
    if (this->kwargs["root"].empty())
        this->serveERROR("404", "Not Found");
    else if (this->method == GET)
    {
        /*
            trying to find index page if it exists when the request is GET
            if so, we will set resourceFullPath to the index page path
        */
        if (indexExists(*this) == false)
        {
            std::string root = this->kwargs["root"][0];
            this->resourceFullPath = root.append(this->_req.path);
            std::cout << "resourceFullPath = " << this->resourceFullPath << std::endl;
        }
    }
    this->resourceType = this->getResourceType();
}