#include "../MainInc/main.hpp"
#include "../Servers/Servers.hpp"
#include "../Kqueue/Kqueue.hpp"

int main(int argc, char **argv, char **env)
{
    (void) env;

    if (argc != 2)
    {
        std::cerr << "Invalid number of arguments : Usage ./Parsing <configuration file>" << std::endl;
        return (EXIT_FAILURE);
    }
    try
    {
        configurationSA config(argv[1]);
        Servers         server(config);
        Kqueue          kq(server);
        
        while (1)
        {
            std::cout << std::endl;
            std::cout << COLOR_GREEN << "       Success initialization -> "  << "Server is running" << COLOR_RESET << std::endl;
            std::cout << std::endl;
            sleep(60);
        }
    }
    catch (std::exception &e)
    {
        
        std::cerr << COLOR_RED << "Failed to init " << e.what() << COLOR_RESET <<std::endl;
        return (EXIT_FAILURE);
    }

    std::cout <<  COLOR_GREEN <<"~ B y e ~" << COLOR_RESET << std::endl;

    return (EXIT_SUCCESS);
}