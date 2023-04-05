#include "parsing.hpp"

int main(int argc, char **argv, char **envp)
{
    const std::string &file = argv[1];
    std::vector<ServerBlock> sb;

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <file>" << std::endl;
        return (EXIT_FAILURE);
    }
    else if (argc < 2)
        std::cout << "Parsing file: " << argv[1] << std::endl;
    try
    {
        ParseConfiguration parse;

        parse.readFile(file);
        parse.getServerBlocks(parse.getlines());
    }
    catch (std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}