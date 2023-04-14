#ifndef CONFIGURATION_PARSER_HPP
# define CONFIGURATION_PARSER_HPP

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

// All the typedefs necessary for the configuration file parsing :

#define UNLIMITED_PARAMS 0
#define SIZEOF(arr) sizeof(arr) / sizeof(*arr)
#define DEFAULT_LISTEN_INTERF "0.0.0.0"
#define DEFAULT_LISTEN_PORT "8080"

typedef std::string::iterator                             line_iterator;
typedef std::vector<std::string>::iterator                file_iterator;
typedef std::pair<line_iterator, line_iterator>           line_range_type;
typedef std::pair<file_iterator, file_iterator>           file_range_type;
typedef std::pair<std::string, std::vector<std::string> > key_value_type;

// Server type typedefs :

typedef std::map<std::string, std::set<std::string> > typeListen; // key = IP address, value = set of ports
typedef std::set<std::string>                         typeServerName_t;

// Configuration file Syntax analysis class

class configurationSA   // BEGIN OF CONFIGURATIONSA
{
    // I will use two inner structs named 'Location' && 'Server'
    public :

        // Location struct will contain a map of none unique keys and a map of unique keys
        struct location // BEGIN OF LOCATION STRUCT
        {
            typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t;
            typedef std::map<std::string, std::vector<std::string> >                         UniqueKey_t;
            
            UniqueKey_t     UniqueKey;
            NoneUniqueKey_t NoneUniqueKey;

            static void insertUniqueKey(const UniqueKey_t &a, UniqueKey_t &b)
            {
                for (UniqueKey_t::const_iterator it = a.begin(); it != a.end(); it++)
                    b.insert(*it);
            }

            void insert(const location &otherInsert)
            {
                insertUniqueKey(otherInsert.UniqueKey, UniqueKey);
                for (NoneUniqueKey_t::const_iterator it = otherInsert.NoneUniqueKey.begin(); it != otherInsert.NoneUniqueKey.end(); it++)
                    insertUniqueKey(it->second, NoneUniqueKey[it->first]);
            }
        }; // END OF LOCATION STRUCT

        // map of locations, key = location path, value = location struct :

        typedef std::map<std::string, location>               typeLocation;
        
        // Server struct will contain a map of locations and a set of server 
        // names and a map of listen ports.

        struct Server // BEGIN OF SERVER STRUCT
        {
            typeListen        Listen;
            typeServerName_t  ServerName;
            typeLocation      Location;
            
        }; // END OF SERVER STRUCT

    private :

        // Configuration struct :
        struct conf // BEGIN OF CONF STRUCT
        {
            enum KEYTYPE
            {
                NONE_KEYTYPE,
                UNIQUE_KEYTYPE,
                NONE_UNIQUE_KEYTYPE,
                SERVER_KEYTYPE
            };
                // sub struct it contains information about each defined key 
                // in the configuration file.

                struct rawConf // FINAL CONTAINER
                {
                    KEYTYPE keyType;
                    void    (               *func)(key_value_type &, int &start_last_line, std::string &line);
                    int                     max_Parameters;
                    std::set<std::string>   validParametters;
                    rawConf(void){};
                    rawConf(const KEYTYPE &keytype, void (*func)(key_value_type &, int &start_last_line, std::string &line), int maxParameters, std::string validParameterstab[], int validParamettersSize)
                    {
                        keyType = keytype;
                        func = func;
                        max_Parameters = maxParameters;
                        validParametters = std::set<std::string>(validParameterstab, validParameterstab + validParamettersSize);
                    };
                    rawConf(const KEYTYPE &keytype, void (*func)(key_value_type &, int &start_last_line, std::string &line), int maxParameters)
                    {
                        keyType = keytype;
                        func = func;
                        max_Parameters = maxParameters;
                    };
                };

                // map of rawConf, key = key name, value = rawConf struct :
                typedef std::map<std::string, rawConf> data_type;

////////////////////////////////////////   DEBUG SECTION  ////////////////////////////////////////

                static void printMap(data_type mapofConfs)
                {
                    for (auto const& [key, rawConf] : mapofConfs)
                    {
                        std::cout << "Key : " << key << std::endl;
                        std::cout << "  KeyType          : " << rawConf.keyType << std::endl;
                        std::cout << "  max_Parameters   : " << rawConf.max_Parameters << std::endl;
                        std::cout << "  validParametters : ";
                        for (const auto& param : rawConf.validParametters)
                            std::cout << param << " ";
                        std::cout << std::endl;
                    }
                };

                static void printKeyVal(std::pair<std::string, std::vector<std::string> > kvp)
                {
                    std::cout << "Key: " << kvp.first << std::endl;
                    std::cout << "Values: ";
                    for (const auto& value : kvp.second)
                    {
                        std::cout << value << " ";
                    }
                    std::cout << std::endl;
                };

//////////////////////////////////////// END DEBUG SECTION ////////////////////////////////////////
                
                // map of rawConf, key = key name, value = rawConf struct :
                //typedef std::map<std::string, rawConf> data_type;
                static  data_type                      _data;
                static location                        _defaultVals;

                static void  printDefaultVal(location _defaultVals);


                static void                            init_data(void);
                static void                            initDefaultVals(void);
                static void                            print_data();
             
                const static std::string               _whiteSpacesSet;
                const static std::string               _LineBeakSet;
                const static std::string               _CommentSet;
                const static std::string               _ScopeSet;

                static KEYTYPE                        getKeyType(const std::string &key)
                {
                    return (_data.count(key) ? _data[key].keyType : NONE_KEYTYPE);
                }
        }; // END OF CONF STRUCT

    public :

        typedef std::vector<Server>                               data_type;
        typedef std::map<std::string, std::vector<std::string> >  TypeOfMap;
    
    private :

        data_type                                       _data;

/////////////////////////////////// PARSING FUNCTIONS LOGIC : ///////////////////////////////////////

    static void     _listenFormat(key_value_type &key_value, int &start_last_line, std::string &line);
    static void     _checkPort(std::string str, int &start_last_line, std::string &line);
    static void     _checkroot(key_value_type &key_values, int &start_last_line, std::string &line);
    static void     _checkIp(std::vector<std::string> ip, int &start_last_line, std::string &line);
    static void     _checkCgi(key_value_type &key_value, int &start_last_line, std::string &line);
    static void     _checkBodySize(key_value_type &key_value, int &start_last_line, std::string &line);

    std::string     getWord(line_range_type &line_range);

    void            goToNExtWordInFile(line_range_type &line_range, file_range_type &file_range);
    void            skipCharSet(line_range_type &line_range, const std::string &charSet);
    std::string     _getWord_skip_space(line_range_type &line_range);

    bool            _isServerContext(key_value_type key_value, line_range_type &line_range, file_range_type &file_range);
    
    key_value_type  _getKeyValue(line_range_type &line_range);

    bool            _isLocationContext(key_value_type key_value, line_range_type &line_range, file_range_type &file_range, int start_last_line);

    bool            checkDuplicatedParametters(std::vector<std::string> parameters, int start_last_line, std::string &line);

    bool            CheckValidParametters(std::vector<std::string> parameters, std::set<std::string> validParamters, int start_last_line, std::string &line);
    
    void            insertKeyValLocation(location &location, key_value_type key_value, int start_last_line, std::string &line);
    void            checkKeyValues(key_value_type &keyVals, const conf::rawConf &keyConfig, int start_last_line,std::string &line);
    location        NewLocationCreation(line_range_type &line_range, file_range_type &file_range);
    void            insertKeyValServer(Server &server, key_value_type key_value, int start_last_line, std::string &line); 
    Server          NewServerCreation(line_range_type &line_range, file_range_type &file_range);

    void            printServers(const std::vector<Server>& servers);


///////////////////////////////// END PARSING FUNCTIONS LOGIC : /////////////////////////////////////

    public :
    
        configurationSA();
        configurationSA(char *config_file);
        ~configurationSA();

        class ParsingErr : public std::exception
        {
            private :
                std::string word;
                std::string err;
            public :
                ParsingErr(const std::string &err, const std::string &word = std::string()) : word(word), err(err) {};
                
                virtual ~ParsingErr(void) throw() {};
                
                virtual const char* what() const throw()
                {
                    return (std::string("Parsing error : " + word + " " + err).c_str());
                }
                std::string _word(void)
                {
                    return (word);
                }
        };

        // GETTERS AND SETTERS :

        data_type getData(void);

}; // END OF CONFIGURATIONSA
#endif