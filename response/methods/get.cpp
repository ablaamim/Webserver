#include "methods.hpp"

void    Response::handleGet()
{
    this->body = "<h1>Hello from get function</h1>";
}