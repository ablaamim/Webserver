#ifndef CONFIGURATION_PARSER_HPP 
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
# include "../parsing/libcpp.hpp"

class configurationSA
{
    private :
        typedef std::string::iterator                             line_iterator;   
        typedef std::vector<std::string>::iterator                file_iterator;   
        typedef std::pair<line_iterator, line_iterator>           line_range_type; 
        typedef std::pair<file_iterator, file_iterator>           file_range_type; 
        typedef std::pair<std::string, std::vector<std::string> > key_value_type;
    
    public :
        class location
        {
            public :
                typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t;
                typedef std::map<std::string, std::vector<std::string> >                         UniqueKey_t;
            
                UniqueKey_t     UniqueKey;
                NoneUniqueKey_t NoneUniqueKey;

                location()
                {
                }
                
                ~location()
                {
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

                static void insert_unique_key(const UniqueKey_t &lval, UniqueKey_t &rval)
                {
                    for (UniqueKey_t::const_iterator it = lval.begin(); it != lval.end(); it++)
                        rval.insert(*it);
                }
                void insert(const location &otherInsert)
                {
                    insert_unique_key(otherInsert.UniqueKey, UniqueKey);
                    for (NoneUniqueKey_t::const_iterator iter = otherInsert.NoneUniqueKey.begin(); iter != otherInsert.NoneUniqueKey.end(); iter++)
                        insert_unique_key(iter->second, NoneUniqueKey[iter->first]);
                }
            
            };
            class Server 
            {
                public :

                    Server(){}
                    ~Server(){}
                    typedef std::map<std::string, location>               type_location;     // map of locations
                    typedef std::map<std::string, std::set<std::string> > type_listen;       // map of listen ports and interfaces (ip, set<port>)
                    typedef std::string                                   type_server_name; // set of server names
                    std::string                                           first_location_key;
                    std::string                                           root;
                    std::string                                           port;
                    std::string                                           ip;
                    
                    type_listen                                           listen;       // map of listen ports and interfaces (ip, set<port>)
                    type_server_name                                      server_name;  // set of server names
                    type_location                                         location;     // map of locations
                    
                    void print_type_listen()
                    {
                        if (listen.empty())
                        {
                            std::cout << COLOR_RED << "listen is empty" << COLOR_RESET << std::endl;
                            return ;
                        }
                        for (type_listen::iterator it = listen.begin(); it != listen.end(); it++)
                        {
                            std::cout << COLOR_BLUE << "Listen : " << COLOR_RESET << COLOR_YELLOW << " [" << it->first << "]" << COLOR_RESET;
                            
                            std::cout << std::endl;
                        }
                    };

                    void print_server_name()
                    {
                        if (server_name.empty())
                        {
                            std::cout << COLOR_RED << "server_name is empty" << COLOR_RESET << std::endl;
                            return ;
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
                    type_location &get_location()
                    {
                        return (location);
                    }
             
            };

            public :
                class configuration
                {
                    public :
                        enum KEYTYPE
                        {
                            NONE_KEYTYPE,           
                            SERVER_KEYTYPE,         
                            UNIQUE_KEYTYPE,         
                            NONE_UNIQUE_KEYTYPE     
                        };
                        class raw_configuration 
                        {
                            public :

                                KEYTYPE                 keyType;
                                void                    (*func)(key_value_type &, size_t &start_last_line, std::string &line); // pointer to a function that will be used to check the key value
                                size_t                  max_Parameters;
                                std::set<std::string>   validParametters;

                                raw_configuration()
                                {
                                };
                    
                                raw_configuration(const KEYTYPE &keytype, void (*func)(key_value_type &, size_t &start_last_line, std::string &line), size_t maxParameters, std::string validParameterstab[], size_t validParamettersSize)
                                : keyType(keytype), func(func), max_Parameters(maxParameters), validParametters(validParameterstab, validParameterstab + validParamettersSize)
                                {};
                    
                                raw_configuration(const KEYTYPE &keytype, void (*func)(key_value_type &, size_t &start_last_line, std::string &line), size_t maxParameters)
                                : keyType(keytype), func(func), max_Parameters(maxParameters)
                                {
                                };
                };

                typedef std::map<std::string, raw_configuration> data_type;
                static data_type                       _data;
                static location                        _default_values;
                static void                            initialize_data(void);
                static void                            initialize_default_values(void);             
                const static std::string               is_white_space;
                const static std::string               is_line_break;
                const static std::string               is_comment;
                const static std::string               is_scope;

                static void print_default_values()
                {
                    std::cout << COLOR_BLUE << "Default values : " << COLOR_RESET << std::endl;
                    _default_values.print_unique_key();
                    _default_values.print_none_unique_key();
                }
                
                static KEYTYPE                        get_keytype(const std::string &key)
                {
                    return ((_data.count(key)) ? _data[key].keyType : NONE_KEYTYPE);
                }
        
        };

    public :
        
        typedef     std::vector<Server> data_type;    
        data_type   _data;

        void print_data_type()
        {
            for (data_type::iterator it = _data.begin(); it != _data.end(); it++)
            {
                std::cout << COLOR_BLUE << "Server : " << COLOR_RESET << COLOR_YELLOW << " [" << it->first_location_key << " ]" << COLOR_RESET << std::endl;
                it->print_type_listen();
                it->print_server_name();
                it->print_type_location();
            }
        }  

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
    
    public :
        configurationSA(char *config_file);
        configurationSA();
        ~configurationSA();        
        data_type get_data(void);
        
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
}; 

#endif 