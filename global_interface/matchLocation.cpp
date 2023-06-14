#include "../MainInc/main.hpp"


bool    startsWith(std::string str, std::string prefix)
{
    if (str.length() < prefix.length())
        return false;
    return (str.compare(0, prefix.length(), prefix) == 0);
}

std::string getMaxLocation(std::vector<std::string>& matchedLocations)
{
    std::string result;
    for (std::vector<std::string>::iterator it = matchedLocations.begin(); it != matchedLocations.end(); it++)
    {
        if (it->length() > result.length())
            result = *it;
    }
    return (result);
}


configurationSA::location Webserv::match_location(std::string trgt, configurationSA::Server server)
{
    typedef std::map<std::string, configurationSA::location>    type_location;
    configurationSA::location                                   result;
    std::vector<std::string>                                    matchedLocations;
    std::string                                                 maxLocation; 

    type_location location = server.get_location();
    for (type_location::iterator it = location.begin(); it != location.end(); it++)
    {
        std::string locationPath = it->first;
        if (startsWith(trgt, locationPath))
            matchedLocations.push_back(locationPath);
    }
    if (matchedLocations.size() == 0)
        result = location[server.first_location_key];
    else
        result = location[getMaxLocation(matchedLocations)];
    if (result.UniqueKey["root"].empty())
        result.UniqueKey["root"].push_back(server.root);
    if (result.UniqueKey["allowed_methods"].empty())
        result.UniqueKey["allowed_methods"].push_back("GET");
    result.print_none_unique_key();
    result.print_unique_key();
    return (result);
}