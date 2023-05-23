#include "Response.hpp"

bool isDirectory(std::string path)
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

// this function returns the type of the resource

int getResourceType(std::string path, std::map<std::string, std::vector<std::string> > kwargs)
{
    // check if kwargs has cgi-bin or not
    // if yes, return CGI
    if (kwargs.find("cgi-bin") != kwargs.end())
        return CGI;
    else if (kwargs.find("return") != kwargs.end())
        return REDIRECT;
    // check if the path is a directory
    // if yes, return DIRECTORY
    if (isDirectory(path))
        return DIRECTORY;
    return FILE;
}

void    Response::init()
{
    this->resourceType = getResourceType(this->resourceFullPath, this->kwargs);
    std::cout << "resource type: " << this->resourceType << std::endl;
    this->currentLength = 0;
    this->resouceLength = 0;
}

// Response::Response(int id, configurationSA::location location, char **env)
// {
//     this->httpVersion = "HTTP/1.1";
//     this->clientSocket = id;
//     this->location = location;
//     this->resourceFullPath = "/Users/afaris/abs/response/";
//     this->isCompleted = false;
//     this->headers["Server"] = "webserv";
//     this->headers["Content-Type"] = "text/html";
//     this->init();
// }

Response::Response(int id)
{
    //this->clientSocket = id;
    this->isCompleted = false;
    this->currentLength = 0;
    this->resouceLength = 0;
    this->resourceType = NONE;
    this->resourceFullPath = "/Users/afaris/abs/response/";
    this->httpVersion = "HTTP/1.1";
    this->status = std::make_pair("200", "OK");
    this->headers["Server"] = "webserv";
    this->headers["Content-Type"] = "text/html";
    this->init();
}

Response::~Response()
{
     //std::cout << "Response destructor" << std::endl;
}


void    Response::serve()
{
    std::string responseMessage;

    responseMessage = this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
    for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
        responseMessage += it->first + ": " + it->second + "\r\n";
    responseMessage += "\r\n";

    responseMessage += this->body;
    send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
    this->isCompleted = true;
}


Response::Response(const Response &other)
{
    this->clientSocket = other.clientSocket;
    this->isCompleted = other.isCompleted;
    this->resouceLength = other.resouceLength;
    this->currentLength = other.currentLength;
    this->resourceType = other.resourceType;
    this->resourceFullPath = other.resourceFullPath;
    this->httpVersion = other.httpVersion;
    this->body = other.body;
    this->status = other.status;
    this->headers = other.headers;
    this->kwargs = other.kwargs;
}