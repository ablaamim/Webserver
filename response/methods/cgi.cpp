#include "../Response.hpp"


void    serveGetCGI(Response &resp)
{
    std::cout << "serveGetCGI" << std::endl;
    resp.serveERROR("501", "Not Implemented");
}

void    servePostCGI(Response &resp)
{
    std::cout << "servePostCGI" << std::endl;
}

void    Response::serveCGI()
{
    if (this->method == GET)
        serveGetCGI(*this);
    else if (this->method == POST)
        servePostCGI(*this);
}