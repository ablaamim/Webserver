#include "ConfigurationParser.hpp"

/*
configurationSA::configurationSA()
{
    //std::cout << "Configuration constructor" << std::endl;
}
*/

configurationSA::~configurationSA()
{
    //std::cout << "Configuration destructor" << std::endl;
}


///////////////////////////////////////// CHECKERS : //////////////////////////////////////////

void configurationSA::_checkIp(std::vector<std::string> ip, int &start_last_line, std::string &line)
{
    //std::cout << "Checking IP address ..." << std::endl;
    //std::cout << "IP: " << ip[0] << std::endl;
    if (ip.size() != 4)
    {
        for (std::vector<std::string>::iterator it = ip.begin(); it != ip.end(); it++)
        {
            if (it != ip.begin())
            {
                throw configurationSA::ParsingErr("Listen, up needs to be 4 octets separated by a dot.");
            }
        }
        for (std::vector<std::string>::iterator it = ip.begin(); it != ip.end(); it++)
        {
            if (!isDigit(*it))
            {
                throw configurationSA::ParsingErr("Listen, ip split by dot needs to be a digit.");
            }
            if (atoi(it->c_str()) < 0 || atoi(it->c_str()) > 255)
            {
                throw configurationSA::ParsingErr("Listen, ip split by dot needs to be between 0 and 255.");
            }
        }
    }
    //std::cout << "End of IP address check !" << std::endl;
}

void configurationSA::_checkPort(std::string str, int &start_last_line, std::string &line)
{
    int port;

    //std::cout << "Checking port ..." << std::endl;
    //std::cout << "Port: " << str << std::endl;
    if (!isDigit(str))
    {
        throw configurationSA::ParsingErr("Listen, port needs to be a digit.");
    }
    port = atoi(str.c_str());
    if (port < 0 || port > 65535)
    {
       throw configurationSA::ParsingErr("Listen, port needs to be between 0 and 65535.");
    }
    //std::cout << "End of port check !" << std::endl;
}

void configurationSA::_listenFormat(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //  The key_values_type is an std::pair<std::string, std::vector<std::string> >

    //std::cout << "STARTING LISTEN FORMAT..." << std::endl;

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
    //std::cout << "END OF LISTEN FORMAT..." << std::endl;
}

void configurationSA::_checkroot(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //std::cout << "Root: " << key_values.second[0] << std::endl;

    // check if the root path is valid (no //, no / at the end)
    if (key_values.second[0].find("../") != std::string::npos)
    {
        int tmp = key_values.second[0].find("../");
        while (tmp != std::string::npos)
        {
            tmp = key_values.second[0].find("../", tmp);
        }
        throw configurationSA::ParsingErr("Root, path cannot contain ../");
    }
    // Check if the root path is absolute or not (must start with '/')
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
    {
        key_values.second[0].erase(key_values.second[0].end() - 1);
    }
}

void configurationSA::_checkCgi(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //std::cout << "Cgi: " << key_values.second[0] << std::endl;

    // check if the cgi path is valid (no //, no / at the end)
    if (key_values.first.rfind('.') != 0)
    {
       throw configurationSA::ParsingErr("Cgi, key should start with a dot.");
    }
    if (key_values.first.size() < 2)
    {
        throw configurationSA::ParsingErr("Cgi, key should be at least 2 characters long.");
    }
    if (key_values.second[0].size() < 2)
    {
        throw configurationSA::ParsingErr("Cgi, value should be at least 2 characters long.");
    }
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
    {
        start_last_line += key_values.second[0].size() - 1;
        throw configurationSA::ParsingErr("Cgi, value should not end with a slash.");
    }
    if (key_values.second[0].find("../") != std::string::npos)
    {
        throw configurationSA::ParsingErr("Cgi, value should not contain ../");
    }
}

void configurationSA::_checkBodySize(key_value_type &key_values, int &start_last_line, std::string &line)
{
    //std::cout << "Body size checker..." << std::endl;

    if (!isDigit(key_values.second[0]))
    {
       throw configurationSA::ParsingErr("Body size, value should be a digit.");
    }
    
    if (atoi(key_values.second[0].c_str()) <= 0)
    {
        throw configurationSA::ParsingErr("Body size, value should be greater than 0.");
    }
    //std::cout << "Body size checker... OK" << std::endl;
}

/////////////////////////////////////////// END CHECKERS ///////////////////////////////////////////

/////////////////////////////////////////// PARSE METHODS ///////////////////////////////////////////

void configurationSA::skipCharSet(line_range_type &line_range, const std::string &charSet)
{
    //std::cout << "Skipping char set..." << std::endl;
    //std::cout << "Char set: |" << charSet << "|" << std::endl;
   
    while (line_range.first != line_range.second && charSet.find(*line_range.first) != std::string::npos)
        line_range.first++;
    
    if (line_range.first != line_range.second && conf::_CommentSet.find(*line_range.first) != std::string::npos)
        line_range.first = line_range.second;

    //std::cout << "End of skipping char set: " << *line_range.first << std::endl;
}


void configurationSA::goToNExtWordInFile(line_range_type &line_range, file_range_type &file_range)
{
    //std::cout << "Going to next word in file..." << std::endl;
    
    if (file_range.first == file_range.second)
        return ;
    
    skipCharSet(line_range, conf::_whiteSpacesSet + conf::_LineBeakSet);
    
    while (file_range.first != file_range.second && line_range.first == line_range.second)
    {
        file_range.first++;
        if (file_range.first == file_range.second)
            break ;
        line_range.first = file_range.first->begin();
        line_range.second = file_range.first->end();
        skipCharSet(line_range, conf::_whiteSpacesSet + conf::_LineBeakSet);
    }
    //std::cout << "End of Next word: " << *line_range.first << std::endl;
}

std::string     configurationSA::getWord(line_range_type &line_range)
{
    std::string result;
    
    //std::cout << "Getting word..." << std::endl;
    
    //std::cout << "SEGV: " << *line_range.first << std::endl;
    //exit(1);
    while (line_range.first != line_range.second && (conf::_whiteSpacesSet + conf::_LineBeakSet + conf::_CommentSet + conf::_ScopeSet).find(*line_range.first) == std::string::npos)
    {
        if (*line_range.first == '\\')
            line_range.first++;
        if (line_range.first == line_range.second)
        {
           throw configurationSA::ParsingErr("Unexpected end of line.");         
        }
        result.push_back(*line_range.first++);
    }
    //std::cout << "End of get word: " << result << std::endl;
    return result;
}


std::string     configurationSA::_getWord_skip_space(line_range_type &line_range)
{
    //std::cout << "Getting word skip space..." << std::endl;

    skipCharSet(line_range, conf::_whiteSpacesSet);
    //std::cout << "End of skip space :  SEGV" << *line_range.first << std::endl;
    //exit(1);
    
    std::string word = getWord(line_range);
    
    //std::cout << "End of get word : SEGV" << *line_range.first << std::endl;
    //exit(1);
    
    skipCharSet(line_range, conf::_whiteSpacesSet);
    
    //exit(1);
    //std::cout << word << std::endl;
    return word;
}

configurationSA::key_value_type configurationSA::_getKeyValue(line_range_type &line_range)
{
    //std::cout << "Getting key value..." << std::endl;
    
    std::string key = _getWord_skip_space(line_range);
    //std::cout << "Key: " << key << std::endl;
    
    std::vector<std::string> values;
    //std::cout << "Getting values..." << std::endl;
    
    std::string word = _getWord_skip_space(line_range);
    //std::cout << "Word AFTER GETWORDSKIPSPACE : " << word << std::endl;
    
    while (!word.empty())
    {
        values.push_back(word);
        word = _getWord_skip_space(line_range);
        //std::cout << "Word: " << word << std::endl;
    }
    //std::cout << "End of get key value..." << std::endl;
    return (std::make_pair(key, values));
}

bool configurationSA::_isServerContext(key_value_type key_value, line_range_type &line_range, file_range_type &file_range)
{
    //std::cout << "Checking server context..." << std::endl << std::endl;
    
    //std::cout << "Key  : " << key_value.first << std::endl;
    
    //std::cout << "Value: " << key_value.second[0] << std::endl;
    /*
    if (key_value.first.empty())
    {
        std::cout << "Server context not found" << std::endl;
        return (false);
    }
    */

    // CHECK : server context

    if (key_value.first != "server")
    {
        //std::cout << "Server context not found" << std::endl;
        //exit(1);
        return (false);
    }
    /*
    std::cout << "Values : " << std::endl;

    std::cout << *line_range.first << std::endl;
    std::cout << *line_range.second << std::endl;
    
    std::cout << *file_range.first << std::endl;
    std::cout << *file_range.second << std::endl;
    */
    
    goToNExtWordInFile(line_range, file_range);

    if (!key_value.second.empty())
    {
        throw ParsingErr("Error : Does' take parameters");
        //std::cerr << "Error : Server context should not have any value" << std::endl;
        //exit(EXIT_FAILURE);
    }
    if (line_range.first != line_range.second && *line_range.first != '{')
    {
        throw ParsingErr("Error : Server context should be followed by a '{'");
        //std::cerr << "Error : Server context should be followed by a '{'" << std::endl;
        //exit(EXIT_FAILURE);
    }
    return (true);
}

bool configurationSA::_isLocationContext(key_value_type key_value, line_range_type &line_range, file_range_type &file_range, int start_last_line)
{
    std::cout << "Location context checker..." << std::endl;
    
    line_range_type line_range_copy = line_range;
    file_range_type file_range_copy = file_range;
    

    std::cout << "Key  : " << key_value.first << std::endl;

    if (key_value.first != "location")
    {
        //std::cout << "Location context not found" << std::endl;
        return (false);
    }
    
    goToNExtWordInFile(line_range, file_range);
    
    try
    {
        if (key_value.second.size() != 1)
        {
            throw ParsingErr("Error : Location context should have one parameter");
            //std::cerr << "Error : Location context should have one parameter" << std::endl;
            //exit(EXIT_FAILURE);
        }
        if (!key_value.second[0].empty() && key_value.second[0][0] != '/')
        {
            throw ParsingErr("Error : Location context should start with a '/'");
            //std::cerr << "Error : Location context should start with a '/'" << std::endl;
            //exit(EXIT_FAILURE);
        }
        if (*line_range.first != '{')
        {
            throw ParsingErr("Error : Location context should be followed by a '{'");
            //std::cerr << "Error : Location context should be followed by a '{'" << std::endl;
            //exit(EXIT_FAILURE);
        }
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(e.what());
    }
    return (true);
}

//configurationSA::location configurationSA::conf::_defaultVals = configurationSA::location();

void    configurationSA::insertKeyValLocation(location &Location, key_value_type key_value, int start_last_line, std::string &line)
{
    std::string   keyValueFirstCopy = key_value.first;
    conf::rawConf keyConfig = conf::_data[key_value.first];
    location::UniqueKey_t &insertPoint = (keyConfig.keyType == conf::UNIQUE_KEYTYPE) ? Location.UniqueKey : Location.NoneUniqueKey[key_value.first];

    // Set subkey if needed
    if (keyConfig.keyType == conf::UNIQUE_KEYTYPE && !key_value.second.empty())
    {
        key_value.first = key_value.second[0];
        key_value.second.erase(key_value.second.begin());
    }

    if (insertPoint.count(key_value.first))
    {
        throw ParsingErr("Error : Key " + keyValueFirstCopy + " already exists");
        //std::cerr << "Error : Key " << keyValueFirstCopy << " already exists" << std::endl;
        //exit(EXIT_FAILURE);
    }
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

bool configurationSA::CheckValidParametters(std::vector<std::string> parameters, std::set<std::string> validParamters, int start_last_line, std::string &line)
{
    bool errStatus = false;

    for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
    {
        if (!validParamters.count(*it))
        {
            //std::cerr << "Error : Invalid parameter " << *it << " for key " << line << std::endl;
            errStatus = true;
        }
    }
    return (errStatus);
}

bool    configurationSA::checkDuplicatedParametters(std::vector<std::string> parameters, int start_last_line, std::string &line)
{
    std::set<std::string> s;

    for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
    {
        if (s.count(*it) == 0)
            s.insert(*it);
        else
        {
            //std::cerr << "Error : Duplicated parameter " << *it << " for key " << line << std::endl;
            //return (true);
        }
    }
    return (s.size() != parameters.size());
}

void configurationSA::checkKeyValues(key_value_type &keyVals, const conf::rawConf &keyConfig, int start_last_line,std::string &line)
{
    std::set<std::string> sParameters(keyVals.second.begin(), keyVals.second.end());
    if (keyVals.second.empty())
    {
        throw ParsingErr("Error : Not enough parameters for key " + keyVals.first);
    }
    if (!keyConfig.validParametters.empty())
    {
        if (keyConfig.keyType == conf::UNIQUE_KEYTYPE && CheckValidParametters(keyVals.second, keyConfig.validParametters, start_last_line, line))
        {
            throw ParsingErr("Error : Invalid parameters for key " + keyVals.first);
        }
        else if (keyConfig.keyType == conf::NONE_UNIQUE_KEYTYPE && !keyConfig.validParametters.count(keyVals.first))
        {
            throw ParsingErr("Error : Invalid parameters for key " + keyVals.first);
            //std::cerr << "Error : Invalid parameters for key " << keyVals.first << std::endl;
            //exit(EXIT_FAILURE);
        }
    }
    if (checkDuplicatedParametters(keyVals.second, start_last_line, line))
    {
        throw ParsingErr("Error : Duplicated parameters for key " + keyVals.first);
        //std::cerr << "Error : Duplicated parameters for key " << keyVals.first << std::endl;
        //exit(EXIT_FAILURE);
    }
    if (keyConfig.max_Parameters && sParameters.size() > keyConfig.max_Parameters)
    {
        throw ParsingErr("Error : Too many parameters for key " + keyVals.first);
        //std::cerr << "Error : Too many parameters for key " << keyVals.first << std::endl;
        //exit(EXIT_FAILURE);
    }
    
    // if check is ok, call the function
    if (keyConfig.func)
    {
        keyConfig.func(keyVals, start_last_line, line);
    }
}

void  configurationSA::insertKeyValServer(Server &result, key_value_type key_value, int start_last_line, std::string &line)
{
    const conf::rawConf keyConfig = conf::_data.find(key_value.first)->second;
    checkKeyValues(key_value, keyConfig, start_last_line, line);
    
    if (key_value.first == "listen" && !result.Listen[key_value.second[0]].insert(key_value.second[1]).second)
    {
        throw ParsingErr("Error : Key listen already exists");
        //std::cerr << "Error : Key listen already exists" << std::endl;
        //exit(EXIT_FAILURE);
    }
    else if (key_value.first == "server_name")
    {
        int old_size = result.ServerName.size();
        result.ServerName.insert(key_value.second.begin(), key_value.second.end());
        if (result.ServerName.size() != old_size + key_value.second.size())
        {
            throw ParsingErr("Error : Duplicated server_name");
            //std::cerr << "Error : Duplicated server_name" << std::endl;
            //exit(EXIT_FAILURE);
        }
    }
}

configurationSA::location configurationSA::NewLocationCreation(line_range_type &line_range, file_range_type &file_range)
{
    location result;

    goToNExtWordInFile(line_range, file_range);
    
    int start_last_line = line_range.first - file_range.first->begin();
    key_value_type key_value = _getKeyValue(line_range);
    while (file_range.first != file_range.second && !key_value.first.empty())
    {
        if (conf::getKeyType(key_value.first) == conf::UNIQUE_KEYTYPE || conf::getKeyType(key_value.first) == conf::NONE_UNIQUE_KEYTYPE)
        {
            // insert key value location
            insertKeyValLocation(result, key_value, start_last_line, *file_range.first);
        }
        else if (conf::getKeyType(key_value.first) == conf::SERVER_KEYTYPE)
        {
            throw ParsingErr("Error : Server context should not be in a location context");
            //std::cerr << "Error : Server context should not be in a location context" << std::endl;
            //exit(EXIT_FAILURE);
        }
        else
        {
            throw ParsingErr("Error : Unknown key " + key_value.first);
            //std::cout << "HERE???" << std::endl;
            //std::cerr << "Error : Unknown key " << key_value.first << std::endl;
            //exit(EXIT_FAILURE);
        }
        goToNExtWordInFile(line_range, file_range);
        start_last_line = line_range.first - file_range.first->begin();
        key_value = _getKeyValue(line_range);
    }
    /*
    if (*line_range.first == '{')
    {
        throw ParsingErr("Error : Location context should not be followed by a '{'");
        //std::cerr << "Error : Location context should not be followed by a '{'" << std::endl;
        //exit(EXIT_FAILURE);
    }
    */
    if (file_range.first == file_range.second)
    {
        throw ParsingErr("Error : Location context should be closed by a '}'");
        //std::cerr << "Error : Location context should be closed by a '}'" << std::endl;
        //exit(EXIT_FAILURE);
    }

    line_range.first++;
    goToNExtWordInFile(line_range, file_range);
    
    return (result);
}


configurationSA::Server  configurationSA::NewServerCreation(line_range_type &line_range, file_range_type &file_range)
{
    location ServerLocationConf;
    Server   result;

    //std::cout << std::endl << std::endl << "New server creation..." << std::endl << std::endl << std::endl;
    
    goToNExtWordInFile(line_range, file_range);
    
    //std::cout << "SPOTING SEGFAULT" << std::endl << std::endl << std::endl;
    //exit(1);
    
    int start_last_line = line_range.first - file_range.first->begin();
    
    //std::cout << "START LAST LINE : "<< start_last_line << std::endl;
    
    //std::cout << "BEFORE KEYVAL : SPOTING SEGFAULT" << std::endl << std::endl << std::endl;
    
    //exit(1);
    
    key_value_type key_value = _getKeyValue(line_range);
    
    //std::cout << "KEYVAL : SPOTING SEGFAULT" << std::endl << std::endl << std::endl;
    
    //exit(1);
    
    while (file_range.first != file_range.second && !key_value.first.empty())
    {
        //std::cout << "Parsing location" << std::endl << std::endl << std::endl;

        if (_isLocationContext(key_value, line_range, file_range, start_last_line))
        {
           if (result.Location.count(key_value.second[0]))
            {
                throw ParsingErr("Error : Location context already exists");
               //std::cerr << "Error : Location context already exists" << std::endl;
               //exit(EXIT_FAILURE);
           }
           
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
                //std::cout << "FIXED ?" << std::endl << std::endl << std::endl; 
                throw ParsingErr("Location context : " + key_value.second[0] + " : " + e.what());
            }
        }
        else
        {
            if (conf::getKeyType(key_value.first) == conf::UNIQUE_KEYTYPE || conf::getKeyType(key_value.first) == conf::NONE_UNIQUE_KEYTYPE)
            {
                // insert key value location
                insertKeyValLocation(ServerLocationConf, key_value, start_last_line, *file_range.first);
            }
            else if (conf::getKeyType(key_value.first) == conf::SERVER_KEYTYPE)
            {
                insertKeyValServer(result, key_value, start_last_line, *file_range.first);
            }
            /*
            else
            {
                std::cout << "THIS EXCEPTION SHOULD NEVER BE RAISED" << std::endl;
                throw ParsingErr("Error : Unknown key '" + key_value.first + "'");
                //std::cout << "HERE ???" << std::endl;
                //exit(1);
                //std::cout << "|" << key_value.first << "|" << std::endl;
                //std::cout << "HERE ???" << std::endl;
                //throw ParsingErr("Error : Unknown key '" + key_value.first + "'" + key_value.first);
                
                //std::cerr << "Error : Unknown key " << key_value.first << std::endl;
                //exit(EXIT_FAILURE);
            }
            */
        }
        goToNExtWordInFile(line_range, file_range);
        start_last_line = line_range.first - file_range.first->begin();
        key_value = _getKeyValue(line_range);
    }
    /*
    if (*line_range.first == '{')
    {
        //std::cout << "HERE ???" << std::endl;
        throw ParsingErr("Error : Server context should not be followed by a '{'");
        //std::cerr << "Error : Server context should not be followed by a '{'" << std::endl;
        //exit(EXIT_FAILURE);
    }
    */
    if (file_range.first == file_range.second)
    {
        throw ParsingErr("Error : Server context should be closed by a '}'");
        //std::cerr << "Error : Server context should be closed by a '}'" << std::endl;
        //exit(EXIT_FAILURE);
    }

    line_range.first++;
    result.Location["/"].insert(ServerLocationConf);
    result.Location["/"].insert(conf::_defaultVals);
    
    //std::cout << "Server created" << std::endl;
    return (result);
}



/////////////////////////////////////////// END PARSE METHODS ///////////////////////////////////////////


configurationSA::conf::data_type configurationSA::conf::_data = configurationSA::conf::data_type ();

void configurationSA::conf::init_data(void)
{
    //std::cout << "\n\n Initializing data..." << std::endl << std::endl << std::endl;
    
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
        std::make_pair("server_name", rawConf(SERVER_KEYTYPE, 0, UNLIMITED_PARAMS)), // SERVER NAME DOES NOT TAKE A PARAMETER
       
        // MAKE PAIR FOR LISTEN KEY TYPE.
        std::make_pair("listen", rawConf(SERVER_KEYTYPE, &_listenFormat, 2)), // LISTEN PORT
       
        // MAKE PAIR FOR LOCATION KEY TYPE.
        std::make_pair("root",   rawConf(UNIQUE_KEYTYPE, &_checkroot, 1)), // ROOT
       
        // MAKE PAIR FOR RETURN KEY TYPE.
        std::make_pair("return", rawConf(NONE_UNIQUE_KEYTYPE, 0, 1, returnCode, SIZEOF(returnCode))), // RETURN CODE
       
        // MAKE PAIR FOR ERROR PAGE KEY TYPE.
        std::make_pair("error_page", rawConf(NONE_UNIQUE_KEYTYPE, 0, 1, ErrorPages, SIZEOF(ErrorPages))), // ERROR PAGE
       
        // MAKE PAIR FOR INDEX KEY TYPE.
        std::make_pair("index", rawConf(UNIQUE_KEYTYPE, 0, UNLIMITED_PARAMS)), // INDEX
       
        // MAKE PAIR FOR CGI KEY TYPE.
        std::make_pair("cgi", rawConf(NONE_UNIQUE_KEYTYPE, &_checkCgi, 1)), // CGI = OMMON GATEWAY INTERFACE

         // MAKE PAIR FOR AUTOINDEX KEY TYPE.
        std::make_pair("autoindex", rawConf(UNIQUE_KEYTYPE, 0, 1, autoindex, SIZEOF(autoindex))), // AUTOINDEX

        // MAKE PAIR FOR METHODS KEY TYPE.
        std::make_pair("allow_methods", rawConf(UNIQUE_KEYTYPE, 0, 3, allowedMethods, SIZEOF(allowedMethods))), // METHODS

        // MAKE PAIR FOR CLIENT BODY SIZE KEY TYPE.
        std::make_pair("body_size", rawConf(UNIQUE_KEYTYPE, &_checkBodySize, 0x1)), // CLIENT BODY SIZE

        // MAKE PAIR FOR UPLOAD KEY TYPE.
        std::make_pair("upload", rawConf(UNIQUE_KEYTYPE, 0x0, 0x1)), // UPLOAD
    };

    // insert data into _data map.
    _data.insert(data, data + SIZEOF(data));
    
    // check if _data is empty.
    if (_data.empty())
    {
        std::cerr << "Error : _data is empty" << std::endl;
        //exit(EXIT_FAILURE);
    }

    // loop through _data and print out key-value pairs
    //print_data();
    //printMap(_data);
    //std::cout << std::endl << std::endl << std::endl;
    //printServers(_data);
}

////////////////////////////// INIT DEFAULT VALUES ///////////////////////////////////////

// default values for the configuration file.

configurationSA::location configurationSA::conf::_defaultVals = configurationSA::location();

void configurationSA::conf::printDefaultVal(location _defaultVals)
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

void configurationSA::conf::initDefaultVals(void)
{
    //std::cout << "Initializing default values..." << std::endl;
    
    if (!_defaultVals.NoneUniqueKey.empty() || !_defaultVals.UniqueKey.empty())
        return ;

    // NONE UNIQUE KEY DEFAULT VALUES.

    std::pair<std::string, std::vector<std::string> > returTab[] =
    {
        std::make_pair("200", std::vector<std::string>(1, "ok")),
        std::make_pair("403", std::vector<std::string>(1, "forbidden")),
        std::make_pair("404", std::vector<std::string>(1, "not found")),
        std::make_pair("405", std::vector<std::string>(1, "method not allowed")),
        std::make_pair("413", std::vector<std::string>(1, "request entity too large")),
        std::make_pair("500", std::vector<std::string>(1, "internal server error")),
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
        std::make_pair("allowed_methods", std::vector<std::string>(allow_methods, allow_methods + SIZEOF(allow_methods))),
    };
    _defaultVals.UniqueKey.insert(uniqueKey, uniqueKey + SIZEOF(uniqueKey));

    // CHECKERS FOR DEFAULT VALUES.
    if (_defaultVals.UniqueKey.empty())
    {
        std::cout << "Unique key is empty" << std::endl;
    }
    
    //printDefaultVal(_defaultVals);
    //std::cout << "End of default values..." << std::endl;
}

////////////////////////////// END OF DEFAULT VALUES /////////////////////////////////////

const std::string configurationSA::conf::_whiteSpacesSet = "\t";
const std::string configurationSA::conf::_CommentSet = "#";
const std::string configurationSA::conf::_LineBeakSet = ";";
const std::string configurationSA::conf::_ScopeSet = "{}";


///////////////////////// ACCESSORS : /////////////////////////

configurationSA::data_type configurationSA::getData(void)
{
    return (_data);
}

///////////////////////  CONSTUCTORS : ///////////////////////

configurationSA::configurationSA(char *config_file)
{
    std::cout << "Parsing config file: " << config_file << "..." << std::endl;
    
    // init data.
    conf::init_data();
    
    // init default values.
    conf::initDefaultVals();

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
    //printVector(fullFile);
    
    //exit(124);
    
    line_range_type lineRange(fullFile.begin()->begin(), fullFile.begin()->end());
    file_range_type fileRange(fullFile.begin(), fullFile.end());

    std::cout << "Start parsing..." << std::endl;
    
    //std::cout << fullFile.size() << std::endl;
    //std::cout << "lineRange.first : |" << *lineRange.first << "|" << std::endl;
    //std::cout << "lineRange.second : |" << *lineRange.second << "|" << std::endl;
    //std::cout << "fileRange.first : |" << *fileRange.first << "|" << std::endl;

    //exit(255);

    try
    {
        while (fileRange.first != fileRange.second)
        {
            //std::cout << "-----------------" << std::endl;
            //std::cout << "lineRange.first: |" << *lineRange.first << "|" << std::endl;
            //std::cout << "lineRange.second: |" << *lineRange.second << "|" << std::endl;
            
            goToNExtWordInFile(lineRange, fileRange);
            
            //exit(555);
            
            //std::cout << "LINE => lineRange.first: |" << *lineRange.first << "|" << std::endl;
            std::cout << "FILE => fileRange.first: |" << *fileRange.first << "|" << std::endl;
            
            if(_isServerContext(_getKeyValue(lineRange), lineRange, fileRange))
            {
                lineRange.first++;
                //std::cout << "IS SERVER LOOP : lineRange.first: " << *lineRange.first << std::endl;
                //std::cout << "lineRange.second: " << *lineRange.second << std::endl;
                
                goToNExtWordInFile(lineRange, fileRange);
                
                //std::cout << "lineRange.first: " << *lineRange.first << std::endl;
                //std::cout << "lineRange.second: " << *lineRange.second << std::endl;
                try
                {
                    //std::cout << "-----------------" << std::endl;
                    _data.push_back(NewServerCreation(lineRange, fileRange));
                    //std::cout << "-----------------" << std::endl;
                    //printServers(_data);

                    // insert new server in the data.
                }
                catch (ParsingErr &e)
                {
                    throw ParsingErr("Server " + std::to_string(_data.size()) + " : " + e.what()); 
                }
            }
            else if (fileRange.first != fileRange.second)
            {
                //std::cout << "lineRange.first: |" << *lineRange.first << "|" << std::endl;
                //std::cout << "lineRange.second: |" << *lineRange.second << "|" << std::endl;
                //throw ParsingErr("Wrong server context");
                //std::cerr << "Error: No server context found in config file : " << config_file << std::endl;
                return ;
            }
            goToNExtWordInFile(lineRange, fileRange);
        }
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(std::string(e.what()) + " :\n" + "line " + std::to_string(fullFile.size() - (fileRange.second - fileRange.first) + 1) + " : " + \
        ((fileRange.first == fileRange.second) ? *(fileRange.first - 1) : *fileRange.first));
    }
    //printData(_data);
    //std::cout << "End of parsing config file: " << config_file << "..." << std::endl;
    input.close();
}