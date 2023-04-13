#include "ConfigurationParser.hpp"


configurationSA::configurationSA()
{
    //std::cout << "Configuration constructor" << std::endl;
}

configurationSA::~configurationSA()
{
    //std::cout << "Configuration destructor" << std::endl;
}

///////////////////////////// TOOLS : ///////////////////////////

bool isDigit(std::string &digits)
{
    for (size_t i = 0; i < digits.size(); i++)
        if (!isdigit(digits[i]))
            return (false);
    return (true);
}

///////////////////////////// CHECKERS : ///////////////////////////

void configurationSA::_checkPort(std::string str, int &start_last_line, std::string &line)
{
    int port;
    std::cout << "Port: " << str << std::endl;
    if (!isDigit(str))
    {
        std::cout << "Error: Invalid port number at line " << start_last_line << std::endl;
        std::cout << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    port = atoi(str.c_str());
    if (port < 0 || port > 65535)
    {
        std::cout << "Error: Invalid port number at line " << start_last_line << std::endl;
        std::cout << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
}

void configurationSA::_listenFormat(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //std::cout << "Listen TOKEN : " << key_values.second[0] << std::endl;
    if (isDigit(key_values.second[0]))
    {
        //std::cout << "Listen port: " << key_values.second[0] << std::endl;
        _checkPort(key_values.second[0], start_last_line, line);
        if (key_values.second.size() == 2)
        {
            std::swap(key_values.second[0], key_values.second[1]);
        }
        else
        {
            //std::cout << "Inserting default listen: " << DEFAULT_LISTEN << std::endl;
            key_values.second.insert(key_values.second.begin(), DEFAULT_LISTEN);
        }        
    }
    //conf::printKeyVal(key_values);
    /*
    else
    {
        std::cout << "Error: Invalid listen format at line " << start_last_line << std::endl;
        std::cout << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    */
}

void configurationSA::_checkroot(key_value_type &key_values, int &start_last_line, std::string &line)
{
    std::cout << "Root: " << key_values.second[0] << std::endl;
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
       std::make_pair("listen", rawConf(SERVER_KEYTYPE, &_listenFormat, 2)),
       std::make_pair("root",   rawConf(UNIQUE_KEYTYPE, &_checkroot, 1)),
    };
    _data.insert(data, data + SIZEOF(data));
    // loop through _data and print out key-value pairs
    //print_data();
    printMap(_data);
    //std::cout << std::endl << std::endl << std::endl;
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
        std::cerr << "Error: Could not open file: " << config_file << std::endl;
        return ;
    }
    
    for (std::string line; getline(input, line);)
        fullFile.push_back(line);
    
    if (fullFile.empty())
    {
        std::cerr << "Error: File is empty: " << config_file << std::endl;
        return ;
    }
    //printVector(fullFile);
    input.close();
}