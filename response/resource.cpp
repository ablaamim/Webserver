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
    if (this->kwargs["root"].empty())
    {
        this->status = std::make_pair("404", "Root not Found");
        throw Response_err("No root path specified");
    }
    else
    {
        // std::cout << "ROOT IN SET == "<< this->kwargs["root"][0] << std::endl;
        // std::cout << "PATH IN SET == "<< _req.path << std::endl;
        this->resourceFullPath = this->kwargs["root"][0].append(_req.path);
        //std::cout << "RESOURCE FULL PATH == "<<this->resourceFullPath << std::endl;

    }
    this->resourceType = getResourceType(); 
}