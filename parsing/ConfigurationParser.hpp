#ifndef CONFIGURATION_PARSER_HPP
# define CONFIGURATION_PARSER_HPP

# include "../MainInc/main.hpp"
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

typedef std::string::iterator                             line_iterator;
typedef std::vector<std::string>::iterator                file_iterator;
typedef std::pair<line_iterator, file_iterator>           line_range_type;
typedef std::pair<file_iterator, file_iterator>           file_range_type;
typedef std::pair<std::string, std::vector<std::string> > key_value_type;

// Key type typedefs :
typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t;
typedef std::map<std::string, std::vector<std::string> >                         UniqueKey_t;

// Server type typedefs :

typedef std::map<std::string, std::set<std::string> > typeListen; // key = IP address, value = set of ports
typedef std::set<std::string>                         typeServerName_t;

// Configuration file Syntax analysis class
class configurationSA
{
    // I will use two inner structs named 'Location' && 'Server'
    public :
        // Location struct will contain a map of none unique keys and a map of unique keys
        struct location
        {
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
        };
        
        typedef std::map<std::string, location>               typeLocation;
        // Server struct will contain a map of locations and a set of server 
        // names and a map of listen ports.
        struct Server
        {
            typeListen        Listen;
            typeServerName_t  ServerName;
            typeLocation      Location;
            
        };
    private :
        // Configuration struct :
        struct conf
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
                typedef std::map<std::string, rawConf> data_type;
                
                static  data_type                      _data;
                static void                            init_data(void);
                static void                            print_data();
                static location                        _defaultValues;
                const static std::string               _whiteSpacesSet;
                const static std::string               _LineBeakSet;
                const static std::string               _CommentSet;
                const static std::string               _ScopeSet;
                /*
                static KEYTYPE                        getKeyType(const std::string &key)
                {
                    return (_data.count(key) ? _data[key].keyType : NONE_KEYTYPE);
                }
                */
                
        };

    public :

        typedef std::vector<Server>                               data_type;
        typedef std::map<std::string, std::vector<std::string> >  TypeOfMap;
    
    private :

        data_type                                       _data;

        // PARSING FUNCTIONS LOGIC :
        void _listenFormat(key_value_type &key_value, int &start_last_line, std::string &line);

    public :
    
        configurationSA();
        configurationSA(char *config_file);
        ~configurationSA();

        // GETTERS AND SETTERSls
        // const data_type &getData() const;
};
#endif
