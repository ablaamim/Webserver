#include "Response.hpp"

void    Response::serveRedirect()
{
    /*
        look if return code is 301 or 302 or 303 or 307
        if yes, send response with headers only with Location header set to the value of return
        else, send full response with headers and body (body is set to the value of return)
    */
    std::vector<std::string> return_values = this->kwargs["return"];
    this->status.first = return_values[0]; // status code
    this->status.second = ""; // let browser decide the status message
    if (this->status.first == "301" || this->status.first == "302" || this->status.first == "303" || this->status.first == "307")
    {
        this->headers["Location"] = return_values[1]; // location
        this->sendResponse(HEADERS_ONLY);
    }
    else
    {
        this->body = return_values[1]; // body
        this->sendResponse(FULL);
    }
}

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
    if (send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0) <= 0)
        throw Response_err("send() failed");
    if (this->currentSize >= this->resourceSize)
        this->isCompleted = true;
    this->body.clear();
}

void    Response::serve()
{
    if (this->initialized == false)
    {
        typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t; 
        for (NoneUniqueKey_t::iterator it = _location.NoneUniqueKey.begin(); it != _location.NoneUniqueKey.end(); it++)
        {
                std::string key = it->first;
                std::vector<std::string> values;
                //std::cout << "key = " << key << std::endl;
                std::map<std::string, std::vector<std::string> > ::iterator it_map = it->second.begin();
                while(it_map != it->second.end())
                {
                    values.push_back(it_map->first);
                    std::vector<std::string>::iterator vec_iter = it_map->second.begin();
                    while (vec_iter != it_map->second.end())
                    {
                        values.push_back(*vec_iter);
                        vec_iter++;
                    }
                    it_map++;
                }
                this->kwargs.insert(std::make_pair(key, values));
        }
        for (std::map<std::string, std::vector<std::string> >::iterator it = _location.UniqueKey.begin(); it != _location.UniqueKey.end(); it++)
        {
            std::string key = it->first;
            std::vector<std::string> values = it->second;
            this->kwargs.insert(std::make_pair(key, values));
        }
    }
    try
    {
        /* 
            check if there is a redirection before serving the resource
        */
        if (this->kwargs.find("return") != this->kwargs.end())
            this->serveRedirect();
        else
        {
            if (this->initialized == false)
                this->setResourceInfo();
            if (this->method == GET)
                this->serveGET();
            else if (this->method == POST)
                this->servePOST();
            else if (this->method == DELETE)
                this->serveDELETE();
        }
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}
