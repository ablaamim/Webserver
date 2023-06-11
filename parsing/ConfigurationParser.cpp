#include "ConfigurationParser.hpp"

/*
* COLORS :
*/

void configurationSA::color_words_in_range(size_t &start, const std::string &word, std::string &line, const std::string &color)
{
   std::string reset(COLOR_RESET);
   if (line.find(word, start) == std::string::npos)
       return ;

    start = line.find(word, start);
    line.insert(start, color);
    start += color.size() + word.size();
    line.insert(start, reset);
    start += reset.size();
}

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
    std::string ErrorPages[] = {}; 
    std::string returnCode[] = {};
    /*
     ERROR PAGES : 403 = Forbidden, 404 = Not Found, 405 = Method Not Allowed, 413 = Request Entity Too Large, 500 = Internal Server Error
     RETURN CODE : 200 = OK, 403 = Forbidden, 404 = Not Found, 405 = Method Not Allowed, 413 = Request Entity Too Large, 500 = Internal Server Error
    */
    // Pair of key and value, key is a string that represents a configuration option, and value is an instance of the raw_configuration class.
    std::pair<std::string, raw_configuration> data[] =
    {
        std::make_pair("server_name", raw_configuration(SERVER_KEYTYPE, NULL, UNLIMITED_PARAMS)),                               // SERVER NAME DOES NOT TAKE A PARAMETER
        
        std::make_pair("listen", raw_configuration(SERVER_KEYTYPE, &listen_format, 2)),     
        std::make_pair("cgi-bin", raw_configuration(NONE_UNIQUE_KEYTYPE, &check_cgi, 1)),                                     // LISTEN PORT
        
        std::make_pair("return", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, 1, returnCode, SIZEOF(returnCode))),              // RETURN CODE
        
        std::make_pair("error_pages", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, 1, ErrorPages, SIZEOF(ErrorPages))),         // ERROR PAGE
        
                                              // CGI = OMMON GATEWAY INTERFACE
        
        std::make_pair("auto_index", raw_configuration(UNIQUE_KEYTYPE, NULL, 1, autoindex, SIZEOF(autoindex))),                 // AUTOINDEX
        
        std::make_pair("upload", raw_configuration(UNIQUE_KEYTYPE, NULL, 1)),                                                   // UPLOAD
        
        std::make_pair("index", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, UNLIMITED_PARAMS)),                                     // INDEX
        
        std::make_pair("root",   raw_configuration(UNIQUE_KEYTYPE, &check_root, 1)),                                            // ROOT
        
        std::make_pair("allowed_methods", raw_configuration(UNIQUE_KEYTYPE, NULL, 3, allowedMethods, SIZEOF(allowedMethods))),  // METHODS
        
        std::make_pair("max_body_size", raw_configuration(UNIQUE_KEYTYPE, &check_body_size, 1)), // CLIENT BODY SIZE

    };
    
    // Insert data into the data collection.
    _data.insert(data, data + SIZEOF(data));
}

                                // INIT DEFAULT VALUES :

configurationSA::location configurationSA::configuration::_default_values = configurationSA::location();


void configurationSA::configuration::initialize_default_values(void)
{    
    if (!_default_values.NoneUniqueKey.empty() && !_default_values.UniqueKey.empty())
        return ;
    std::pair<std::string, std::vector<std::string> > return_tab[] =
    {
        //std::make_pair("200", std::vector<std::string> (1, "OK")),
        //
        //std::make_pair("403", std::vector<std::string> (1, "Forbidden")),
        //
        //std::make_pair("404", std::vector<std::string> (1, "Not Found")),
        //
        //std::make_pair("405", std::vector<std::string> (1, "Method Not Allowed")),
        //
        //std::make_pair("413", std::vector<std::string> (1, "Request Entity Too Large")),
        //
        //std::make_pair("500", std::vector<std::string> (1, "Internal Server Error")),
    };
    //std::pair<std::string, std::map<std::string, std::vector<std::string> > > noneUniqueKey[] =
    //{
        //std::make_pair("return", std::map<std::string, std::vector<std::string> >(return_tab, return_tab + SIZEOF(return_tab))),
    //};
    
    //_default_values.NoneUniqueKey.insert(noneUniqueKey, noneUniqueKey + SIZEOF(noneUniqueKey));
    
    // UNIQUE KEY DEFAULT VALUES.
    std::string allowed_methods[] = {"GET", "POST", "DELETE"};
    
    std::pair <std::string, std::vector<std::string> > uniqueKey[] =
    {
        std::make_pair("auto_index", std::vector<std::string>(1, "off")),
        
        std::make_pair("max_body_size", std::vector<std::string>(1, "100000000000000000000000000")),
        
        std::make_pair("allowed_methods", std::vector<std::string>(allowed_methods, allowed_methods + SIZEOF(allowed_methods))),
    };

    _default_values.UniqueKey.insert(uniqueKey, uniqueKey + SIZEOF(uniqueKey));
}

/////////////////////////////////////// END OF INITIALIZATION : //////////////////////////////////////

const std::string configurationSA::configuration::is_white_space = "\t ";
const std::string configurationSA::configuration::is_comment = "#";
const std::string configurationSA::configuration::is_line_break = ";";
const std::string configurationSA::configuration::is_scope = "{}";

///////////////////////////////////////////// CHECKERS : /////////////////////////////////////////////

void configurationSA::check_port(std::string str, size_t &start_last_line, std::string &line)
{
    int port;

    if (!isDigit(str))
    {
        color_words_in_range(start_last_line, str, line, COLOR_RED);
        throw configurationSA::ParsingErr(PORT_ERR_DIGIT);
    }
    port = atoi(str.c_str());
    if (port < 0 || port > PORT_MAX_VALUE)
    {
       color_words_in_range(start_last_line, str, line, COLOR_RED);
       throw configurationSA::ParsingErr(ERR_MAX_PORT);
    }
}

void configurationSA::check_ip(std::vector<std::string> ip, size_t &start_last_line, std::string &line)
{
    if (ip.size() != 4)
    {
        for (std::vector<std::string>::iterator it = ip.begin(); it != ip.end(); it++)
        {
            if (it != ip.begin())
                color_words_in_range(start_last_line, ".", line, COLOR_RED);
            color_words_in_range(start_last_line, *it, line, COLOR_RED);
        }
        throw configurationSA::ParsingErr(LISTEN_IP_ERR);
    }
    for (std::vector<std::string>::iterator it = ip.begin(); it < ip.end(); it++)
    {
        if (!isDigit(*it))
        {
            color_words_in_range(start_last_line, *it, line, COLOR_RED);
            throw configurationSA::ParsingErr(LISTEN_DOTS_ERR);
        }
        if (atoi(it->c_str()) < 0 || atoi(it->c_str()) > 255)
        {
            color_words_in_range(start_last_line, *it, line, COLOR_RED);
            throw configurationSA::ParsingErr(LISTEN_INTERVAL_ERR);
        }
    }
}

void configurationSA::listen_format(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    (void) start_last_line;
    (void) line;
    if (isDigit(key_values.second[0]))
    {       
        check_port(key_values.second[0], start_last_line, line);
        
        if (key_values.second.size() == 2)
        {
            check_ip(split(key_values.second[1], "."), start_last_line, line);
            std::swap(key_values.second[0], key_values.second[1]);
        }
        else
            key_values.second.insert(key_values.second.begin(), DEFAULT_LISTEN_INTERFACE);        
    }
    else
    {
        check_ip(split(key_values.second[0], "."), start_last_line, line);
        if (key_values.second.size() == 2)
            check_port(key_values.second[1], start_last_line, line);
        
        else
            key_values.second.push_back(DEFAULT_LISTEN_PORT);
    }
}

void configurationSA::check_cgi(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    (void) line;
    if (key_values.first.rfind('.') != 0)
    {
        color_words_in_range(start_last_line, key_values.first, line, COLOR_RED);
        throw configurationSA::ParsingErr(CGI_DOT_ERR);
    }
    if (key_values.first.size() < 2)
    {
        color_words_in_range(start_last_line, key_values.first, line, COLOR_RED);
        throw configurationSA::ParsingErr(CGI_SIZE_ERR);
    }
    if (key_values.second[0].size() < 2)
    {
        color_words_in_range(start_last_line, key_values.second[0], line, COLOR_RED);
        throw configurationSA::ParsingErr(CGI_SIZE_ERR);
    }
    
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
    {
        color_words_in_range(start_last_line, key_values.first, line, COLOR_RED);
        start_last_line += key_values.second[0].size() - 1;
        color_words_in_range(start_last_line, "/", line, COLOR_RED);
        throw configurationSA::ParsingErr(CGI_SLASH_ERR);
    }
    if (key_values.second[0].find("../") != std::string::npos)
        throw configurationSA::ParsingErr(CGI_DOTDOT_ERR);
}

void configurationSA::check_root(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    (void) line;
    (void) start_last_line;
    if (key_values.second[0].find("../") != std::string::npos)
    {
        color_words_in_range(start_last_line, key_values.first, line, COLOR_RED);
        throw configurationSA::ParsingErr(ROOT_DOTDOT_ERR);
    }
    if (key_values.second[0][key_values.second[0].size() - 1] == '/')
        key_values.second[0].erase(key_values.second[0].end() - 1);
    if (key_values.second[0].size() < 2)
    {
        color_words_in_range(start_last_line, key_values.second[0], line, COLOR_RED);
        throw configurationSA::ParsingErr(ROOT_SIZE_ERR);
    }
}

void configurationSA::check_body_size(key_value_type &key_values, size_t &start_last_line, std::string &line)
{
    if (!isDigit(key_values.second[0]))
    {
        color_words_in_range(start_last_line, key_values.second[0], line, COLOR_RED);
        throw configurationSA::ParsingErr(BODYSIZE_DIGIT_ERR);
    }
    
    if (atoi(key_values.second[0].c_str()) <= 0)
    {
        color_words_in_range(start_last_line, key_values.second[0], line, COLOR_RED);
        throw configurationSA::ParsingErr(BODYSIZE_OVFL_ERR + to_string(INT_MAX));
    }
    if (atoi(key_values.second[0].c_str()) > INT_MAX)
    {
        color_words_in_range(start_last_line, key_values.second[0], line, COLOR_RED);
        throw configurationSA::ParsingErr(BODYSIZE_OVFL_ERR + to_string(INT_MAX));
    }
    if (atoi(key_values.second[0].c_str()) > MAX_BODY_SIZE)
    {
        color_words_in_range(start_last_line, key_values.second[0], line, COLOR_RED);
        throw configurationSA::ParsingErr(BODYSIZE_OVFL_ERR + to_string(MAX_BODY_SIZE));
    }
}
/////////////////////////////////////////// END CHECKERS ///////////////////////////////////////////

/////////////////////////////////////////// PARSE METHODS ///////////////////////////////////////////

bool configurationSA::is_server_context(key_value_type key_value, line_range_type &line_range, file_range_type &file_range)
{
    if (key_value.first != "server")
        return (false);
    go_to_next_word_in_file(line_range, file_range);
    if (!key_value.second.empty())
        throw ParsingErr(" : Does' take parameters");
    if (line_range.first != line_range.second && *line_range.first != '{')
    {
        throw ParsingErr(" : Server context should be followed by a '{'");
    }
    return (true);
}

bool configurationSA::is_location_context(key_value_type key_value, line_range_type &line_range, file_range_type &file_range, size_t start_last_line)
{
    line_range_type line_range_copy = line_range;
    file_range_type file_range_copy = file_range;
    
    if (key_value.first != "location")
        return (false);
    
    go_to_next_word_in_file(line_range_copy, file_range_copy);
    try
    {
        if (key_value.second.size() != 1)
        {
            color_words_in_range(start_last_line, key_value.first, *file_range_copy.first, COLOR_RED);
            throw ParsingErr("Location context should have one parameter");
        }
        if (!key_value.second[0].empty() && key_value.second[0][0] != '/')
        {
            color_words_in_range(start_last_line, key_value.second[0], *file_range_copy.first, COLOR_RED);
            throw ParsingErr("Location context should start with a '/'");
        }
        if (*line_range_copy.first != '{')
        {
            color_words_in_range(start_last_line, key_value.second[0], *file_range_copy.first, COLOR_RED);
            throw ParsingErr("Location context should be followed by a '{'");
        }
        // location context value should not be "//"
        if (key_value.second[0].size() > 1 && key_value.second[0][1] == '/')
        {
            color_words_in_range(start_last_line, key_value.second[0], *file_range_copy.first, COLOR_RED);
            throw ParsingErr("Location context should not contain '//'");
        }
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(e.what());
    }
    return (true);
}

configurationSA::key_value_type configurationSA::get_keyvalue(line_range_type &line_range)
{    
    std::string                 key = get_word_skip_space(line_range);
    std::vector<std::string>    values;
    std::string                 word = get_word_skip_space(line_range);
    
    while (!word.empty())
    {
        values.push_back(word);
        word = get_word_skip_space(line_range);
    }
    return (std::make_pair(key, values));
}

bool    configurationSA::check_duplicated_parametters(std::vector<std::string> parameters, size_t &start_last_line, std::string &line)
{
    (void) start_last_line;
    (void) line;
    std::set<std::string> s;

    for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
    {
        if (s.count(*it) == 0)
            s.insert(*it);
    }
    return (s.size() != parameters.size());
}

bool configurationSA::check_valid_parametters(std::vector<std::string> parameters, std::set<std::string> validParamters, size_t &start_last_line, std::string &line)
{
    (void)  line;
    (void)  start_last_line;
    bool    errStatus = false;

    for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
    {
        if (!validParamters.count(*it))
            errStatus = true;
    }
    return (errStatus);
}

void configurationSA::check_keyvalues(key_value_type &keyVals, const configuration::raw_configuration &keyConfig, size_t start_last_line,std::string &line)
{
    std::set<std::string> sParameters(keyVals.second.begin(), keyVals.second.end());
    
    if (keyVals.second.empty())
        throw ParsingErr("Not enough parameters for key " + keyVals.first);
    
    if (!keyConfig.validParametters.empty())
    {
        if (keyConfig.keyType == configuration::UNIQUE_KEYTYPE && check_valid_parametters(keyVals.second, keyConfig.validParametters, start_last_line, line))
            throw ParsingErr("Unknown Parameter");
        else if (keyConfig.keyType == configuration::NONE_UNIQUE_KEYTYPE && !keyConfig.validParametters.count(keyVals.first))
            throw ParsingErr("Unknown Parameter");
    }

    if (check_duplicated_parametters(keyVals.second, start_last_line, line))
        throw ParsingErr("Duplicated parameters");

    if (keyConfig.max_Parameters && sParameters.size() > keyConfig.max_Parameters)
        throw ParsingErr("Too many parameters");
    
    if (keyConfig.func)
        keyConfig.func(keyVals, start_last_line, line);
}

void    configurationSA::insert_keyvalue_location(location &Location, key_value_type &key_value, size_t &start_last_line, std::string &line)
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
        throw ParsingErr(" : Already exists");
   
    try
    {
        check_keyvalues(key_value, keyConfig ,start_last_line, line);
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(keyValueFirstCopy + " : " + e.what());
    }
    insertPoint[key_value.first] = key_value.second;
}

configurationSA::location configurationSA::new_location_creation(line_range_type &line_range, file_range_type &file_range)
{
    location result;

    go_to_next_word_in_file(line_range, file_range);
    
    size_t start_last_line = (size_t) (line_range.first - file_range.first->begin());
    key_value_type key_value = get_keyvalue(line_range);
    
    while (file_range.first != file_range.second && !key_value.first.empty())
    {
        if (configuration::get_keytype(key_value.first) == configuration::UNIQUE_KEYTYPE || configuration::get_keytype(key_value.first) == configuration::NONE_UNIQUE_KEYTYPE)
            insert_keyvalue_location(result, key_value, start_last_line, *file_range.first);

        else if (configuration::get_keytype(key_value.first) == configuration::SERVER_KEYTYPE)
            throw ParsingErr(" : Server context should not be in a location context");
        
        else
            throw ParsingErr(" : Unknown key " + key_value.first);

        go_to_next_word_in_file(line_range, file_range);
        
        start_last_line = (size_t) (line_range.first - file_range.first->begin());
        
        key_value = get_keyvalue(line_range);
    }
    if (*line_range.first == '{')
        throw ParsingErr(" : Location context should not be followed by a '{'");
    
    if (file_range.first == file_range.second)
        throw ParsingErr(" : Location context should be closed by a '}'");

    line_range.first++;
    
    go_to_next_word_in_file(line_range, file_range);
    //result.print_none_unique_key();         // CGI / RETUN / ERROR PAGES
    //result.print_unique_key();
    return (result);
}

void  configurationSA::insert_keyvalue_server(Server &result, key_value_type &key_value, size_t &start_last_line, std::string &line)
{
    const configuration::raw_configuration keyConfig = configuration::_data.find(key_value.first)->second;
    
    check_keyvalues(key_value, keyConfig, start_last_line, line);
    
    if (key_value.first == "listen" && !result.listen[key_value.second[0]].insert(key_value.second[1]).second)
        throw ParsingErr("Error : Key listen already exists" + key_value.second[0] + " " + key_value.second[1]);
    
    else if (key_value.first == "server_name")
    {
        size_t old_size = result.server_name.size();
        
        result.server_name.insert(key_value.second.begin(), key_value.second.end());
        
        if (result.server_name.size() != old_size + key_value.second.size())
            throw ParsingErr(" : Duplicated server_name");
    }
}

configurationSA::Server  configurationSA::new_server_creation(line_range_type &line_range, file_range_type &file_range)
{
    location server_location_config;
    Server   result;
    
    go_to_next_word_in_file(line_range, file_range);

    size_t         start_last_line = (size_t) (line_range.first - file_range.first->begin());
    key_value_type key_value = get_keyvalue(line_range);
    
    while (file_range.first != file_range.second && !key_value.first.empty())
    {
        if (is_location_context(key_value, line_range, file_range, start_last_line))
        {
           if (result.location.count(key_value.second[0]))
                throw ParsingErr(" : Location context already exists");
           
           go_to_next_word_in_file(line_range, file_range);
           line_range.first++;
           if (key_value.second[0].size() > 1 && key_value.second[0][key_value.second[0].size() - 1] == '/')
               key_value.second[0].erase(key_value.second[0].end() - 1);
            try
            {
                //std::cout << COLOR_YELLOW <<  "LOCATION KEYS = " << key_value.second[0] << std::endl;
                if (result.first_location_key.empty())
                    result.first_location_key = key_value.second[0];
                result.location.insert(std::make_pair(key_value.second[0], new_location_creation(line_range, file_range)));
            }
            catch(ParsingErr &e)
            {
                throw ParsingErr(" : Location context : " + key_value.second[0] + " : " + e.what());
            }
        }
        else
        {
            // CGI SHOULD NOT BE IN A SERVER CONTEXT
            if (key_value.first == "cgi-bin")
                throw ParsingErr(" : CGI should not be in a server context");
            // RETURN SHOULD NOT BE IN A SERVER CONTEXT
            
            else if (key_value.first == "return")
                throw ParsingErr(" : Return should not be in a server context");
            
            else if (key_value.first == "index")
                throw ParsingErr(" : index should not be in server context");
            
            else if (key_value.first == "root")
                throw ParsingErr(" : root should not be in server context");

            else if (key_value.first == "auto_index")
                throw ParsingErr(" : auto index should not be in server context");
            
            else if (configuration::get_keytype(key_value.first) == configuration::UNIQUE_KEYTYPE || configuration::get_keytype(key_value.first) == configuration::NONE_UNIQUE_KEYTYPE)
                insert_keyvalue_location(server_location_config, key_value, start_last_line, *file_range.first);
            
            else if (configuration::get_keytype(key_value.first) == configuration::SERVER_KEYTYPE)
                insert_keyvalue_server(result, key_value, start_last_line, *file_range.first);
            
            else
                throw ParsingErr(" : Unknown key '" + key_value.first + "'");
            
            //std::cout << "key_value.first : " << key_value.first << std::endl;
        }
        
        go_to_next_word_in_file(line_range, file_range);
        
        start_last_line = (int) (line_range.first - file_range.first->begin());
        
        key_value = get_keyvalue(line_range);
        //server_location_config.print_none_unique_key();
        //server_location_config.print_unique_key();
    }

    if (*line_range.first == '{')
        throw ParsingErr(" : Server context should not be followed by a '{'");
    
    if (file_range.first == file_range.second)
        throw ParsingErr(" : Server context should be closed by a '}'");

    line_range.first++;
    //result.location["/"].insert(server_location_config);
    //result.location["/"].insert(configuration::_default_values);
    //server_location_config.print_none_unique_key();
    //server_location_config.print_unique_key();
    return (result);
}

void configurationSA::skip_charset(line_range_type &line_range, const std::string &charSet)
{
    while (line_range.first != line_range.second && charSet.find(*line_range.first) != std::string::npos)
        line_range.first++;
    
    if (line_range.first != line_range.second && configuration::is_comment.find(*line_range.first) != std::string::npos)
        line_range.first = line_range.second;
}


void configurationSA::go_to_next_word_in_file(line_range_type &line_range, file_range_type &file_range)
{    
    if (file_range.first == file_range.second)
        return ;
    
    skip_charset(line_range, configuration::is_white_space + configuration::is_line_break);
    
    while (file_range.first != file_range.second && line_range.first == line_range.second)
    {
        file_range.first++;
        if (file_range.first == file_range.second)
            break ;
        line_range.first = file_range.first->begin();
        line_range.second = file_range.first->end();
        skip_charset(line_range, configuration::is_white_space + configuration::is_line_break);
    }
}

std::string     configurationSA::get_word(line_range_type &line_range)
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


std::string     configurationSA::get_word_skip_space(line_range_type &line_range)
{
    skip_charset(line_range, configuration::is_white_space); 
    std::string word = get_word(line_range);
    skip_charset(line_range, configuration::is_white_space);
    return (word);
}

///////////////////////// ACCESSORS : /////////////////////////

configurationSA::data_type configurationSA::get_data(void)
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
       throw ParsingErr(" : File does not open : " + std::string(config_file));
    }
    // Read file line by line.

    for (std::string line; !input.eof();)
    {
        std::getline(input, line);
        fullFile.push_back(line);
    }
    // close fd
    input.close();    
    // Check if file is empty.
    
    line_range_type line_range(fullFile.begin()->begin(), fullFile.begin()->end());
    file_range_type file_range(fullFile.begin(), fullFile.end());
    try
    {
        while (file_range.first != file_range.second)
        {
            go_to_next_word_in_file(line_range, file_range);
            if(is_server_context(get_keyvalue(line_range), line_range, file_range))
            {
                line_range.first++;
                go_to_next_word_in_file(line_range, file_range);
                try
                {
                    _data.push_back(new_server_creation(line_range, file_range));
                }
                
                catch (ParsingErr &e)
                {
                    throw ParsingErr("Server " + std::to_string(_data.size()) + " : " + e.what()); 
                }
            }
            else if (file_range.first != file_range.second)
                throw ParsingErr(" : Wrong server context");
            
            go_to_next_word_in_file(line_range, file_range);
        }
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(std::string(e.what()) + "\n" + "line " + std::to_string(fullFile.size() - (file_range.second - file_range.first) + 1) + " : " + \
        ((file_range.first == file_range.second) ? *(file_range.first - 1) : *file_range.first));
    }
    //input.close();
}