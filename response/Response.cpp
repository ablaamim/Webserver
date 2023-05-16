#include "Response.hpp"

Response::Response(int fd)
{
    std::cout << "Response constructor" << std::endl;
    
    //this->_location = location;
    
    //this->_location.print_unique_key();

    this->socketFd = fd;

    std::cout << socketFd << std::endl; // CORRECT SOCKET FD IS PRINTED
    
    this->body = "";
    this->fullPath = "";
    this->httpVersion = "HTTP/1.1";
    this->resourceType = 0;
    this->status = std::make_pair("200", "OK");
    this->statusList[200] = "OK";
    this->headers["Server"] = "webserv";
}



Response::Response()
{
    //statusListInitialization(this->statusList);
}

Response::~Response()
{
    std::cout << "Response destructor" << std::endl;
}

void    Response::generate()
{
    std::string responseMessage;

    responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second;
}
