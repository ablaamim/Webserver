#include "../MainInc/main.hpp"

int main(int argc, char **argv, char **env)
{
    if (argc != 2)
    {
        std::cout << "Usage: ./Parsing <config_file>" << std::endl;
        return (EXIT_FAILURE);
    }
    else
    {
        std::cout << "Parsing config file: " << argv[1] << "..." << std::endl;
        //const std::string config_file = argv[1];
        configurationSA config(argv[1]);
        //configurationSA::conf::init_data();
           
    }
    return (EXIT_SUCCESS);
}