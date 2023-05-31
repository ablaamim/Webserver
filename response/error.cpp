#include "Response.hpp"

std::string    getCustomErrorPage(Response& resp)
{
    /* 
        trying to find custom error page in kwargs
        if found return it else return empty string
    */
    std::map<std::string, std::vector<std::string> >::iterator it;
    it = resp.kwargs.find("error_pages");
    /* kwargs are empty */
    //resp.print_kwargs();
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
    /*
        find custom error page in kwargs (if exists)
        otherwise, generate default error page with current status code and message,
        send response, throw exception, which will be catched in Webserv.cpp.
    */
    std::string errorPage;

    this->status.first = errorCode;
    this->status.second = errorMsg;
    errorPage = getCustomErrorPage(*this);
    if (errorPage.length() > 0)
    {
        this->headers["Location"] = errorPage;
        this->sendResponse(HEADERS_ONLY);
    }
    else
        generateDefaultErrorPage(*this);
    throw Response_err(this->status.second);
}