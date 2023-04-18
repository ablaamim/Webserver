#include "ConfigurationParser.hpp"

/*
* default constructor : this function is used to initialize the configuration class.
*/

configurationSA::~configurationSA()
{
    //std::cout << "Configuration destructor" << std::endl;
}

///////////////////////////////////////// INITIALIZE DATA : //////////////////////////////////////////

/*
* configuration::initialize_data() : this function initializes a collection of configuration data that is used by configurationSA
* class to parse server configuration file, The method first checks if the data collection is empty, and if not, it returns immediately.
* Otherwise, it initializes the data collection by creating an array of key-value pairs. Each key is a string that represents a 
* configuration option, and each value is an instance of the raw_configuration class. The raw class is a container for configuration 
* values, and it contains a function pointer to a function that validates the configuration value.
* After creating the array of key-value pairs, the method inserts them into the _data collection, which is a member variable of the Configuration class. The _data collection is a map that maps a string key to a raw_configuration value.
*/

/*
* std::make_pair : is a utility function that creates a std::pair object with the specified values. In the init_data function, std::make_pair is used to create pairs of values that represent configuration data.
* std::make_pair takes two arguments, which are used to construct the pair. The first argument is the value for the first element of the pair, and the second argument is the value for the second element of the pair. 
* The function returns a std::pair object initialized with the specified values.
* Using std::make_pair instead of constructing a std::pair directly using the pair constructor can make the code more concise and 
* easier to read. Additionally, it ensures that the pair is constructed in a consistent manner, using the same syntax throughout the codebase.
*/

configurationSA::configuration::data_type configurationSA::configuration::_data = configurationSA::configuration::data_type();

// _data is a map that maps a string key to a raw_configuration value.

void configurationSA::configuration::initialize_data(void)
{
    // Check if data is already initialized, if so, return immediately.    
    if (!_data.empty())
       return ;

    // Initialize data collection.    
    std::string allowedMethods[] = {"GET", "POST", "DELETE"};
    std::string autoindex[] = {"on", "off"};
    std::string ErrorPages[] = {"403", "404", "405", "413", "500"}; 
    std::string returnCode[] = {"200", "403", "404" ,"405", "413", "500"};
    
    /*
     ERROR PAGES : 403 = Forbidden, 404 = Not Found, 405 = Method Not Allowed, 413 = Request Entity Too Large, 500 = Internal Server Error
     RETURN CODE : 200 = OK, 403 = Forbidden, 404 = Not Found, 405 = Method Not Allowed, 413 = Request Entity Too Large, 500 = Internal Server Error
    */

    // Pair of key and value, key is a string that represents a configuration option, and value is an instance of the raw_configuration class.
    std::pair<std::string, raw_configuration> data[] =
    {
        std::make_pair("server_name", raw_configuration(SERVER_KEYTYPE, NULL, UNLIMITED_PARAMS)), // SERVER NAME DOES NOT TAKE A PARAMETER
        std::make_pair("listen", raw_configuration(SERVER_KEYTYPE, &_listenFormat, 2)), // LISTEN PORT
        std::make_pair("return", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, 1, returnCode, SIZEOF(returnCode))), // RETURN CODE
        std::make_pair("error_page", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, 1, ErrorPages, SIZEOF(ErrorPages))), // ERROR PAGE
        std::make_pair("cgi", raw_configuration(NONE_UNIQUE_KEYTYPE, &_checkCgi, 1)), // CGI = OMMON GATEWAY INTERFACE
        std::make_pair("auto_index", raw_configuration(UNIQUE_KEYTYPE, NULL, 1, autoindex, SIZEOF(autoindex))), // AUTOINDEX
        std::make_pair("upload", raw_configuration(UNIQUE_KEYTYPE, NULL, 1)), // UPLOAD
        std::make_pair("index", raw_configuration(UNIQUE_KEYTYPE, NULL, UNLIMITED_PARAMS)), // INDEX
        std::make_pair("root",   raw_configuration(UNIQUE_KEYTYPE, &_checkroot, 1)), // ROOT
        std::make_pair("allow_methods", raw_configuration(UNIQUE_KEYTYPE, NULL, 3, allowedMethods, SIZEOF(allowedMethods))), // METHODS
        std::make_pair("body_size", raw_configuration(UNIQUE_KEYTYPE, &_checkBodySize, 1)), // CLIENT BODY SIZE

    };
    // Insert data into the data collection.
    _data.insert(data, data + SIZEOF(data));
}

                                // INIT DEFAULT VALUES :

configurationSA::location configurationSA::configuration::_defaultVals = configurationSA::location();


void configurationSA::configuration::initialize_default_values(void)
{    
    if (!_defaultVals.NoneUniqueKey.empty() && !_defaultVals.UniqueKey.empty())
        return ;
    
    std::pair<std::string, std::vector<std::string> > returTab[] =
    {
        std::make_pair("200", std::vector<std::string>(1, "OK")),
        std::make_pair("403", std::vector<std::string>(1, "Forbidden")),
        std::make_pair("404", std::vector<std::string>(1, "Not Found")),
        std::make_pair("405", std::vector<std::string>(1, "Method Not Allowed")),
        std::make_pair("413", std::vector<std::string>(1, "Request Entity Too Large")),
        std::make_pair("500", std::vector<std::string>(1, "Internal Server Error")),
    };

    std::pair<std::string, std::map<std::string, std::vector<std::string> > > noneUniqueKey[] =
    {
        std::make_pair("return", std::map<std::string, std::vector<std::string> >(returTab, returTab + SIZEOF(returTab))),
    };

    _defaultVals.NoneUniqueKey.insert(noneUniqueKey, noneUniqueKey + SIZEOF(noneUniqueKey));

    // UNIQUE KEY DEFAULT VALUES.

    std::string allow_methods[] = {"GET", "POST", "DELETE"};

    std::pair <std::string, std::vector<std::string> > uniqueKey[] =
    {
        std::make_pair("auto_index", std::vector<std::string>(1, "of")),
        std::make_pair("body_size", std::vector<std::string>(1, "2000000")),
        std::make_pair("allow_methods", std::vector<std::string>(allow_methods, allow_methods + SIZEOF(allow_methods))),
    };
    _defaultVals.UniqueKey.insert(uniqueKey, uniqueKey + SIZEOF(uniqueKey));
}

/////////////////////////////////////// END OF INITIALIZATION : //////////////////////////////////////

const std::string configurationSA::configuration::is_white_space = "\t ";
const std::string configurationSA::configuration::is_comment = "#";
const std::string configurationSA::configuration::is_line_break = ";";
const std::string configurationSA::configuration::is_scope = "{}";

///////////////////////////////////////////// CHECKERS : /////////////////////////////////////////////

void configurationSA::_checkPort(std::string str, size_t &start_last_line, std::string &line)
{
    int port;

    if (!isDigit(str))
        throw configurationSA::ParsingErr("Error : Listen port needs to be a digit.");
    
    port = atoi(str.c_str());
    
    if (port < 0 || port > PORT_MAX)
       throw configurationSA::ParsingErr("Listen, port needs to be between 0 and 65535.");
}

void configurationSA::_checkIp(std::vector<std::string> ip, size_t &start_last_line, std::string &line)
{
    if (ip.size() != 4)
    {
        throw configurationSA::ParsingErr("Listen, up needs to be 4 octets separated by a dot.");
    }
    for (std::vector<std::string>::iterator it = ip.begin(); it < ip.end(); it++)
    {
        if (!isDigit(*it))
            throw configurationSA::ParsingErr("Listen, ip split by dot needs to be a digit.");
        if (atoi(it->c_str()) < 0 || atoi(it->c_str()) > 255)
            throw configurationSA::ParsingErr("Listen, ip split by dot needs to be between 0 and 255.");
    }
}
//std::cout << "End of IP address check !" << std::endl;


void configurationSA::_listenFormat(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    if (isDigit(key_values.second[0]))
    {       
        _checkPort(key_values.second[0], start_last_line, line);
        
        if (key_values.second.size() == 2)
        {
            _checkIp(split(key_values.second[1], "."), start_last_line, line);
            std::swap(key_values.second[0], key_values.second[1]);
        }
        else
            key_values.second.insert(key_values.second.begin(), DEFAULT_LISTEN_INTERF);        
    }
    else
    {
        _checkIp(split(key_values.second[0], "."), start_last_line, line);
        
        if (key_values.second.size() == 2)
            _checkPort(key_values.second[1], start_last_line, line);
        
        else
            key_values.second.push_back(DEFAULT_LISTEN_PORT);
    }
}

void configurationSA::_checkCgi(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    if (key_values.first.rfind('.') != 0)
       throw configurationSA::ParsingErr("Cgi, key should start with a dot.");
    
    if (key_values.first.size() < 2)
     throw configurationSA::ParsingErr("Cgi, key should be at least 2 characters long.");
    
    if (key_values.second[0].size() < 2)
        throw configurationSA::ParsingErr("Cgi, value should be at least 2 characters long.");
    
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
    {
        start_last_line += key_values.second[0].size() - 1;
        throw configurationSA::ParsingErr("Cgi, value should not end with a slash.");
    }
    if (key_values.second[0].find("../") != std::string::npos)
        throw configurationSA::ParsingErr("Cgi, value should not contain ../");
}

void configurationSA::_checkroot(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    if (key_values.second[0].find("../") != std::string::npos)
    {
        int tmp = key_values.second[0].find("../");
        
        while (tmp != std::string::npos)
        {
            tmp = key_values.second[0].find("../", tmp);
        }
        throw configurationSA::ParsingErr("Root, path cannot contain '../'");
    }
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
        key_values.second[0].erase(key_values.second[0].end() - 1);
}

void configurationSA::_checkBodySize(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    if (!isDigit(key_values.second[0]))
    {
       throw configurationSA::ParsingErr("Body size, value should be a digit.");
    }
    
    if (atoi(key_values.second[0].c_str()) <= 0)
    {
        throw configurationSA::ParsingErr("Body size overflow, max value : " + to_string(INT_MAX));
    }}

/////////////////////////////////////////// END CHECKERS ///////////////////////////////////////////


/////////////////////////////////////////// PARSE METHODS ///////////////////////////////////////////

bool configurationSA::_isServerContext(key_value_type key_value, line_range_type &line_range, file_range_type &file_range)
{
    if (key_value.first != "server")
        return (false);
    
    goToNExtWordInFile(line_range, file_range);

    if (!key_value.second.empty())
        throw ParsingErr("Error : Does' take parameters");
    
    if (line_range.first != line_range.second && *line_range.first != '{')
        throw ParsingErr("Error : Server context should be followed by a '{'");
    
    return (true);
}

bool configurationSA::_isLocationContext(key_value_type key_value, line_range_type &line_range, file_range_type &file_range, size_t start_last_line)
{    
    line_range_type line_range_copy = line_range;
    file_range_type file_range_copy = file_range;
    
    if (key_value.first != "location")
        return (false);
    
    goToNExtWordInFile(line_range_copy, file_range_copy);

    try
    {
        if (key_value.second.size() != 1)
            throw ParsingErr("Error : Location context should have one parameter");
        
        if (!key_value.second[0].empty() && key_value.second[0][0] != '/')
            throw ParsingErr("Error : Location context should start with a '/'");
        
        if (*line_range_copy.first != '{')
            throw ParsingErr("Error : Location context should be followed by a '{'");
    }
    catch (ParsingErr &e)
    {
        std::cerr << "IS_LOCATION_CONTEXT : Error : in location context : ";
        throw ParsingErr(e.what());
    }
    return (true);
}

configurationSA::key_value_type configurationSA::_getKeyValue(line_range_type &line_range)
{    
    std::string key = _getWord_skip_space(line_range);
    
    std::vector<std::string> values;
    
    std::string word = _getWord_skip_space(line_range);
    
    while (!word.empty())
    {
        values.push_back(word);
        word = _getWord_skip_space(line_range);
    }
    return (std::make_pair(key, values));
}

bool    configurationSA::checkDuplicatedParametters(std::vector<std::string> parameters, size_t &start_last_line, std::string &line)
{
    std::set<std::string> s;

    for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
    {
        if (s.count(*it) == 0)
            s.insert(*it);
    }
    return (s.size() != parameters.size());
}

bool configurationSA::CheckValidParametters(std::vector<std::string> parameters, std::set<std::string> validParamters, size_t &start_last_line, std::string &line)
{
    bool errStatus = false;

    for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
    {
        if (!validParamters.count(*it))
            errStatus = true;
    }
    return (errStatus);
}

void configurationSA::checkKeyValues(key_value_type &keyVals, const configuration::raw_configuration &keyConfig, size_t start_last_line,std::string &line)
{
    std::set<std::string> sParameters(keyVals.second.begin(), keyVals.second.end());
    
    if (keyVals.second.empty() /*&& keyConfig.keyType == conf::UNIQUE_KEYTYPE*/)
        throw ParsingErr("Not enough parameters for key " + keyVals.first);
    
    if (!keyConfig.validParametters.empty())
    {
        if (keyConfig.keyType == configuration::UNIQUE_KEYTYPE && CheckValidParametters(keyVals.second, keyConfig.validParametters, start_last_line, line))
            throw ParsingErr("Unknown Parameter");
        else if (keyConfig.keyType == configuration::NONE_UNIQUE_KEYTYPE && !keyConfig.validParametters.count(keyVals.first))
            throw ParsingErr("Unknown Parameter");
    }

    if (checkDuplicatedParametters(keyVals.second, start_last_line, line))
        throw ParsingErr("Duplicated parameters");

    if (keyConfig.max_Parameters && sParameters.size() > keyConfig.max_Parameters)
        throw ParsingErr("Too many parameters");
    
    if (keyConfig.func)
        keyConfig.func(keyVals, start_last_line, line);
}

void    configurationSA::insertKeyValLocation(location &Location, key_value_type &key_value, size_t &start_last_line, std::string &line)
{
    std::string                              keyValueFirstCopy = key_value.first;
    configuration::raw_configuration         keyConfig = configuration::_data[key_value.first];
    location::UniqueKey_t                    &insertPoint = (keyConfig.keyType == configuration::UNIQUE_KEYTYPE) ? Location.UniqueKey : Location.NoneUniqueKey[key_value.first];

    if (keyConfig.keyType == configuration::NONE_UNIQUE_KEYTYPE && !key_value.second.empty())
    {
        key_value.first = key_value.second[0];
        key_value.second.erase(key_value.second.begin());
    }

    if (insertPoint.count(key_value.first))
        throw ParsingErr("Already exists");
   
    try
    {
        checkKeyValues(key_value, keyConfig ,start_last_line, line);
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(keyValueFirstCopy + " : " + e.what());
    }
    insertPoint[key_value.first] = key_value.second;
}

configurationSA::location configurationSA::NewLocationCreation(line_range_type &line_range, file_range_type &file_range)
{
    location result;

    goToNExtWordInFile(line_range, file_range);
    
    size_t start_last_line = (size_t) (line_range.first - file_range.first->begin());
    key_value_type key_value = _getKeyValue(line_range);
    
    while (file_range.first != file_range.second && !key_value.first.empty())
    {
        if (configuration::getKeyType(key_value.first) == configuration::UNIQUE_KEYTYPE || configuration::getKeyType(key_value.first) == configuration::NONE_UNIQUE_KEYTYPE)

            insertKeyValLocation(result, key_value, start_last_line, *file_range.first);

        else if (configuration::getKeyType(key_value.first) == configuration::SERVER_KEYTYPE)
            throw ParsingErr("Error : Server context should not be in a location context");
        
        else
            throw ParsingErr("Error : Unknown key " + key_value.first);

        goToNExtWordInFile(line_range, file_range);
        start_last_line = (size_t) (line_range.first - file_range.first->begin());
        key_value = _getKeyValue(line_range);
    }
    if (*line_range.first == '{')
        throw ParsingErr("Error : Location context should not be followed by a '{'");
    if (file_range.first == file_range.second)
        throw ParsingErr("Error : Location context should be closed by a '}'");

    line_range.first++;
    
    goToNExtWordInFile(line_range, file_range);
    
    return (result);
}

void  configurationSA::insertKeyValServer(Server &result, key_value_type &key_value, size_t &start_last_line, std::string &line)
{
    const configuration::raw_configuration keyConfig = configuration::_data.find(key_value.first)->second;
    
    checkKeyValues(key_value, keyConfig, start_last_line, line);
    
    if (key_value.first == "listen" && !result.Listen[key_value.second[0]].insert(key_value.second[1]).second)
        throw ParsingErr("Error : Key listen already exists" + key_value.second[0] + " " + key_value.second[1]);
    
    else if (key_value.first == "server_name")
    {
        size_t old_size = result.ServerName.size();
        
        result.ServerName.insert(key_value.second.begin(), key_value.second.end());
        
        if (result.ServerName.size() != old_size + key_value.second.size())
            throw ParsingErr("Error : Duplicated server_name");
    }
}

configurationSA::Server  configurationSA::NewServerCreation(line_range_type &line_range, file_range_type &file_range)
{
    location server_location_config;
    Server   result;
    
    goToNExtWordInFile(line_range, file_range);
    
    size_t start_last_line = (size_t) (line_range.first - file_range.first->begin());
    
    key_value_type key_value = _getKeyValue(line_range);
    
    while (file_range.first != file_range.second && !key_value.first.empty())
    {

        if (_isLocationContext(key_value, line_range, file_range, start_last_line))
        {
           if (result.Location.count(key_value.second[0]))
                throw ParsingErr("Error : Location context already exists");

           
           goToNExtWordInFile(line_range, file_range);

           line_range.first++;
           
           if (key_value.second[0].size() > 1 && key_value.second[0][key_value.second[0].size() - 1] == '/')
               key_value.second[0].erase(key_value.second[0].end() - 1);
            try
            {
                result.Location.insert(std::make_pair(key_value.second[0], NewLocationCreation(line_range, file_range)));
            }
            catch(ParsingErr &e)
            {
                throw ParsingErr("Location context : " + key_value.second[0] + " : " + e.what());
            }
        }
        else
        {
            if (configuration::getKeyType(key_value.first) == configuration::UNIQUE_KEYTYPE || configuration::getKeyType(key_value.first) == configuration::NONE_UNIQUE_KEYTYPE)
                insertKeyValLocation(server_location_config, key_value, start_last_line, *file_range.first);
            else if (configuration::getKeyType(key_value.first) == configuration::SERVER_KEYTYPE)
                insertKeyValServer(result, key_value, start_last_line, *file_range.first);
            else
                throw ParsingErr("Error : Unknown key '" + key_value.first + "'");

        }
        goToNExtWordInFile(line_range, file_range);
        start_last_line = (int) (line_range.first - file_range.first->begin());
        key_value = _getKeyValue(line_range);
    }

    if (*line_range.first == '{')
        throw ParsingErr("Error : Server context should not be followed by a '{'");
    
    if (file_range.first == file_range.second)
        throw ParsingErr("Error : Server context should be closed by a '}'");

    line_range.first++;
    result.Location["/"].insert(server_location_config);
    result.Location["/"].insert(configuration::_defaultVals);
    return (result);
}

void configurationSA::skipCharSet(line_range_type &line_range, const std::string &charSet)
{
    while (line_range.first != line_range.second && charSet.find(*line_range.first) != std::string::npos)
        line_range.first++;
    
    if (line_range.first != line_range.second && configuration::is_comment.find(*line_range.first) != std::string::npos)
        line_range.first = line_range.second;
}


void configurationSA::goToNExtWordInFile(line_range_type &line_range, file_range_type &file_range)
{    
    if (file_range.first == file_range.second)
        return ;
    
    skipCharSet(line_range, configuration::is_white_space + configuration::is_line_break);
    
    while (file_range.first != file_range.second && line_range.first == line_range.second)
    {
        file_range.first++;
        if (file_range.first == file_range.second)
            break ;
        line_range.first = file_range.first->begin();
        line_range.second = file_range.first->end();
        skipCharSet(line_range, configuration::is_white_space + configuration::is_line_break);
    }
}

std::string     configurationSA::getWord(line_range_type &line_range)
{
    std::string result;
    
    while (line_range.first != line_range.second && (configuration::is_white_space + configuration::is_line_break \
    + configuration::is_comment + configuration::is_scope).find(*line_range.first) == std::string::npos)
    {
        if (*line_range.first == '\\')
            line_range.first++;
        if (line_range.first == line_range.second)
           throw configurationSA::ParsingErr("Unexpected end of line.");         
        result.push_back(*line_range.first++);
    }
    return (result);
}


std::string     configurationSA::_getWord_skip_space(line_range_type &line_range)
{
    skipCharSet(line_range, configuration::is_white_space);
    
    std::string word = getWord(line_range);
    
    skipCharSet(line_range, configuration::is_white_space);
    return (word);
}

/////////////////////////////////////////// END PARSE METHODS ///////////////////////////////////////////


void configurationSA::configuration::printDefaultVal(location _defaultVals)
{
    std::cout << "-----------------" << std::endl;
    std::cout << _defaultVals.UniqueKey["allowed_methods"][0] << std::endl;
    std::cout << _defaultVals.UniqueKey["allowed_methods"][1] << std::endl;
    std::cout << _defaultVals.UniqueKey["allowed_methods"][2] << std::endl;
    std::cout << "-----------------" << std::endl;
    std::cout << _defaultVals.NoneUniqueKey["return"]["200"][0] << std::endl;
    std::cout << _defaultVals.NoneUniqueKey["return"]["403"][0] << std::endl;
    std::cout << _defaultVals.NoneUniqueKey["return"]["404"][0] << std::endl;
    std::cout << _defaultVals.NoneUniqueKey["return"]["405"][0] << std::endl;
    std::cout << _defaultVals.NoneUniqueKey["return"]["413"][0] << std::endl;
    std::cout << _defaultVals.NoneUniqueKey["return"]["500"][0] << std::endl;
    std::cout << "-----------------" << std::endl;
    std::cout << _defaultVals.UniqueKey["body_size"][0] << std::endl;
    std::cout << "-----------------" << std::endl;
}


///////////////////////// ACCESSORS : /////////////////////////

configurationSA::data_type configurationSA::getData(void)
{
    return (_data);
}

///////////////////////  CONSTUCTORS : ///////////////////////

configurationSA::configurationSA(char *config_file)
{
    // init data.
    configuration::initialize_data();
    
    // init default values.
    configuration::initialize_default_values();

    std::ifstream            input(config_file);
    std::vector<std::string> fullFile;

    // open file and check if it is open.

    if (!input.is_open())
    {
       throw ParsingErr("Error: File is not open : " + std::string(config_file));
    }
    // Read file line by line.

    for (std::string line; !input.eof();)
    {
        std::getline(input, line);
        fullFile.push_back(line);
    }
    // Check if file is empty.

    if (fullFile.empty())
    {
        std::cerr << "Error: File is empty : " << config_file << std::endl;
        return ;
    }
    
    line_range_type line_range(fullFile.begin()->begin(), fullFile.begin()->end());
    file_range_type file_range(fullFile.begin(), fullFile.end());

    try
    {
        while (file_range.first != file_range.second)
        {
            goToNExtWordInFile(line_range, file_range);
            if(_isServerContext(_getKeyValue(line_range), line_range, file_range))
            {
                line_range.first++;
                goToNExtWordInFile(line_range, file_range);
                try
                {

                    _data.push_back(NewServerCreation(line_range, file_range));
                }
                catch (ParsingErr &e)
                {
                    throw ParsingErr("Server " + std::to_string(_data.size()) + " : " + e.what()); 
                }
            }
            else if (file_range.first != file_range.second)
                throw ParsingErr("Wrong server context");
            goToNExtWordInFile(line_range, file_range);
        }
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(std::string(e.what()) + " :\n" + "line " + std::to_string(fullFile.size() - (file_range.second - file_range.first) + 1) + " : " + \
        ((file_range.first == file_range.second) ? *(file_range.first - 1) : *file_range.first));
    }
    //printData(_data);
    //std::cout << "End of parsing config file: " << config_file << "..." << std::endl;
    input.close();
}