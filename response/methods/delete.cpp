#include "methods.hpp"
#include <sys/stat.h>

bool hasWritePermission(const std::string& file_path)
{
    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) == -1)
    {
        std::cout << "Error getting file status for: " << file_path << std::endl;
        return false;
    }
    
    // Check if the file is writable by the current user
    if ((file_stat.st_mode & S_IWUSR) != 0)
    {
        return true;
    }

    return false;
}

void deleteFiles(const std::string& directory)
{
    DIR* dir = opendir(directory.c_str());
    if (dir == NULL)
    {
        std::cout << "Error opening directory: " << directory << std::endl;
        return;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string file_name = entry->d_name;
        std::string full_path = directory + "/" + file_name;

        struct stat file_stat;
        if (stat(full_path.c_str(), &file_stat) == -1)
        {
            std::cout << "Error getting file status for: " << full_path << std::endl;
            continue;
        }

        if (S_ISREG(file_stat.st_mode))
        {
        
            if (hasWritePermission(full_path))
            {
                if (remove(full_path.c_str()) != 0)
                {
                    std::cout << "Error deleting file: " << full_path << std::endl;
                } 
                else
                {
                    std::cout << "Deleted file: " << full_path << std::endl;
                }
            }
            else
            {
                //this->serveERROR("403", "Forbidden");
            }
        }
    }
    closedir(dir);
}

void    Response::serveDELETE()
{
    try
    {
        deleteFiles(this->resourceFullPath);
        this->status.first = "200";
    }
    catch(const std::exception& e)
    {
        this->status.first = "500";
        throw std::runtime_error(e.what());
    }
    this->sendResponse(HEADERS_ONLY);
    
}