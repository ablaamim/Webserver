#include "Response.hpp"

void    Response::checkRequest()
{
    std::vector<std::string> allowedMethods = this->kwargs["allowed_methods"];

    if (_req.method != "GET" && _req.method != "POST" && _req.method != "DELETE")
        this->serveERROR("501", "Not Implemented");
    if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_req.params["Method"]) == allowedMethods.end())
        this->serveERROR("405", "Method Not Allowed");
    if (this->_req.version != "HTTP/1.1")
        this->serveERROR("505", "HTTP Version Not Supported");
    std::cout << "CONTENT LENGTH = " << _req.content_length << std::endl;
    //if (this->_req.content_length == 0)
      //  this->serveERROR("411", "Length Required");
}
