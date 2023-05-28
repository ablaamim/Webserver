#include "Response.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>
#include <iomanip>

std::map<std::string, std::string>  mime_types;                // map of (extension, mime_type)

Response::Response(void) { /* DEFAULT CONSTRUCTOR */ };

void Response::insert_Location_kwargs(std::string key, std::vector<std::string> value)
{
    this->kwargs.insert(std::pair<std::string, std::vector<std::string> >(key, value));
}

void    Response::init()
{
    mime_types.insert(_mime_types, _mime_types + sizeof(_mime_types) / sizeof(_mime_types[0]));
    this->httpVersion = this->_req.version;
    this->status = std::make_pair("200", "OK");
    this->headers["Server"] = "Webserver/1.0";
    this->currentSize = 0;
    this->resourceSize = 0;
    this->lastChunkSize = 0;
    this->isCompleted = false;
    this->isChunked = false;
    try
    {
        /* Check if request is valid or not and set information about the requested resource */
        this->checkRequest();
        this->setResourceInfo();
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}

Response::Response(Request req, int id, configurationSA::location location, char **env) : _req(req), clientSocket(id) ,_location(location), _env(env)
{
    /*
        Here, we will initialize the response instance.
        if any error occurs, we will throw an exception
        and catch it in (Webserver.cpp) and send serveEmpty() which will serve a response
        that has the actual status code only (no body),
        we will see how to render the body later (error pages)
    */
};


Response::~Response()
{
    if (this->fs.is_open())
        this->fs.close();
}

void    Response::serveFile()
{
    std::string responseMessage;
    char buf[CHUNCK_SIZE];
    if (!isChunked)
    {
        responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
        std::map<std::string, std::string>::iterator it = this->headers.begin();
        while (it != this->headers.end())
        {
            responseMessage += it->first + ": " + it->second + "\r\n";
            it++;
        }
        if (this->resourceSize >= CHUNCK_SIZE)
            this->isChunked = true;
        responseMessage += "\r\n";
        send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
    }
    this->fs.seekg(this->currentSize, std::ios::beg);
    this->fs.read(buf, CHUNCK_SIZE);
    this->lastChunkSize = this->fs.gcount();
    this->currentSize += this->lastChunkSize;
    send(this->clientSocket, buf, this->lastChunkSize, 0);
    if (this->currentSize >= this->resourceSize)
        this->isCompleted = true;
}

Response::Response(const Response &other)
{
    this->httpVersion = other.httpVersion;
    this->status = other.status;
    this->headers = other.headers;
    this->resourceType = other.resourceType;
    this->resourceFullPath = other.resourceFullPath;
    this->resourceSize = other.resourceSize;
    this->currentSize = other.currentSize;
    this->lastChunkSize = other.lastChunkSize;
    this->isCompleted = other.isCompleted;
    this->isChunked = other.isChunked;
    this->clientSocket = other.clientSocket;
    this->resourceSize = other.resourceSize;
    this->method = other.method;
    this->_env = other._env;
    this->_req = other._req;
    this->_location = other._location;
    this->fs = std::ifstream(other.resourceFullPath);

}