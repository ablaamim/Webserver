#include "ConfigurationParser.hpp"

configurationSA::configuration::data_type configurationSA::configuration::_data = configurationSA::configuration::data_type();

const std::string configurationSA::configuration::is_white_space = "\t ";
const std::string configurationSA::configuration::is_comment = "#";
const std::string configurationSA::configuration::is_line_break = ";";
const std::string configurationSA::configuration::is_scope = "{}";

std::string remove_front_whitespaces(std::string str)
{
    size_t i = 0;
    while (i < str.size() && configurationSA::configuration::is_white_space.find(str[i]) != std::string::npos)
        i++;
    return (str.substr(i));
}


void configurationSA::configuration::initialize_data(void)
{
    if (!_data.empty())
       return ;
    std::string allowedMethods[] = {"GET", "POST", "DELETE"};
    std::string autoindex[] = {"on", "off"};
    std::string ErrorPages[] = {}; 
    std::string returnCode[] = {};
    
    std::pair<std::string, raw_configuration> data[] =
    {
        std::make_pair("server_name", raw_configuration(SERVER_KEYTYPE, NULL, UNLIMITED_PARAMS)),                                  
        std::make_pair("listen", raw_configuration(SERVER_KEYTYPE, &listen_format, 2)),     
        std::make_pair("cgi-bin", raw_configuration(NONE_UNIQUE_KEYTYPE, &check_cgi, 1)),                                     
        std::make_pair("return", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, 1, returnCode, SIZEOF(returnCode))),              
        std::make_pair("error_pages", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, 1, ErrorPages, SIZEOF(ErrorPages))),         
        std::make_pair("auto_index", raw_configuration(UNIQUE_KEYTYPE, NULL, 1, autoindex, SIZEOF(autoindex))),                 
        std::make_pair("upload_pass", raw_configuration(UNIQUE_KEYTYPE, NULL, 1)),                                                   
        std::make_pair("index", raw_configuration(NONE_UNIQUE_KEYTYPE, NULL, UNLIMITED_PARAMS)),                                    
        std::make_pair("root",   raw_configuration(UNIQUE_KEYTYPE, &check_root, 1)),                                            
        std::make_pair("allowed_methods", raw_configuration(UNIQUE_KEYTYPE, NULL, 3, allowedMethods, SIZEOF(allowedMethods))),  
        std::make_pair("max_body_size", raw_configuration(UNIQUE_KEYTYPE, &check_body_size, 1)), 
    };
     _data.insert(data, data + SIZEOF(data));
}

configurationSA::location configurationSA::configuration::_default_values = configurationSA::location();

void configurationSA::configuration::initialize_default_values(void)
{    
    if (!_default_values.NoneUniqueKey.empty() && !_default_values.UniqueKey.empty())
        return ;

    std::string allowed_methods[] = {"GET"};
    
    std::pair <std::string, std::vector<std::string> > uniqueKey[] =
    {
        std::make_pair("auto_index", std::vector<std::string>(1, "off")),
        std::make_pair("max_body_size", std::vector<std::string>(1, "1000000000")),
        std::make_pair("allowed_methods", std::vector<std::string>(allowed_methods, allowed_methods + SIZEOF(allowed_methods))),
    };

    _default_values.UniqueKey.insert(uniqueKey, uniqueKey + SIZEOF(uniqueKey));
}

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
    (void)                start_last_line;
    (void)                line;
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
            throw ParsingErr("Unknown Parameter for key " + keyVals.first);
        else if (keyConfig.keyType == configuration::NONE_UNIQUE_KEYTYPE && !keyConfig.validParametters.count(keyVals.first))
            throw ParsingErr("Unknown Parameter for key " + keyVals.first);
    }
    if (check_duplicated_parametters(keyVals.second, start_last_line, line))
        throw ParsingErr("Duplicated parameters for key " + keyVals.first);
    if (keyConfig.max_Parameters && sParameters.size() > keyConfig.max_Parameters)
        throw ParsingErr("Too many parameters for key " + keyVals.first);
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
        throw ParsingErr("Already exists" + keyValueFirstCopy);
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
            throw ParsingErr("Server context should not be in a location context");
        
        else
            throw ParsingErr("Unknown key " + key_value.first);

        go_to_next_word_in_file(line_range, file_range);
        
        start_last_line = (size_t) (line_range.first - file_range.first->begin());
        
        key_value = get_keyvalue(line_range);
    }
    if (*line_range.first == '{')
        throw ParsingErr("Location context should not be followed by a '{'");
    
    if (file_range.first == file_range.second)
        throw ParsingErr("Location context should be closed by a '}'");

    line_range.first++;    
    go_to_next_word_in_file(line_range, file_range);
    return (result);
}

void  configurationSA::insert_keyvalue_server(Server &result, key_value_type &key_value, size_t &start_last_line, std::string &line)
{
    const configuration::raw_configuration keyConfig = configuration::_data.find(key_value.first)->second;
    
    check_keyvalues(key_value, keyConfig, start_last_line, line);
    if (key_value.first == "listen")
        result.listen[key_value.second[0]].insert(key_value.second[1]);
    if (result.listen[key_value.second[0]].size() > 1)
        throw ParsingErr("More than one listen");
    else if (key_value.first == "server_name")
    {
        size_t old_size = result.server_name.size();
        
        if (key_value.second.size() > 1)
            throw ParsingErr("Too many parameters for key " + key_value.first);
        
        result.server_name.insert(key_value.second.begin(), key_value.second.end());
        
        if (result.server_name.size() != old_size + key_value.second.size())
            throw ParsingErr("Duplicated server_name");
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
                throw ParsingErr("Location context already exists");
           
           go_to_next_word_in_file(line_range, file_range);
           line_range.first++;
           if (key_value.second[0].size() > 1 && key_value.second[0][key_value.second[0].size() - 1] == '/')
               key_value.second[0].erase(key_value.second[0].end() - 1);
            try
            {
                if (result.first_location_key.empty())
                    result.first_location_key = key_value.second[0];
                result.location.insert(std::make_pair(key_value.second[0], new_location_creation(line_range, file_range)));
            }
            catch(ParsingErr &e)
            {
                throw ParsingErr("Location context : " + key_value.second[0] + " : " + e.what());
            }
        }
        else
        {
            if (key_value.first == "cgi-bin")
                throw ParsingErr("CGI should not be in a server context");            
            else if (key_value.first == "return")
                throw ParsingErr("Return should not be in a server context");
            else if (key_value.first == "index")
                throw ParsingErr("index should not be in server context");
            else if (key_value.first == "root")
            {
                result.root = key_value.second[0];
            }    
            else if (key_value.first == "auto_index")
                throw ParsingErr("Auto index should not be in server context");
            else if (key_value.first == "error_pages")
                throw ParsingErr("Error pages should not be in server context");
            else if (configuration::get_keytype(key_value.first) == configuration::UNIQUE_KEYTYPE || configuration::get_keytype(key_value.first) == configuration::NONE_UNIQUE_KEYTYPE)
                insert_keyvalue_location(server_location_config, key_value, start_last_line, *file_range.first);
            else if (configuration::get_keytype(key_value.first) == configuration::SERVER_KEYTYPE)
                insert_keyvalue_server(result, key_value, start_last_line, *file_range.first);
            else
                throw ParsingErr("Unknown key '" + key_value.first + "'");
        }
        go_to_next_word_in_file(line_range, file_range);
        start_last_line = (int) (line_range.first - file_range.first->begin());
        key_value = get_keyvalue(line_range);
    }

    if (*line_range.first == '{')
        throw ParsingErr("Server context should not be followed by a '{'");
    
    if (file_range.first == file_range.second)
        throw ParsingErr("Server context should be closed by a '}'");

    line_range.first++;
    if(result.location.empty())
    {
        result.location["/"].insert(server_location_config);
        result.location["/"].insert(configuration::_default_values);
    }
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

configurationSA::data_type configurationSA::get_data(void)
{
    return (_data);
}

configurationSA::configurationSA(char *config_file)
{
    std::ifstream            input(config_file);
    std::vector<std::string> fullFile;

    configuration::initialize_data();
    configuration::initialize_default_values();
    if (!input.is_open())
    {
       throw ParsingErr("File does not open : " + std::string(config_file));
    }
    for (std::string line; !input.eof();)
    {
        std::getline(input, line);
        fullFile.push_back(line);
    }
    input.close();    
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
                throw ParsingErr("Wrong server context");
            go_to_next_word_in_file(line_range, file_range);
        }
    }
    catch (ParsingErr &e)
    {
        throw ParsingErr(std::string(e.what()) + "\n" + "ParseError at Line " + std::to_string(fullFile.size() - (file_range.second - file_range.first) + 1) +  ",Col " + std::to_string(line_range.first - file_range.first->begin() + 1) + " : " + remove_front_whitespaces(*file_range.first));
    }
}

/*
* COLORS :
*/

void configurationSA::color_words_in_range(size_t &start, const std::string &word, std::string &line, \
const std::string &color)
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

configurationSA::~configurationSA(){}
configurationSA::configurationSA(){}