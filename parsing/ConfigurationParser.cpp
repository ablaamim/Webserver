#include "ConfigurationParser.hpp"

configurationSA::configurationSA()
{
    //std::cout << "Configuration constructor" << std::endl;
}

configurationSA::~configurationSA()
{
    //std::cout << "Configuration destructor" << std::endl;
}

///////////////////////////////////////// CHECKERS : //////////////////////////////////////////

void configurationSA::_checkIp(std::vector<std::string> ip, int &start_last_line, std::string &line)
{
    std::cout << "Checking IP address ..." << std::endl;
    std::cout << "IP: " << ip[0] << std::endl;
    if (ip.size() != 4)
    {
        for (std::vector<std::string>::iterator it = ip.begin(); it != ip.end(); it++)
        {
            if (it != ip.begin())
            {
                std::cout << "Error: Invalid IP address at line " << start_last_line << std::endl;
                std::cout << "Line: " << line << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        for (std::vector<std::string>::iterator it = ip.begin(); it != ip.end(); it++)
        {
            if (!isDigit(*it))
            {
                std::cerr << "Error: Invalid IP address at line " << start_last_line << std::endl;
                std::cerr << "Line: " << line << std::endl;
                exit(EXIT_FAILURE);
            }
            int octet = atoi(it->c_str());
            if (octet < 0 || octet > 255)
            {
                std::cerr << "Error: Invalid IP address at line " << start_last_line << std::endl;
                std::cerr << "Line: " << line << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    std::cout << "End of IP address check !" << std::endl;
}

void configurationSA::_checkPort(std::string str, int &start_last_line, std::string &line)
{
    int port;

    std::cout << "Checking port ..." << std::endl;
    std::cout << "Port: " << str << std::endl;
    if (!isDigit(str))
    {
        std::cerr << "Error: Invalid port number at line " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    port = atoi(str.c_str());
    if (port < 0 || port > 65535)
    {
        std::cerr << "Error: Invalid port number at line " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "End of port check !" << std::endl;
}

void configurationSA::_listenFormat(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //  The key_values_type is an std::pair<std::string, std::vector<std::string> >

    std::cout << "STARTING LISTEN FORMAT..." << std::endl;

    if (isDigit(key_values.second[0]))
    {
        //std::cout << "Listen port: " << key_values.second[0] << std::endl;
        _checkPort(key_values.second[0], start_last_line, line);
        
        if (key_values.second.size() == 2)
        {
            _checkIp(split(key_values.second[1], "."), start_last_line, line);
            std::swap(key_values.second[0], key_values.second[1]);
        }
        else
        {
            //std::cout << "Inserting default listen: " << DEFAULT_LISTEN_PORT << std::endl;
            key_values.second.insert(key_values.second.begin(), DEFAULT_LISTEN_INTERF);
        }        
    }
    else
    {
        //std::cout << "Listen IP: " << key_values.second[0] << std::endl;
        _checkIp(split(key_values.second[0], "."), start_last_line, line);
        
        if (key_values.second.size() == 2)
        {
            _checkPort(key_values.second[1], start_last_line, line);
        }
        else
        {
            //std::cout << "Inserting default listen: " << DEFAULT_LISTEN_PORT << std::endl;
            key_values.second.push_back(DEFAULT_LISTEN_PORT);
        }
    }
    std::cout << "END OF LISTEN FORMAT..." << std::endl;
}

void configurationSA::_checkroot(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //std::cout << "Root: " << key_values.second[0] << std::endl;

    // check if the root path is valid (no //, no / at the end)
    if (key_values.second[0].find("../") != std::string::npos)
    {
        std::cerr << "Error : Invalid root path at line " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Check if the root path is absolute or not (must start with '/')
    if (key_values.second[0][0] != '/')
    {
        std::cerr << "Error : Invalid root path at line " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Check if the root path contains a double slash (//)
    if (key_values.second[0].find("//") != std::string::npos)
    {
        std::cerr << "Error : Invalid root path at line " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Check if the root path ends with a slash (/)
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
    {
        key_values.second[0].erase(key_values.second[0].size() - 1);
    }   
}

void configurationSA::_checkCgi(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //std::cout << "Cgi: " << key_values.second[0] << std::endl;

    // check if the cgi path is valid (no //, no / at the end)
    if (key_values.first.rfind('.') != 0)
    {
        std::cerr << "Error : Cgi First param should start with '.' " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    if (key_values.first.size() < 2)
    {
        std::cerr << "Error : Cgi first param should be at least 2 characters long " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    if (key_values.second[0].size() < 2)
    {
        std::cerr << "Error : Second param should be at least 2 characters long " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
    {
        start_last_line += key_values.second[0].size() - 1;
        std::cerr << "Error : Cgi path should not end with a slash " << start_last_line << std::endl;
    }
    if (key_values.second[0].find("//") != std::string::npos)
    {
        std::cerr << "Error : Cgi path should not contain a double slash " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
    if (key_values.second[0].find("../") != std::string::npos)
    {
        std::cerr << "Error : Cgi path should not contain a double slash " << start_last_line << std::endl;
        std::cerr << "Line: " << line << std::endl;
        exit(EXIT_FAILURE);
    }
}

/////////////////////////////////////////// END CHECKERS ///////////////////////////////////////////

configurationSA::conf::data_type configurationSA::conf::_data = configurationSA::conf::data_type ();

void configurationSA::conf::init_data(void)
{
    std::cout << "\n\nInitializing data..." << std::endl << std::endl << std::endl;
    
    if (!_data.empty())
       return ;

    // strings that are allowed in the configuration file for each key type.

    std::string allowedMethods[] = {"GET", "POST", "DELETE"};
    std::string autoindex[] = {"on", "off"};
    std::string ErrorPages[] = {"403", "404", "405", "413", "500"}; 
    std::string returnCode[] = {"200", "403", "404" ,"405", "413", "500"};
    
    // ERROR PAGES : 403 = Forbidden, 404 = Not Found, 405 = Method Not Allowed, 413 = Request Entity Too Large, 500 = Internal Server Error
    // RETURN CODE : 200 = OK, 403 = Forbidden, 404 = Not Found, 405 = Method Not Allowed, 413 = Request Entity Too Large, 500 = Internal Server Error
    
    std::pair<std::string, rawConf> data[] =
    {
        // MAKE PAIR FOR SERVER KEY TYPE.
        std::make_pair("server_name", rawConf(SERVER_KEYTYPE, 0x0, UNLIMITED_PARAMS)), // SERVER NAME
       
        // MAKE PAIR FOR LISTEN KEY TYPE.
        std::make_pair("listen", rawConf(SERVER_KEYTYPE, &_listenFormat, 2)), // LISTEN PORT
       
        // MAKE PAIR FOR LOCATION KEY TYPE.
        std::make_pair("root",   rawConf(UNIQUE_KEYTYPE, &_checkroot, 0x1)), // ROOT
       
        // MAKE PAIR FOR RETURN KEY TYPE.
        std::make_pair("return", rawConf(NONE_UNIQUE_KEYTYPE, 0x0, 0x1, returnCode, SIZEOF(returnCode))), // RETURN CODE
       
        // MAKE PAIR FOR ERROR PAGE KEY TYPE.
        std::make_pair("error_page", rawConf(NONE_UNIQUE_KEYTYPE, 0x0, 0x1, ErrorPages, SIZEOF(ErrorPages))), // ERROR PAGE
       
        // MAKE PAIR FOR INDEX KEY TYPE.
        std::make_pair("index", rawConf(UNIQUE_KEYTYPE, 0x0, UNLIMITED_PARAMS)), // INDEX
       
        // MAKE PAIR FOR CGI KEY TYPE.
        std::make_pair("cgi", rawConf(UNIQUE_KEYTYPE, &_checkCgi, UNLIMITED_PARAMS)), // CGI = OMMON GATEWAY INTERFACE

         // MAKE PAIR FOR AUTOINDEX KEY TYPE.
        std::make_pair("autoindex", rawConf(UNIQUE_KEYTYPE, 0x0, 0x1, autoindex, SIZEOF(autoindex))), // AUTOINDEX

        // MAKE PAIR FOR METHODS KEY TYPE.
        std::make_pair("methods", rawConf(UNIQUE_KEYTYPE, 0x0, UNLIMITED_PARAMS, allowedMethods, SIZEOF(allowedMethods))), // METHODS

        // MAKE PAIR FOR CLIENT BODY SIZE KEY TYPE.
        std::make_pair("client_body_size", rawConf(UNIQUE_KEYTYPE, 0x0, 0x1)), // CLIENT BODY SIZE

        // MAKE PAIR FOR REDIRECT KEY TYPE.
        std::make_pair("redirect", rawConf(UNIQUE_KEYTYPE, 0x0, 0x1)), // REDIRECT

        // MAKE PAIR FOR UPLOAD KEY TYPE.
        std::make_pair("upload", rawConf(UNIQUE_KEYTYPE, 0x0, 0x1)), // UPLOAD
    };

    // insert data into _data map.
    _data.insert(data, data + SIZEOF(data));
    
    // loop through _data and print out key-value pairs
    //print_data();
    printMap(_data);
    std::cout << std::endl << std::endl << std::endl;
}

///////////////////////// ACCESSORS : /////////////////////////

configurationSA::data_type configurationSA::getData(void)
{
    return (_data);
}

///////////////////////  CONSTUCTORS : ///////////////////////

configurationSA::configurationSA(char *config_file)
{
    std::cout << "Parsing config file: " << config_file << "..." << std::endl;
    
    conf::init_data();
    std::ifstream input(config_file);
    std::vector<std::string> fullFile;

    // open file and check if it is open.

    if (!input.is_open())
    {
        std::cerr << "Error: Could not open file : " << config_file << std::endl;
        return ;
    }
    // Read file line by line.

    for (std::string line; getline(input, line);)
        fullFile.push_back(line);

    // Check if file is empty.

    if (fullFile.empty())
    {
        std::cerr << "Error: File is empty : " << config_file << std::endl;
        return ;
    }
    printVector(fullFile);
    input.close();
}