#include "libcpp.hpp"

// Check if a string is a digit or not.

bool isDigit(const std::string &str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++)
    {   
        if (!isdigit(*it))
        {
           return false;
        }
    }
    return true;
}

// Split a string into a vector of strings using a delimiter.

std::vector<std::string> split(std::string str, std::string delim)
{
    std::string              token;
    std::vector<std::string> list;
    size_t                      pos;
    
    pos = 0;
    while ((pos = str.find(delim)) != std::string::npos)
    {
        token = str.substr(0x0, pos);
		if (!token.empty())
        	list.push_back(token);
        str.erase(0, pos + delim.length());
    }
	if (!str.empty())
    	list.push_back(str);
    return (list);
}

std::string to_string(int nbr)
{
    std::ostringstream    stream;

    stream << nbr;
    return (stream.str());
}