#include "../Response.hpp"


void    Response::openFile()
{
    std::ifstream file(this->resourceFullPath.c_str(), std::ios::binary);
    
    if (file.good())
    {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        this->resourceSize = static_cast<size_t>(length);
    }
    else
        this->status = std::make_pair("404", "Not Found");
    if (this->status.first != "200")
        throw Response_err("File not found");
}

int     Response::getResourceType()
{
    if (this->kwargs.find("cgi-bin") != this->kwargs.end())
        return CGI;
    else if (this->kwargs.find("return") != this->kwargs.end())
        return REDIRECT;
    if (isDirectory(path))
        return DIRECTORY;
    return FILE;
}

void    Response::checkResource()
{
    /* try to check the file and set data about it (e.g. ifstream instance, size ... ) */
    try
    {
        if (resourceType == FILE)
            this->openFile();
    }
    catch(const std::exception& e)
    {
        // this will be caught in Webserver.cpp, and serveEmpty() will be called. instead of serve()
        throw Response_err(e.what());
    }
}

void    Response::setResourceInfo()
{
    this->resourceType = FILE;
    this->resourceFullPath = this->kwargs["root"][0].append(_req.path);    
    this->headers["Content-Type"] = mime_types[this->resourceFullPath.substr(this->resourceFullPath.find_last_of('.'))];
    //this->headers["Content-Length"] = std::to_string(this->resourceSize);
}