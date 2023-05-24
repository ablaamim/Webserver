#include "Response.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>
#include <iomanip>

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

Response::Response(abstract_req req, int id, configurationSA::location location, std::string _client_ip, char **env) : _req(req), clientSocket(id) ,_location(location), _client_ip(_client_ip) , _env(env)
{
    // init method map
    if (!this->_methods.empty())
        this->_methods.clear();
    this->_methods.insert(std::pair<std::string, void(Response::*)()>("GET", &Response::handleGet));
    this->_methods.insert(std::pair<std::string, void(Response::*)()>("POST", &Response::handlePost));
    this->_methods.insert(std::pair<std::string, void(Response::*)()>("DELETE", &Response::handleDelete));

    this->resourceFullPath = "/Users/afaris/Desktop/Webserver/response/example.html";
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
    char buf[BUFFER_SIZE] = {0};
    this->lastChunkSize = read(this->fd, buf, BUFFER_SIZE - 1);
    std::cout << "last chunk size: " << this->lastChunkSize << std::endl;
    buf[this->lastChunkSize] = '\0';
    if (this->lastChunkSize == -1)
        throw std::runtime_error("Error reading file");
    this->currentLength += this->lastChunkSize;
    this->body.append(buf);
}

void    Response::serve()
{
    std::string responseMessage;
    this->headers["transfer-encoding"] = "chunked";
    this->headers["content-type"] = "text/html";
    responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
    std::map<std::string, std::string>::iterator it = this->headers.begin();
    while (it != this->headers.end())
    {
        responseMessage += it->first + ": " + it->second + "\r\n";
        it++;
    }
    generateBody();
    responseMessage += "\r\n";
    responseMessage += intToHexString(this->lastChunkSize) + "\r\n";
    responseMessage.append(this->body) + "\r\n";
    responseMessage += "0\r\n\r\n";
    send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
    if (this->currentLength >= this->resouceLength)
        this->isCompleted = true;
}

Response::Response(const Response &other)
{
    std::cout << "Response copy constructor" << std::endl;
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