#include "Response.hpp"

void    Response::sendResponse(int mode)
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
    /* if mode != HEADERS_ONLY, keep sending generated body content */
    if (mode == FULL)
        responseMessage += this->body;
    if (send(this->clientSocket, this->responseMessage.c_str(), this->responseMessage.length(), 0) <= 0)
        throw std::runtime_error("Send failed");
    if (this->currentSize >= this->resourceSize)
        this->isCompleted = true;
    this->body.clear();
}

// this is the main entry point for the response

void    Response::serve()
{
    if (this->method == GET)
        this->serveGET();
    else if (this->method == POST)
        this->servePOST();
    else if (this->method == DELETE)
        this->serveDELETE();
}
