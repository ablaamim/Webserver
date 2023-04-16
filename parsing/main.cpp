#include "../MainInc/main.hpp"
#include "../Servers/Servers.hpp"

int main(int argc, char **argv, char **env)
{
    if (argc != 2)
    {
        std::cerr << "Invalid number of arguments : Usage ./Parsing <configuration file>" << std::endl;
        return (EXIT_FAILURE);
    }
    try
    {
        //std::cout << "Parsing config file: " << argv[1] << "..." << std::endl
        configurationSA config(argv[1]);
        Servers server(config);
        //config.print_data();
    }
    catch (std::exception &e)
    {
        std::cerr << "Failed to init, error : " << e.what() << std::endl;
        return (EXIT_FAILURE);
    }

    std::cout << "~ B y e ~" << std::endl;
    return (EXIT_SUCCESS);
}