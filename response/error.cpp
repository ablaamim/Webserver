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
    resp.body = "<style>@import url('https://fonts.googleapis.com/css?family=Poppins:400,500,600,700&display=swap');\
                *{box-sizing: border-box;font-family: 'Poppins', Arial, Helvetica, sans-serif;margin: 0;padding: 0;}\
                body{height: 100vh;background: -webkit-repeating-linear-gradient(-45deg, #71b7e6, #69a6ce, #b98acc, #ee8176, #b98acc, #69a6ce, #9b59b6);background-size: 400%;display: flex;align-items: center;justify-content: center;}\
                .content {width: 75%;height: 45%;background: #fff;box-shadow: 0px 5px 10px rgba(0,0,0,0.1);max-width: 600px;text-align: center;display: flex;justify-content: center;align-items: center;flex-direction: column;}\
                h1.title {font-size: 7vw;line-height: 1em;position: relative;}\
                h1.title:after{position: absolute;content: '"+resp.status.first+"';top: 0;left: 0;right: 0;background: -webkit-repeating-linear-gradient(-45deg, #71b7e6, #69a6ce, #b98acc, #ee8176, #b98acc, #69a6ce, #9b59b6);background-size: 400%;-webkit-background-clip: text;-webkit-text-fill-color: transparent;text-shadow: 1px 1px 2px rgba(255,255,255,0.25);animation: animate 10s ease-in-out infinite;}\
                @keyframes animate {0%{background-position: 0 0;}25%{background-position: 100% 0;}50%{background-position: 100% 100%;}75%{background-position: 0% 100%;}100%{background-position: 0% 0%;}}\
                .text{font-size: 2.2em;color: #0d0d0d;margin: 50px 0;}\
                </style>";
    resp.body += "</head>\n";
    resp.body += "<body>\n";
    resp.body += "<div class=\"content\"><h1 style=\"text-align: center;\" class=\"title\" >" + resp.status.first + "</h1>\n";
    resp.body += "<div class=\"text\" >" + resp.status.second + "</div>\n";
    resp.body += "<address style=\"text-align: center;\" >" + resp.headers["Server"] + "</address>\n";
    resp.body += "</div></body>\n";
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