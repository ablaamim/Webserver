#include "methods.hpp"

void    Response::serveGET()
{
    this->body = "<h1>Hello from get function</h1>";
}