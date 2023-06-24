#include "../../MainInc/main.hpp"

bool    uploadSupported(Response& resp)
{
    std::map<std::string, std::vector<std::string> >::iterator it = resp.kwargs.find("upload_pass");
    if (it == resp.kwargs.end())
        return false;
    return true;
}

void    servePostFile(Response& resp)
{
    size_t in = resp._req.params["Content-Extension"].rfind('-');
    std::string ex = resp._req.params["Content-Extension"];

    if (in != std::string::npos)
        ex = resp._req.params["Content-Extension"].substr(in + 1);
    std::string filename = resp._req.params["Url"] + "." + ex;
    std::string full_path = pathJoin(resp.kwargs["upload_pass"][0], filename);
    std::ifstream file(full_path.c_str(), std::ios::binary);
    if (file.good())
    {
        file.close();
        resp.serveERROR(_CS_409, _CS_409_m);
        return ;
    }
    std::ifstream source(resp._req.file_body_name, std::ios::binary);
    if (source.good())
    {
        std::ofstream destination(full_path, std::ios::binary);
        destination << source.rdbuf();
        source.close();
        destination.close();
        resp.status = std::make_pair(_CS_201, _CS_201_m);
    }
    else
        resp.status = std::make_pair(_CS_520, _CS_520_m);
    resp.sendResponse(HEADERS_ONLY);
}

void    Response::servePOST()
{
    try
    {
        if (this->isCGI)
            this->serveCGI();
        else if (uploadSupported(*this))
        {
            if (this->resourceType == DIRECTORY)
                this->serveERROR(_CS_409, _CS_409_m);
            else
                servePostFile(*this);
        }
        else
            this->serveERROR(_CS_403, _CS_403_m);
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}

