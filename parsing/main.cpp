#include "parsing.hpp"

void printVector(std::vector<std::string> vec)
{
    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
        std::cout << *it << std::endl;
}

void printMap(std::map<std::string, std::string> map)
{
    for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        return (EXIT_FAILURE);
    }
    else
    {
        ConfigFile file(argv[1]);
        printVector(file.getLines());
        file.parseServers();
        printMap(file.getServers());
        return (EXIT_SUCCESS);
    }
}