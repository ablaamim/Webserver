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

// Configuration file Syntax analysis class

class configurationSA
{
    // I will use two inner structs named 'Location' && 'Server'
    public :
        // Location struct will contain a map of none unique keys and a map of unique keys
        struct location
        {
            std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey;
            std::map<std::string, std::vector<std::string> >                        UniqueKey;
            
            static void insertUniqueKey(const std::map<std::string, std::vector<std::string> > &a, std::map<std::string, std::vector<std::string> > &b)
            {
                for (std::map<std::string, std::vector<std::string> >::const_iterator it = a.begin(); it != a.end(); it++)
                    b.insert(*it);
            }

            void insert(const location &otherInsert)
            {
                insertUniqueKey(otherInsert.UniqueKey, UniqueKey);
                for (std::map<std::string, std::map<std::string, std::vector<std::string> > > ::const_iterator it = otherInsert.NoneUniqueKey.begin(); it != otherInsert.NoneUniqueKey.end(); it++)
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
        //~configurationSA();

        // GETTERS AND SETTERSls
       // std::vector<Server>         getData() const;
};
#endif
