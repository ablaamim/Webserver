#include "Response.hpp"

int     Response::getResourceType()
{
    if (this->kwargs.find("cgi-bin") != this->kwargs.end())
        return CGI;
    else if (this->kwargs.find("return") != this->kwargs.end())
    {
        //std::string returnCode = this->kwargs["return"][0];
        //std::cout << "RETURN CODE == " << returnCode << std::endl;
        return REDIRECT;
    }
        
    if (isDirectory(this->resourceFullPath.c_str()))
        return DIRECTORY;
    return FILE;
}

void    Response::setResourceInfo()
{
    if (this->kwargs["root"].empty())
        this->serveERROR("404", "Not Found");
    else
    {
        // std::cout << "ROOT IN SET == "<< this->kwargs["root"][0] << std::endl;
        // std::cout << "PATH IN SET == "<< _req.path << std::endl;
        this->resourceFullPath = this->kwargs["root"][0].append(_req.path);
        //std::cout << "RESOURCE FULL PATH == "<<this->resourceFullPath << std::endl;

    }
    this->resourceType = getResourceType(); 
}