#include "Response.hpp"

void    Response::serveRedirect()
{
    std::vector<std::string> return_values = this->kwargs["return"];
    this->status.first = return_values[0];
    this->status.second = "";
    if (this->status.first == _CS_301 || this->status.first == _CS_302 || \
    this->status.first == _CS_303 || this->status.first == _CS_307)
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

bool    needsRedirection(Response& resp)
{
    if (resp.resourceType == REDIRECT)
    {
        resp.serveRedirect();
        return true;
    }
    else if (resp.resourceType == DIRECTORY)
    {
        if (resp.cleanPath[resp.cleanPath.length() - 1] != '/')
        {
            resp.cleanPath.append("/");
            if (resp.queryParams.empty() == false)
            {
                resp.cleanPath.append("?");
                resp.cleanPath.append(resp.queryParams);
            }
            resp.headers["Location"] = resp.cleanPath;
            resp.status = std::make_pair(_CS_301, _CS_301_m);
            resp.sendResponse(HEADERS_ONLY);
            return true;
        }
    }
    return false;
}