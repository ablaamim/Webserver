#include "../../MainInc/main.hpp"

bool isDirectoryEmpty(const std::string& folderPath)
{
    DIR* dir = opendir(folderPath.c_str());
    if (dir == NULL)
        return false;
    dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
        {
            closedir(dir);
            return false;
        }
    }
    closedir(dir);
    return true;
}

bool deleteFiles(const std::string& path, Response& resp)
{
    if (access(path.c_str(), F_OK) != 0)
        resp.serveERROR(_CS_404, _CS_404_m);
    if (access(path.c_str(), W_OK) != 0)
        resp.serveERROR(_CS_403, _CS_403_m);
    if (isDirectoryEmpty(path))
    {
        if (rmdir(path.c_str()) == 0)
            return true;
        else
            resp.serveERROR(_CS_409, _CS_409_m);
    } 
    else
    {
        if (remove(path.c_str()) == 0)
            return true;
        else
            resp.serveERROR(_CS_409, _CS_409_m);
    }
    return false;
}

void    Response::serveDELETE()
{
    try
    {
        if (this->_location.NoneUniqueKey["cgi-bin"].size() > 0)
            this->serveERROR("501", "Not Implemented");
        else if (deleteFiles(this->resourceFullPath, *this))
            this->sendResponse(HEADERS_ONLY);
    }
    catch(const std::exception& e)
    {
        Response::Response_err(e.what());
    }
}