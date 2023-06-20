#include "Response.hpp"

std::string    getCustomErrorPage(Response& resp)
{
    std::map<std::string, std::vector<std::string> >::iterator it;
    it = resp.kwargs.find("error_pages");
    if (it != resp.kwargs.end())
    {
        std::vector<std::string> errorPages = it->second;
        if (errorPages[0] == resp.status.first)
            return (errorPages[1]);
    }
    return ("");
}

void    generateDefaultErrorPage(Response& resp)
{
    resp.body += "<!DOCTYPE html>\n";
    resp.body += "<html>\n";
    resp.body += "<head>\n";
    resp.body += "<title>" + resp.status.first + " " + resp.status.second + "</title>\n";
    resp.body += "</head>\n";
    resp.body += "<body>\n";
    resp.body += "<h1 style=\"text-align: center;\" >" + resp.status.first + " " + resp.status.second + "</h1>\n";
    resp.body += "<hr>\n";
    resp.body += "<address style=\"text-align: center;\" >" + resp.headers["Server"] + "</address>\n";
    resp.body += "</body>\n";
    resp.body += "</html>\n";
    resp.headers["Content-Type"] = "text/html";
    resp.headers["Content-Length"] = std::to_string(resp.body.size());
    resp.sendResponse(FULL);
}

void    Response::serveERROR(std::string errorCode, std::string errorMsg)
{
    std::string errorPage;

    this->body.clear();
    this->status.first = errorCode;
    this->status.second = errorMsg;
    errorPage = getCustomErrorPage(*this);
    if (errorPage.length() > 0 && this->customErrorFound == false)
    {
        std::cout << "Custom error page found: " << errorPage << std::endl;
        this->resourceFullPath = pathJoin(this->kwargs["root"][0], errorPage);
        this->method = GET;
        this->customErrorFound = true;
        this->resourceType = getResourceType();
    }
    else
        generateDefaultErrorPage(*this);
    throw Response_err(this->status.second);
}