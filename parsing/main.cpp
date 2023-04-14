#include "../MainInc/main.hpp"

int main(int argc, char **argv, char **env)
{
    if (argc != 2)
    {
        std::cerr << "Invalid number of arguments : Usage ./Parsing <configuration file>" << std::endl;
        return (EXIT_FAILURE);
    }
    else
    {
        //std::cout << "Parsing config file: " << argv[1] << "..." << std::endl
        configurationSA config(argv[1]);
        
        //config.print_data();
    }
    return (EXIT_SUCCESS);
}