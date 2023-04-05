#ifndef PARSING_HPP
#define PARSING_HPP

# include <vector>
# include <map>
# include <set>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <algorithm>
# include <iterator>

class ServerBlock
{
    public :
        ServerBlock();
        ServerBlock(std::vector<std::string> lines);
        ~ServerBlock();
        const std::string &getListen() const;
        const std::string &getServerName() const;
        const std::string &getAutoindex() const;
        const std::string &getIndex() const;
        const std::string &getRoot() const;
        const std::string &getUploadPath() const;
        int               getClientMaxBodySize() const;

    private :
        std::string listen;
        std::string server_name;
        std::string autoindex;
        std::string index;
        std::string root;
        std::string upload_path;
        int         client_max_body_size;
};

class ParseConfiguration
{
    private :
        std::vector<std::string> lines;
        std::vector<ServerBlock> server_blocks;
    public :
        ParseConfiguration(); // Constructor
        ~ParseConfiguration(); // Destructor

        std::vector<std::string> getlines() const;
        void                     readFile(const std::string &file);
        
        std::vector<ServerBlock> getServerBlocks(const std::vector<std::string>& lines);
};

#endif