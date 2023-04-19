#ifndef CONFIGURATION_PARSER_HPP // BEGIN OF CONFIGURATION_PARSER_HPP
# define CONFIGURATION_PARSER_HPP

# include <exception>
# include "../MainInc/main.hpp"
# include "debug.hpp"
# include "libcpp.hpp"
# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <algorithm>
# include <iterator>
# include <set>
# include <stdlib.h>

# define UNLIMITED_PARAMS 0
# define SIZEOF(arr) sizeof(arr) / sizeof(*arr)
# define DEFAULT_LISTEN_INTERFACE "0.0.0.0"
# define DEFAULT_LISTEN_PORT "8080"
# define PORT_MAX_VALUE 65535

class configurationSA   // BEGIN OF CONFIGURATIONSA
{
    private :
        typedef std::string::iterator                             line_iterator;   // iterator for a line in configuration file
        typedef std::vector<std::string>::iterator                file_iterator;   // iterator for a file in configuration file
        typedef std::pair<line_iterator, line_iterator>           line_range_type; // pair of iterators for a line in configuration file
        typedef std::pair<file_iterator, file_iterator>           file_range_type; // pair of iterators for a file in configuration file
        typedef std::pair<std::string, std::vector<std::string> > key_value_type;  // pair of key and value in configuration file
    
    // I will use two inner structs named 'Location' && 'Server'
    public :

        // Location struct will contain a map of none unique keys and a map of unique keys
        class location // BEGININING OF LOCATION STRUCT
        {
            public :
            
                typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t;
                typedef std::map<std::string, std::vector<std::string> >                         UniqueKey_t;
            
                UniqueKey_t     UniqueKey;
                NoneUniqueKey_t NoneUniqueKey;

                // Insert a unique key in the location struct
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
            }; // END OF LOCATION STRUCT
        
            // Server struct will contain a map of locations and a set of server 
            // names and a map of listen ports.
            class Server // BEGIN OF SERVER STRUCT
            {
                public :

                    typedef std::map<std::string, location>               type_location;     // map of locations
                    typedef std::map<std::string, std::set<std::string> > type_listen;       // map of listen ports and interfaces (ip, set<port>)
                    typedef std::set<std::string>                         type_server_name; // set of server names


                    type_listen        listen;      // map of listen ports and interfaces (ip, set<port>)
                    type_server_name   server_name;  // set of server names
                    type_location      location;    // map of locations
             
            }; // END OF SERVER STRUCT

            private :

                // Configuration struct :
                class configuration // BEGIN OF CONF STRUCT
                {
                    public :
                
                        enum KEYTYPE
                        {
                            NONE_KEYTYPE,
                            SERVER_KEYTYPE,
                            UNIQUE_KEYTYPE,
                            NONE_UNIQUE_KEYTYPE
                        };
                        // sub struct it contains information about each defined key 
                        // in the configuration file.
                        class raw_configuration // FINAL CONTAINER
                        {
                            public :

                                KEYTYPE                 keyType;
                                void                    (*func)(key_value_type &, size_t &start_last_line, std::string &line);
                                size_t                  max_Parameters;
                                std::set<std::string>   validParametters;

                                raw_configuration(void){};
                    
                                raw_configuration(const KEYTYPE &keytype, void (*func)(key_value_type &, size_t &start_last_line, std::string &line), size_t maxParameters, std::string validParameterstab[], size_t validParamettersSize)
                                : keyType(keytype), func(func), max_Parameters(maxParameters), validParametters(validParameterstab, validParameterstab + validParamettersSize)
                                {};
                    
                                raw_configuration(const KEYTYPE &keytype, void (*func)(key_value_type &, size_t &start_last_line, std::string &line), size_t maxParameters)
                                : keyType(keytype), func(func), max_Parameters(maxParameters)
                                {
                        };
                };

                // map of raw_configuration, key = key name, value = raw_configuration struct :
                typedef std::map<std::string, raw_configuration> data_type;
                
                // map of rawConf, key = key name, value = rawConf struct :
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
        }; // END OF CONF STRUCT

    public :

        typedef std::vector<Server> data_type;    
    
    private :

        data_type   _data;

/////////////////////////////////// PARSING FUNCTIONS LOGIC : ///////////////////////////////////////

    static void     listen_format(key_value_type &key_value, size_t &start_last_line, std::string &line);
    static void     check_port(std::string str);
    static void     check_root(key_value_type &key_values, size_t &start_last_line, std::string &line);
    static void     check_ip(std::vector<std::string> ip);
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

///////////////////////////////// END PARSING FUNCTIONS LOGIC : /////////////////////////////////////

    public :
        // CONSTRUCTORS AND DESTRUCTORS :
        //configurationSA();
        
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
