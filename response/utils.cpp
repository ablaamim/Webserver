#include "utils.hpp"

bool    isDirectory(std::string path)
{
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else
        return false;
}


std::string     pathJoin(std::string path1, std::string path2)
{
    if (path1[path1.length() - 1] != '/')
        path1.append("/");
    if (path2[0] == '/')
        path2.erase(0, 1);
    path1.append(path2);
    return (path1);
}