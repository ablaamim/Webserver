#include "parsing.hpp"

ServerBlock::ServerBlock()
{
    //std::cout << "----> Constructor called." << std::endl;
}

ServerBlock::ServerBlock(std::vector<std::string> lines)
{
    //std::cout << "----> Constructor called." << std::endl;
    //printVector2(lines);
}

ServerBlock::~ServerBlock()
{
    //std::cout << "----> Destructor called." << std::endl;
}

const std::string &ServerBlock::getListen() const
{
    return (listen);
}

const std::string &ServerBlock::getServerName() const
{
    return (server_name);
}

const std::string &ServerBlock::getAutoindex() const
{
    return (autoindex);
}

const std::string &ServerBlock::getIndex() const
{
    return (index);
}

const std::string &ServerBlock::getRoot() const
{
    return (root);
}

const std::string &ServerBlock::getUploadPath() const
{
    return (upload_path);
}

int ServerBlock::getClientMaxBodySize() const
{
    return (client_max_body_size);
}

void printVector2(std::vector<std::string> lines)
{
    std::cout << "----> Printing vector..." << std::endl;
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
    {
        std::cout << *it << std::endl;
    }
    std::cout << "----> Printing done." << std::endl;
}

std::vector<ServerBlock> ParseConfiguration::getServerBlocks(const std::vector<std::string> &file)
{
    std::vector<ServerBlock> serverblocks;

    // Loop through the raw configuration
    std::cout << "----> Serverblocks" << std::endl << std::endl;
    int startIndex = 0; 

    while (startIndex < lines.size())
    {
        std::vector<std::string> serverblock;
        int endIndex = startIndex;
        while (endIndex < lines.size())
        {
            serverblock.push_back(lines[endIndex]);
            endIndex++;
        }
        serverblocks.push_back(ServerBlock(serverblock));
        printVector2(serverblock);
        startIndex = endIndex + 1;
    }
    return (serverblocks);
}