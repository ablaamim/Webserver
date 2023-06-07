#include "methods.hpp"


bool    uploadSupported(Response& resp)
{
    /*
        check if upload is supported
        by checking if the upload_pass exists in the kwargs (matched location)
    */
    std::map<std::string, std::vector<std::string> >::iterator it = resp.kwargs.find("upload");
    if (it == resp.kwargs.end())
        return false;
    return true;
}

void    servePostCGI(Response& resp)
{
    std::cout << "servePostCGI" << std::endl;
}

void    servePostFile(Response& resp)
{
    std::string full_path = resp.kwargs["upload"][0] + resp._req.params["Url"] + "." + resp._req.params["Content-Extension"];
    //std::cout << "full_path: " << full_path << std::endl;
    std::ifstream file(full_path.c_str(), std::ios::binary);
    if (file.good())
    {
        file.close();
        resp.serveERROR("409", "Conflict");
        return ;
    }
    std::ifstream source(resp._req.file_body_name, std::ios::binary);
    if (source.good())
    {
        std::ofstream destination(full_path, std::ios::binary);
        destination << source.rdbuf();
        source.close();
        destination.close();
        resp.status = std::make_pair("201", "Created");
    }
    else
        resp.status = std::make_pair("520", "Web Server Returned an Unknown Error");
    resp.sendResponse(HEADERS_ONLY);
}

void    servePostDirectory(Response& resp)
{
    std::cout << "servePostDirectory" << std::endl;
}

void    Response::servePOST()
{
    try
    {
        std::cout << "servePOST" << std::endl;
        if (uploadSupported(*this))
        {
            if (this->resourceType == CGI)
                servePostCGI(*this);
            else if (this->resourceType == DIRECTORY)
                this->serveERROR("409", "Conflict");
            else
                servePostFile(*this);
        }
        else
            this->serveERROR("403", "Forbidden");
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}

