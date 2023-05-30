#include "methods.hpp"


void    openFile(Response& resp)
{
    /* check if you have file permissions and set the appropriate status code (404, 403) */
    std::cout << "openFile" << std::endl;
    if(resp.fs)
        delete(resp.fs);
    resp.fs = new std::ifstream(resp.resourceFullPath.c_str(), std::ios::binary);
    if (resp.fs->good())
    {
        std::cout << "good" << std::endl;
        resp.fs->seekg(0, std::ios::end);
        std::streampos length = resp.fs->tellg();
        resp.resourceSize = static_cast<size_t>(length);
    }
    else
        resp.status = std::make_pair("404", "Not Found");
    
    /* if anything goes wrong, throw an exeception, NOTE: this will be catched in Webserv.cpp */
    if (resp.status.first != "200")
        throw std::runtime_error(resp.status.second);
}

void    serveFile(Response& resp)
{
    try
    {
        char buf[CHUNCK_SIZE];
        if (resp.currentSize == 0)
        {
            resp.headers["Content-Type"] = getContentType(resp.resourceFullPath);
            openFile(resp);
        }
        resp.fs->seekg(resp.currentSize, std::ios::beg);
        resp.fs->read(buf, CHUNCK_SIZE);
        resp.lastChunkSize = resp.fs->gcount();
        resp.currentSize += resp.lastChunkSize;
        if (resp.currentSize >= resp.resourceSize)
            resp.isCompleted = true;
        resp.body.append(buf, resp.lastChunkSize);
        resp.sendResponse(FULL);
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error(e.what());
    }
}

void    serveDirectory(Response& resp)
{
    std::cout << "serveDirectory" << std::endl;
    resp.headers["Content-Type"] = "text/html";
    resp.body = "<h1> Directory </h1> "; 
    resp.sendResponse(FULL);
}

void    serveCGI(Response& resp)
{
}

void    serveRedirect(Response& resp)
{
    resp.headers["Location"] = resp.resourceFullPath;
    resp.status = std::make_pair("301", "Moved Permanently");
    resp.sendResponse(HEADERS_ONLY);
}

void    Response::serveGET()
{
    try
    {
        if(this->resourceType == FILE)
            serveFile(*this);
        else if (this->resourceType == DIRECTORY)
            serveDirectory(*this);
        else if (this->resourceType == CGI)
            serveCGI(*this);
        else if (this->resourceType == REDIRECT)
            serveRedirect(*this);
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}