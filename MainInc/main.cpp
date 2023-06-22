#include "../MainInc/main.hpp"

std::vector<int> Servers::fd_vector;

int main(int argc, char **argv, char **env)
{
    try
    {
        signal(SIGPIPE, SIG_IGN);
        // if no config file use default
        if (argc == 1)
            argv[1] = (char *)"./configs/afaris.conf";
        else if (argc > 2)
        {
            std::cerr << COLOR_RED << "Too many arguments" << COLOR_RESET << std::endl;
            exit(EXIT_FAILURE);
        }
        configurationSA config(argv[1]);
        Webserv webserv(config, env);   // init Webserv
    }
    catch (std::exception &e)
    {
        std::cerr << COLOR_RED << "Failed to init : " << e.what() << COLOR_RESET <<std::endl;
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}