#include "Response.hpp"

void    Response::checkRequest()
{
    std::vector<std::string> allowedMethods = this->kwargs["allowed_methods"];

    if (_req.method != "GET" && _req.method != "POST" && _req.method != "DELETE")
    {
        this->status = std::make_pair("501", "Not Implemented");        
        throw Response_err("Method not implemented");
    }
    if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_req.params["Method"]) == allowedMethods.end())
    {
        this->status = std::make_pair("405", "Method Not Allowed");
        throw Response_err("Method not allowed");
    }
    if (this->_req.version != "HTTP/1.1")
    {
        this->status = std::make_pair("505", "HTTP Version Not Supported");
        throw Response_err("HTTP Version not supported");
    }
    //this->_req.params["Host"] = ""; // for testing purposes
    if (this->_req.params["Host"] != "localhost" || this->_req.params["Host"] == "")
    {
        this->status = std::make_pair("400", "Bad Request");
        throw Response_err("Bad Request");
    }
    //if (this->_req.params[]
    // nice, but more tests to be added ...
}
