#include "Response.hpp"

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