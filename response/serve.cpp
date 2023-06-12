#include "Response.hpp"

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
    if (send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0) <= 0)
        throw Response_err("send() failed");
    if (this->currentSize >= this->resourceSize)
        this->isCompleted = true;
    this->body.clear();
}

void Response::serveRedirect()
{
    std::vector<std::string> return_values = this->kwargs["return"];
    this->status.first = return_values[0];
    this->status.second = "";
    if (this->status.first == "301" || this->status.first == "302" || this->status.first == "303" || this->status.first == "307")
    {
        this->headers["Location"] = return_values[1];
        this->sendResponse(HEADERS_ONLY);
    }
    else
    {
        this->body = return_values[1];
        this->sendResponse(FULL);
    }
}

void Response::serve()
{
    try
    {
        if (needsRedirection(*this))
            return ;
        if (!this->indexChecked && this->resourceType == DIRECTORY && this->method != DELETE)
            lookForIndex(*this);
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
