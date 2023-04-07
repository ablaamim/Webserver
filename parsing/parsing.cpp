#include "parsing.hpp"

ConfigFile::ConfigFile()
{
}

ConfigFile::ConfigFile(std::string filename)
{
    read(filename);
}

ConfigFile::~ConfigFile()
{
}

void ConfigFile::read(std::string filename)
{
    std::ifstream file(filename.c_str());
    std::string line;

    if (file.is_open() == false)
    {
        std::cout << "Error: file " << filename << " not found" << std::endl;
        return ;
    }
    while (std::getline(file, line))
        _lines.push_back(line);
}

std::vector<std::string> ConfigFile::getLines()
{
    return (_lines);
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;

    while (std::getline(ss, item, delim))
        elems.push_back(item);
    return (elems);
}

std::map<std::string, std::string> ConfigFile::getServers()
{
    return (servers);
}

void ConfigFile::parseServers()
{
    std::map<std::string, std::string> *Servers;
    std::vector<std::string> tmp;
    std::string key;
    std::string value;

    for (std::vector<std::string>::iterator it = _lines.begin(); it != _lines.end(); ++it)
    {
        tmp = split(*it, ' ');
        key = tmp[0];
        value = tmp[1];
        servers[key] = value;
    }
    for (std::map<std::string, std::string>::iterator it = servers.begin(); it != servers.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;
}