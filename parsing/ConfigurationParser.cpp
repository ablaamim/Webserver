#include "ConfigurationParser.hpp"

configurationSA::configurationSA()
{
    //std::cout << "Configuration constructor" << std::endl;
}

configurationSA::~configurationSA()
{
    //std::cout << "Configuration destructor" << std::endl;
}

void configurationSA::conf::print_data()
{
  for (std::map<std::string, rawConf>::const_iterator it = _data.begin(); it != _data.end(); ++it)
  {
    std::cout << it->first << ": {" << std::endl;
    std::cout << "    keyType: " << it->second.keyType << std::endl;
    //std::cout << "    maxParams: " << it->second.max_Parameters << std::endl;
    //std::cout << "    validParams: {";
    /*
    for (size_t i = 0; i < it->second.validParametters.size(); ++i) {
      std::cout << " " << it->second.validParametters[i];
    }
    */
    std::cout << " }" << std::endl;
    std::cout << "}" << std::endl;
  }
}

///////////////////////////// TOOLS : ///////////////////////////

bool isDigit(std::string &digits)
{
    for (size_t i = 0; i < digits.size(); i++)
        if (!isdigit(digits[i]))
            return false;
    return true;
}
///////////////////////////// CHECKERS : ///////////////////////////

void configurationSA::_listenFormat(key_value_type &key_values, int &start_last_line, std::string &line)
{
    if (isDigit(key_values.second[0]))
    {
        std::cout << "Listen port: " << key_values.second[0] << std::endl;
    }
}

configurationSA::conf::data_type configurationSA::conf::_data = configurationSA::conf::data_type ();

void configurationSA::conf::init_data(void)
{
std::cout << "Initializing data..." << std::endl << std::endl << std::endl;
   if (!_data.empty())
       return ;
   std::string allowedMethods[] = {"GET", "POST", "DELETE"};
   std::string autoindex[] = {"on", "off"};
   std::string ErrorPages[] = {"404", "403", "405"}; // NEEDS TO BE CHANGED // 404 = not found, 403 = forbidden, 405 = method not allowed
   std::string returnCode[] = {"404", "403", "405"}; // NEEDS TO BE CHANGED // 404 = not found, 403 = forbidden, 405 = method not allowed
   std::pair<std::string, rawConf> data[] =
   {
       std::make_pair("server", rawConf(SERVER_KEYTYPE, 0x0, UNLIMITED_PARAMS)),
       //std::make_pair("listen", rawConf(SERVER_KEYTYPE, &_listenFormat, 2)),
   };
    _data.insert(data, data + SIZEOF(data));
    // loop through _data and print out key-value pairs
    print_data();
    std::cout << std::endl << std::endl << std::endl;
}

///////////////////////  CONSTUCTORS : ///////////////////////

configurationSA::configurationSA(char *config_file)
{
    std::cout << "Parsing config file: " << config_file << "..." << std::endl;
    conf::init_data();
    std::ifstream input(config_file);
    std::vector<std::string> fullFile;

    if (!input.is_open())
    {
        std::cout << "Error: Could not open file: " << config_file << std::endl;
        return ;
    }
    
    for (std::string line; getline(input, line);)
        fullFile.push_back(line);
    
    if (fullFile.empty())
    {
        std::cout << "Error: File is empty: " << config_file << std::endl;
        return ;
    }
    printVector(fullFile);
    input.close();
}