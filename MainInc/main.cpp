#include "../MainInc/main.hpp"

std::vector<int> Servers::fd_vector;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << INVALID_ARGS << std::endl;
        return (EXIT_FAILURE);
    }
    if (!strcmp(argv[1], "-h"))
    {
        std::cerr << HELP << std::endl;
        return (EXIT_SUCCESS);
    }
    try
    {
        signal(SIGPIPE, SIG_IGN);
        Webserv webserv(argv[1]);
    }
    catch (std::exception &e)
    {
        std::cerr << COLOR_RED << "Failed to init : " << e.what() << COLOR_RESET <<std::endl;
        return (EXIT_FAILURE);
    }

    std::cout <<  COLOR_GREEN <<"~ B y e ~" << COLOR_RESET << std::endl;

    return (EXIT_SUCCESS);
}
