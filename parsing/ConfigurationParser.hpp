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

// ALL TYPEDEFS NEEDED TO MAKE THE CODE MORE READABLE :

typedef std::string::iterator                             line_iterator;
typedef std::vector<std::string>::iterator                file_iterator;
typedef std::pair<line_iterator, file_iterator>           line_range_type;
typedef std::pair<std::string, std::vector<std::string> > key_value_type;

typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t;
typedef std::map<std::string, std::vector<std::string> >                         UniqueKey_t;


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
        // Server struct will contain a map of locations and a set of server names and a map of listen ports
        struct Server
        {
            std::map<std::string, std::set<std::string> > listen;
            std::set<std::string>                         server_name;
            std::map<std::string, location>               location;
        };
        struct conf
        {
            enum KEYTYPE
            {
                NONE_KEYTYPE,
                UNIQUE_KEYTYPE,
                NONE_UNIQUE_KEYTYPE,
                SERVER_KEYTYPE
            };
            struct rawConf
            {
                KEYTYPE keyType;
            };
            //rawCong(KEYTPE key);
            std::map<std::string, rawConf> data;
            //static void                           init_data(void);
        };

    private :
        static std::vector<Server>                        _data;
        std::map<std::string, std::vector<std::string> >  TypeOfMap;

        // PARSING METHODS
    

    public :
        std::vector<std::string>    server;
        configurationSA();
        configurationSA(char *config_file);
        ~configurationSA();

        // GETTERS AND SETTERSls
       // std::vector<Server>         getData() const;
};
#endif
