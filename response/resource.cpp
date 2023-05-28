#include "Response.hpp"




int     Response::getResourceType()
{
    if (this->kwargs.find("cgi-bin") != this->kwargs.end())
        return CGI;
    else if (this->kwargs.find("return") != this->kwargs.end())
        return REDIRECT;
    if (isDirectory(this->resourceFullPath.c_str()))
        return DIRECTORY;
    return FILE;
}

void    Response::setResourceInfo()
{
    this->resourceFullPath = this->kwargs["root"][0].append(_req.path);
    this->resourceType = getResourceType(); 
}