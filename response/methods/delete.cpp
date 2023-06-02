#include "methods.hpp"
#include <sys/stat.h>

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
            return false;  // Directory is not empty
        }
    }
    closedir(dir);
    return true;  // Directory is empty
}

bool deleteFiles(const std::string& path, Response& resp)
{
    if (access(path.c_str(), F_OK) != 0)
        resp.serveERROR("404", "Not Found");
    if (access(path.c_str(), W_OK) != 0)
        resp.serveERROR("403", "Forbidden");
    if (isDirectoryEmpty(path))
    {
        if (rmdir(path.c_str()) == 0)
            return true;
        else
            resp.serveERROR("409", "Conflict");
    } 
    else
    {
        if (remove(path.c_str()) == 0)
            return true;
        else
            resp.serveERROR("409", "Conflict");
    }
    return false;
}

void    Response::serveDELETE()
{
    try
    {
        //std::cout << "DELETE" << std::endl;
        //std::cout << this->_location.NoneUniqueKey["cgi-bin"][0] << std::endl;
        if (this->_location.NoneUniqueKey["cgi-bin"].size() > 0)
        {
            //std::cout << "CGI IS NOT EMPTY" << std::endl;
            this->serveERROR("501", "Not Implemented");
        }
        //std::cout << "END OF CHECK" << std::endl;
        if (deleteFiles(this->resourceFullPath, *this))
            this->sendResponse(HEADERS_ONLY);
    }
    catch(const std::exception& e)
    {
        Response::Response_err(e.what());
    }
}