#ifndef CONFIGURATION_PARSER_HPP // BEGIN OF CONFIGURATION_PARSER_HPP
# define CONFIGURATION_PARSER_HPP

# include <iostream>
# include <iomanip>
# include <fstream>
# include <fcntl.h>
# include <sstream>
# include <exception>
# include <cstring>
# include <algorithm>
# include <iterator>
# include <vector>
# include <map>
# include <set>
# include <list>
# include <unistd.h>
# include "../defines/defines.hpp"
# include "../parsing/debug.hpp"
# include "../parsing/libcpp.hpp"

//class Response;

class Webserv;

class configurationSA   // BEGIN OF CONFIGURATIONSA "SA means SYNTAX ANALYSIS"
{
    private :
        typedef std::string::iterator                             line_iterator;   // iterator for a line in configuration file
        typedef std::vector<std::string>::iterator                file_iterator;   // iterator for a file in configuration file
        typedef std::pair<line_iterator, line_iterator>           line_range_type; // pair of iterators for a line in configuration file
        typedef std::pair<file_iterator, file_iterator>           file_range_type; // pair of iterators for a file in configuration file
        typedef std::pair<std::string, std::vector<std::string> > key_value_type;  // pair of key and value in configuration file
    
    public :
        // Location struct will contain a map of none unique keys and a map of unique keys
        class location // BEGININING OF LOCATION 
        {
            public :
                
                typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t; // map of none unique keys that have more than one value
                typedef std::map<std::string, std::vector<std::string> >                         UniqueKey_t;     // map of unique keys that have only one value
            
                UniqueKey_t     UniqueKey;
                NoneUniqueKey_t NoneUniqueKey;

                location()
                {
                    //std::cout << "location constructor" << std::endl;
                    //this->UniqueKey.clear();
                    //this->NoneUniqueKey.clear();
                }
                
                ~location()
                {
                    //std::cout << "location destructor" << std::endl;
                }

                bool error_if_empty_keys()
                {
                    if (UniqueKey.empty() || NoneUniqueKey.empty())         
                    {
                        return 1;
                    }
                    return 0;
                };

                void print_unique_key()
                {
                    if (UniqueKey.empty())
                    {
                        std::cout << COLOR_RED << "UniqueKey is empty" << COLOR_RESET << std::endl;
                        return ;
                    }
                    for (UniqueKey_t::const_iterator it = UniqueKey.begin(); it != UniqueKey.end(); it++)
                    {
                        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET;
                        for (std::vector<std::string>::const_iterator iter = it->second.begin(); iter != it->second.end(); iter++)
                            std::cout << COLOR_YELLOW << *iter << " " << COLOR_RESET;
                        std::cout << std::endl;
                        // if (it->first == "root")
                        //     std::cout << COLOR_BLUE << "root : " << COLOR_RESET << COLOR_YELLOW << it->second[0] << COLOR_RESET << std::endl;
                        // if (it->first == "upload")
                        //     std::cout << COLOR_BLUE << "upload : " << COLOR_RESET << COLOR_YELLOW << it->second[0] << COLOR_RESET << std::endl;
                    }

                };

                void print_none_unique_key()
                {
                    if (NoneUniqueKey.empty())
                    {
                        std::cout << COLOR_RED << "NoneUniqueKey is empty" << COLOR_RESET << std::endl;
                        return ;
                    }
                    for (NoneUniqueKey_t::const_iterator it = NoneUniqueKey.begin(); it != NoneUniqueKey.end(); it++)
                    {
                        std::cout << "          " << COLOR_GREEN << it->first << COLOR_RESET << std::endl;
                        
                        for (std::map<std::string, std::vector<std::string> >::const_iterator iter = it->second.begin(); iter != it->second.end(); iter++)
                        {
                            std::cout << COLOR_BLUE << iter->first << " : " << COLOR_RESET;
                            for (std::vector<std::string>::const_iterator ite = iter->second.begin(); ite != iter->second.end(); ite++)
                                std::cout << COLOR_YELLOW << *ite << " " << COLOR_RESET;
                            std::cout << std::endl;
                        }
                    }
                };

                // Insert a unique key in the location 
                static void insert_unique_key(const UniqueKey_t &lval, UniqueKey_t &rval)
                {
                    for (UniqueKey_t::const_iterator it = lval.begin(); it != lval.end(); it++)
                        rval.insert(*it);
                }
                // Insert a none unique key in the location struct
                void insert(const location &otherInsert)
                {
                    insert_unique_key(otherInsert.UniqueKey, UniqueKey);
                    // Insert none unique keys in the location struct
                    for (NoneUniqueKey_t::const_iterator iter = otherInsert.NoneUniqueKey.begin(); iter != otherInsert.NoneUniqueKey.end(); iter++)
                        insert_unique_key(iter->second, NoneUniqueKey[iter->first]);
                }
            
            }; // END OF LOCATION 
            // Server struct will contain a map of locations and a set of server 
            // names and a map of listen ports.
            class Server // BEGIN OF SERVER
            {
                public :

                    Server()
                    {
                        //std::cout << "Server constructor" << std::endl;
                    }

                    ~Server()
                    {
                        //std::cout << "Server destructor" << std::endl;
                    }
                    // get first location
                    typedef std::map<std::string, location>               type_location;     // map of locations
                    typedef std::map<std::string, std::set<std::string> > type_listen;       // map of listen ports and interfaces (ip, set<port>)
                    typedef std::set<std::string>                         type_server_name; // set of server names
                    std::string                                           first_location_key;
                    
                    type_listen                                           listen;       // map of listen ports and interfaces (ip, set<port>)
                    type_server_name                                      server_name;  // set of server names
                    type_location                                         location;     // map of locations
                    
                    void print_type_listen()
                    {
                        if (listen.empty())
                        {
                            std::cout << COLOR_RED << "listen is empty" << COLOR_RESET << std::endl;
                            exit(EXIT_FAILURE);
                        }
                        for (type_listen::const_iterator it = listen.begin(); it != listen.end(); it++)
                        {
                            std::cout << COLOR_BLUE << "IP / PORT     : " << COLOR_RESET << COLOR_YELLOW <<"[ "<< it->first << " : " << COLOR_RESET;
                            for (std::set<std::string>::iterator iter = it->second.begin(); iter != it->second.end(); iter++)
                                std::cout << COLOR_YELLOW << *iter << " ]" << COLOR_RESET;
                            std::cout << std::endl;
                        }
                    };

                    void print_server_name()
                    {
                        if (server_name.empty())
                        {
                            std::cout << COLOR_RED << "server_name is empty" << COLOR_RESET << std::endl;
                            exit(EXIT_FAILURE);
                        }
                        for (type_server_name::iterator it = server_name.begin(); it != server_name.end(); it++)
                            std::cout << COLOR_BLUE << "Server_name : " << COLOR_RESET << COLOR_YELLOW << " [" << *it << " ]" << COLOR_RESET;
                        std::cout << std::endl;
                    };

                    void print_type_location()
                    {
                        if (location.empty())
                        {
                            std::cout << COLOR_RED << "location is empty" << COLOR_RESET << std::endl;
                            exit(EXIT_FAILURE);
                        }
                        for (type_location::iterator it = location.begin(); it != location.end(); it++)
                        {
                            std::cout << COLOR_BLUE << "Location : " << COLOR_RESET << COLOR_YELLOW << " [" << it->first << " ]" << COLOR_RESET << std::endl;
                            it->second.print_unique_key();
                            it->second.print_none_unique_key();
                        }
                    }

                    // get type location
                    type_location &get_location()
                    {
                        return (location);
                    }
             
            }; // END OF SERVER 

            private :
                
                // Configuration 
                class configuration // BEGIN OF CONF 
                {
                    public :
                        enum KEYTYPE
                        {
                            NONE_KEYTYPE,           // NONE_KEYTYPE is used to check if the key is defined in the configuration file
                            SERVER_KEYTYPE,         // SERVER_KEYTYPE is used to check if the key is a server key
                            UNIQUE_KEYTYPE,         // UNIQUE_KEYTYPE is used to check if the key is a unique key
                            NONE_UNIQUE_KEYTYPE     // NONE_UNIQUE_KEYTYPE is used to check if the key is a none unique key
                        };
                        // sub struct it contains information about each defined key 
                        // in the configuration file.
                        class raw_configuration // FINAL CONTAINER
                        {
                            public :

                                KEYTYPE                 keyType;
                                void                    (*func)(key_value_type &, size_t &start_last_line, std::string &line); // pointer to a function that will be used to check the key value
                                size_t                  max_Parameters;
                                std::set<std::string>   validParametters;

                                raw_configuration()
                                {
                                    //std::cout << "raw_configuration default constructor" << std::endl;
                                };
                    
                                raw_configuration(const KEYTYPE &keytype, void (*func)(key_value_type &, size_t &start_last_line, std::string &line), size_t maxParameters, std::string validParameterstab[], size_t validParamettersSize)
                                : keyType(keytype), func(func), max_Parameters(maxParameters), validParametters(validParameterstab, validParameterstab + validParamettersSize)
                                {};
                    
                                raw_configuration(const KEYTYPE &keytype, void (*func)(key_value_type &, size_t &start_last_line, std::string &line), size_t maxParameters)
                                : keyType(keytype), func(func), max_Parameters(maxParameters)
                                {
                                };
                };

                // map of raw_configuration, key = key name, value = raw_configuration :
                typedef std::map<std::string, raw_configuration> data_type;
                // map of rawConf, key = key name, value = rawConf s:
                //typedef std::map<std::string, rawConf> data_type;
                static data_type                       _data;
                static location                        _default_values;
                static void                            initialize_data(void);
                static void                            initialize_default_values(void);             
                const static std::string               is_white_space;
                const static std::string               is_line_break;
                const static std::string               is_comment;
                const static std::string               is_scope;
                
                static KEYTYPE                        get_keytype(const std::string &key)
                {
                    return ((_data.count(key)) ? _data[key].keyType : NONE_KEYTYPE);
                }
        
        }; // END OF CONF 

    public :
        
        typedef std::vector<Server> data_type;    
    
        data_type   _data;
        
        void print_data_type()
        {
            if (_data.empty())
            {
                std::cout << "Data is empty" << std::endl;
                return ;
            }
            for (data_type::const_iterator it = _data.begin(); it != _data.end(); it++)
            {
                std::cout << "Server name : " << std::endl;
                
                for (std::set<std::string>::const_iterator iter = it->server_name.begin(); iter != it->server_name.end(); iter++)
                    std::cout << *iter << " ";
                std::cout << std::endl;
                std::cout << "Listen : " << std::endl;
                
                for (Server::type_listen::const_iterator iter = it->listen.begin(); iter != it->listen.end(); iter++)
                {
                    std::cout << iter->first << std::endl;
                    for (std::set<std::string>::const_iterator ite = iter->second.begin(); ite != iter->second.end(); ite++)
                        std::cout << *ite << std::endl;
                }
            }
        };
/////////////////////////////////// PARSING FUNCTIONS LOGIC : ///////////////////////////////////////
    static void     listen_format(key_value_type &key_value, size_t &start_last_line, std::string &line);
    static void     check_port(std::string str, size_t &start_last_line, std::string &line);
    static void     check_root(key_value_type &key_values, size_t &start_last_line, std::string &line);
    static void     check_ip(std::vector<std::string> ip, size_t &start_last_line, std::string &line);
    static void     check_cgi(key_value_type &key_value, size_t &start_last_line, std::string &line);
    static void     check_body_size(key_value_type &key_value, size_t &start_last_line, std::string &line);
    std::string     get_word(line_range_type &line_range);
    void            go_to_next_word_in_file(line_range_type &line_range, file_range_type &file_range);
    void            skip_charset(line_range_type &line_range, const std::string &charSet);
    std::string     get_word_skip_space(line_range_type &line_range);
    bool            is_server_context(key_value_type key_value, line_range_type &line_range, file_range_type &file_range);
    key_value_type  get_keyvalue(line_range_type &line_range);
    bool            is_location_context(key_value_type key_value, line_range_type &line_range, file_range_type &file_range, size_t start_last_line);
    bool            check_duplicated_parametters(std::vector<std::string> parameters, size_t &start_last_line, std::string &line);
    bool            check_valid_parametters(std::vector<std::string> parameters, std::set<std::string> validParamters, size_t &start_last_line, std::string &line);
    void            insert_keyvalue_location(location &location, key_value_type &key_value, size_t &start_last_line, std::string &line);
    void            check_keyvalues(key_value_type &keyVals, const configuration::raw_configuration &keyConfig, size_t start_last_line,std::string &line);
    location        new_location_creation(line_range_type &line_range, file_range_type &file_range);
    void            insert_keyvalue_server(Server &server, key_value_type &key_value, size_t &start_last_line, std::string &line); 
    Server          new_server_creation(line_range_type &line_range, file_range_type &file_range);
    static void     color_words_in_range(size_t &start, const std::string &word, std::string &line, const std::string &color);
///////////////////////////////// END PARSING FUNCTIONS LOGIC : ///////////////////////////////////////
    public :
        // CONSTRUCTORS AND DESTRUCTORS :
        configurationSA()
        {};
        
        configurationSA(char *config_file);
        
        ~configurationSA();
                
        // GETTERS AND SETTERS :
        data_type get_data(void);

        // EXCEPTIONS :
        class ParsingErr : public std::exception
        {
            private :
                std::string _word;
                std::string _err;
            
            public :
                ParsingErr(const std::string &err, const std::string &word = std::string()) : _word(word), _err(err) {};            
                virtual ~ParsingErr(void) throw() {};
                virtual const char *what() const throw()
                {
                    return (_err.c_str());
                }
                std::string word(void)
                {
                    return (_word);
                }
        };
}; // END OF CONFIGURATIONSA

#endif // CONFIGURATIONSA_HPP