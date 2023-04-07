#ifndef PARSING_HPP
#define PARSING_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <map>
#include <stdlib.h>

class ConfigFile
{
    private :
        std::vector<std::string> _lines;
        std::map<std::string, std::string> servers;
    
    public :
        ConfigFile();
        ConfigFile(std::string filename);
        ~ConfigFile();
        void read(std::string filename);
        std::vector<std::string> getLines();
        void parseServers();
        std::map<std::string, std::string> getServers();
};

#endif