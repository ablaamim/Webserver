#include "Response.hpp"

int     Response::getResourceType()
{
    if (this->kwargs.find("cgi-bin") != this->kwargs.end())
        return CGI;
    else if (this->kwargs.find("return") != this->kwargs.end())
    {
        std::string returnCode = this->kwargs["return"][0];
        std::cout << "RETURN CODE == " << returnCode << std::endl;
        return REDIRECT;
    }
        
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
        
    }
    return false;
}

void    Response::setResourceInfo()
{
    if (this->kwargs["root"].empty())
        this->serveERROR("404", "Not Found");
    else
    {
        if (indexExists(*this) == false)
            this->resourceFullPath = this->kwargs["root"][0].append(_req.path);
    }
    this->resourceType = getResourceType();
    std::cout << "RESOURCE path: " << this->resourceFullPath << std::endl;
}