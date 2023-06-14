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
    if (_req.error)
        this->serveERROR(std::to_string(_req.error), _req.erro_msg);
    checkHTTP(*this);
}
