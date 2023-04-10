#include "ConfigurationParser.hpp"

configurationSA::configurationSA()
{
}

/*
void configurationSA::conf::init_data(void)
{
   if (_data.size() != 0)
       return ;
   std::string allowedMethods[] = {"GET", "POST", "DELETE"};
   std::string autoindex[] = {"on", "off"};
   std::string ErrorPages[] = {"404", "403", "405"}; // NEEDS TO BE CHANGED // 404 = not found, 403 = forbidden, 405 = method not allowed
   std::string returnCode[] = {"404", "403", "405"}; // NEEDS TO BE CHANGED // 404 = not found, 403 = forbidden, 405 = method not allowed
    std::pair<std::string, rawConf> data[] =
    {
        std::make_pair("ServerName", rawConf({UNIQUE_KEYTYPE})),
        std::make_pair("Listen", rawConf({UNIQUE_KEYTYPE})),
        std::make_pair("return", rawConf({UNIQUE_KEYTYPE})),
        std::make_pair("error_page", rawConf({NONE_UNIQUE_KEYTYPE})),
        std::make_pair("index", rawConf({UNIQUE_KEYTYPE})),
    };
    _data.push_back(Server());
}
*/

// CONSTUCTORS :

configurationSA::configurationSA(char *config_file)
{
    //configurationSA::conf::init_data();
    std::ifstream input(config_file);
    std::vector<std::string> fullFile;

    if (!input.is_open())
    {
        std::cout << "Error: Could not open file: " << config_file << std::endl;
        return ;
    }
    for (std::string line; getline(input, line);)
        fullFile.push_back(line);
    //std::cout << "HELLO" << std::endl;
    printVector(fullFile);
    input.close();
}