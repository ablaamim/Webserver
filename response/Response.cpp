#include "Response.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>
#include <iomanip>


// void    Response::checkRequest()
// {
//     std::vector<std::string> allowedMethods = this->kwargs["allowed_methods"];
//     if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_req.params["Method"]) == allowedMethods.end())
//         this->serve(std::pair<std::string, std::string>("405", "Method Not Allowed"));
//     // if request method == POST
//     if (this->_req.params["Method"] == "POST")
//     {

//     }
// }

void    Response::serve(std::pair<std::string, std::string> status)
{
    std::string responseMessage;
    responseMessage += this->httpVersion + " " + status.first + " " + status.second + "\r\n";
    for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
        responseMessage += it->first + ": " + it->second + "\r\n";
    responseMessage += "\r\n";
    send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
}

std::string intToHexString(int number) {
    std::stringstream stream;
    stream << std::hex << number;
    return stream.str();
}

int getFileSize(std::string path)
{
    std::ifstream f;
    f.open(path.c_str(), std::ios::in | std::ios::binary);
    f.seekg(0, std::ios::end);
    return f.tellg();
}

Response::Response(Request req, int id, configurationSA::location location, char **env) : _req(req), clientSocket(id) ,_location(location), _env(env)
{
    std::cout << "Response constructor" << std::endl;

    //if (req.params["Method"] == "GET")
    //{
    //    std::cout << "Method is GET" << std::endl;
    //    this->handleGet();
    //}
    //else if (req.params["Method"] == "POST")
    //    this->handlePost();
    //else if (req.params["Method"] == "DELETE")
    //    this->handleDelete();
    //else
    //    std::cout << "Method is not GET, POST or DELETE" << std::endl;
    ////std::cout << "Response constructor" << std::endl;
    //
    //if (!this->_methods.empty())
    //    this->_methods.clear();
    //
    //this->_methods.insert(std::pair<std::string, void(Response::*)()>("GET", &Response::handleGet));
    //this->_methods.insert(std::pair<std::string, void(Response::*)()>("POST", &Response::handlePost));
    //this->_methods.insert(std::pair<std::string, void(Response::*)()>("DELETE", &Response::handleDelete));
    
    this->_req.print_params();

    // this->resourceFullPath = "/Users/afaris/Desktop/Webserver-1/slayer.mp4";
    // this->headers["content-type"] = "video/mp4";
    // this->resouceLength = getFileSize(this->resourceFullPath);
    // this->currentLength = 0;
    // this->lastChunkSize = 0;
    // this->isCompleted = false;
    // this->httpVersion = "HTTP/1.1";
    // this->headers.insert(std::pair<std::string, std::string>("Server", "webserv"));
    // this->status = std::pair<std::string, std::string>("200", "OK");
    // this->fd = open(this->resourceFullPath.c_str(), O_RDONLY);
    // std::cout << "fd: " << this->fd << std::endl;
};

Response::Response(int id) : clientSocket(id)
{
    ////std::cout << "Response constructor" << std::endl;
    //if (!this->_methods.empty())
    //    this->_methods.clear();
    //
    //this->_methods.insert(std::pair<std::string, void(Response::*)()>("GET", &Response::handleGet));
    //this->_methods.insert(std::pair<std::string, void(Response::*)()>("POST", &Response::handlePost));
    //this->_methods.insert(std::pair<std::string, void(Response::*)()>("DELETE", &Response::handleDelete));
//
    //// print method from request
//
    //if (this->_req.params["Method"] == "GET")
    //    std::cout << "Method is GET" << std::endl;
    //else if (this->_req.params["Method"] == "POST")
    //    std::cout << "Method is POST" << std::endl;
    //else if (this->_req.params["Method"] == "DELETE")
    //    std::cout << "Method is DELETE" << std::endl;
    //else
    //    std::cout << "Method is not GET, POST or DELETE" << std::endl;
    //
    //this->_req.print_params();

    this->resourceFullPath = "/Users/afaris/Desktop/Webserver-1/slayer.mp4";
    this->headers["content-type"] = "video/mp4";
    this->resouceLength = getFileSize(this->resourceFullPath);
    this->currentLength = 0;
    this->lastChunkSize = 0;
    this->isCompleted = false;
    this->httpVersion = "HTTP/1.1";
    this->headers.insert(std::pair<std::string, std::string>("Server", "webserv"));
    this->status = std::pair<std::string, std::string>("200", "OK");
    this->fd = open(this->resourceFullPath.c_str(), O_RDONLY);
    std::cout << "fd: " << this->fd << std::endl;
};

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

int getResourceType(std::string path, std::map<std::string, std::vector<std::string> > kwargs)
{
    if (kwargs.find("cgi-bin") != kwargs.end())
        return CGI;
    else if (kwargs.find("return") != kwargs.end())
        return REDIRECT;
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

Response::~Response()
{
     //std::cout << "Response destructor" << std::endl;
}

void    Response::generateBody()
{
    char buf[CHUNCK_SIZE + 1] = {0};
    std::ifstream f;
    f.open(this->resourceFullPath.c_str(), std::ios::in | std::ios::binary);
    f.seekg(this->currentLength, std::ios::beg);
    f.read(buf, CHUNCK_SIZE);
    this->lastChunkSize = f.gcount();
    this->currentLength += this->lastChunkSize;
    this->body = std::string(buf);
    std::cout << "body: " << this->body << std::endl;
}

void    Response::serve()
{
    std::string responseMessage;
    if (!isChunked)
    {
        responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
        std::map<std::string, std::string>::iterator it = this->headers.begin();
        while (it != this->headers.end())
        {
            responseMessage += it->first + ": " + it->second + "\r\n";
            it++;
        }
        if (this->resouceLength >= CHUNCK_SIZE)
            this->isChunked = true;
    }
    generateBody();
    responseMessage += "\r\n";
    responseMessage.append(this->body);
    if (this->currentLength >= this->resouceLength)
    {
        this->isCompleted = true;
        responseMessage += "\r\n";
    }
    send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
}

Response::Response(const Response &other)
{
    //std::cout << "Response copy constructor" << std::endl;
    // copy all the attributes of the other object to this object
    this->clientSocket = other.clientSocket;
    this->resourceFullPath = other.resourceFullPath;
    this->resouceLength = other.resouceLength;
    this->currentLength = other.currentLength;
    this->lastChunkSize = other.lastChunkSize;
    this->isCompleted = other.isCompleted;
    this->httpVersion = other.httpVersion;
    this->headers = other.headers;
    this->status = other.status;
    this->fd = other.fd;
    this->resourceType = other.resourceType;
    this->body = other.body;
    this->_req = other._req;
    this->_location = other._location;
    this->_client_ip = other._client_ip;
    this->_env = other._env;

}