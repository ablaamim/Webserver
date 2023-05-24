#include "../MainInc/main.hpp"

std::vector<int> Servers::fd_vector;

int main(int argc, char **argv)
{
    try
    {
        invalid_argc(argc);
        get_help(argv[1]);
        signal(SIGPIPE, SIG_IGN);
        configurationSA config(argv[1]);
        signal(SIGPIPE, SIG_IGN);  // ignore SIGPIPE
        Webserv webserv(config, argv);   // init Webserv
    }
    catch (std::exception &e)
    {
        std::cerr << COLOR_RED << "Failed to init : " << e.what() << COLOR_RESET <<std::endl;
        return (EXIT_FAILURE);
    }

    std::cout <<  COLOR_GREEN <<"~ B y e ~" << COLOR_RESET << std::endl;

    return (EXIT_SUCCESS);
}