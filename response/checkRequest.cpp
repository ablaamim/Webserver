#include "Response.hpp"

void    Response::checkRequest()
{
    if (_req.error)
        this->serveERROR(std::to_string(_req.error), _req.erro_msg);
}
