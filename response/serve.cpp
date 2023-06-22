#include "Response.hpp"

void    Response::sendCGIResponse()
{
    std::string responseMessage;
    if (this->cgi.firstCall)
    {
        this->cgi.firstCall = false;
        if(this->body.find("Status: ") == 0)
        {
            std::string status = this->body.substr(this->body.find("Status: ") + 8, 3);
            this->status.first = status;
            this->status.second = "";
            //this->body.erase(0, this->body.find("\r\n\r\n") + 4);
        }
        responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
        for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
            responseMessage += it->first + ": " + it->second + "\r\n";
    }
    responseMessage += this->body;

    //std::cerr << "responseMessage: " << responseMessage << std::endl;
    this->body.clear();
    if (!this->isCompleted) 
    {
        if (send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0) <= 0)
            throw Response_err("send() failed here in sendCGIResponse()");
    }
}

void Response::sendResponse(int mode)
{
    std::string responseMessage;

    if (!isChunked)
    {
        responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
        for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
            responseMessage += it->first + ": " + it->second + "\r\n";
        responseMessage += "\r\n";
        this->isChunked = true;
    }
    if (mode == FULL)
        responseMessage += this->body;
    if (this->currentSize >= this->resourceSize)
        this->isCompleted = true;
    if (responseMessage.length() > 0)
    {
        if (send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0) <= 0)
            throw Response_err("send() failed here in sendResponse()");
    }
    this->body.clear();
}

void Response::serve()
{
    try
    {
        if (this->method == GET)
            this->serveGET();
        else if (this->method == POST)
            this->servePOST();
        else if (this->method == DELETE)
            this->serveDELETE();
    }
    catch (const std::exception &e)
    {
        throw Response_err(e.what());
    }
}
