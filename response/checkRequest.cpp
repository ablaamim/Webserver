#include "Response.hpp"

void    checkHTTP(Response& resp)
{
    if (resp._req.version.substr(0, 5) != "HTTP/")
        resp.serveERROR("505", "HTTP Version Not Supported");
    if (resp._req.version.substr(5) != "1.1")
        resp.serveERROR("400", "BAD REQUEST");
}

void    Response::checkRequest()
{
    // root iterator
    std::vector<std::string> allowedMethods = this->kwargs["allowed_methods"];
    if (_req.method != "GET" && _req.method != "POST" && _req.method != "DELETE")
        this->serveERROR("501", "Not Implemented");
    if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_req.params["Method"]) == allowedMethods.end())
        this->serveERROR("405", "Method Not Allowed");
    checkHTTP(*this);
    if (this->_req.params.find("Host") == this->_req.params.end())
        this->serveERROR("400", "Bad Request");
    /*
        CHECKING IF THE REQUEST BODY IS LARGER THAN MAX_BODY_SIZE FROM CONFIG FILE
        WE ARE CHECKING JUST THE DEFAULT VALUE, NOT THE VALUE FROM THE CONFIG FILE FOR NOW
        NEEDS TO BE FIXED ...
    */
    if (this->_req.content_length > MAX_BODY_SIZE)
        this->serveERROR("413", "Request Entity Too Large");
    
}
