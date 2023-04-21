#include "../MainInc/main.hpp"

int main(int argc, char **argv, char **env)
{
    (void) env;

    if (argc != 2)
    {
        std::cerr << INVALID_ARGS << std::endl;
        return (EXIT_FAILURE);
    }
    if (strcmp(argv[1], "-h") == 0)
    {
        std::cerr << HELP << std::endl;
        return (EXIT_SUCCESS);
    }
    try
    {
        configurationSA config(argv[1]);
        Servers         server(config);

        std::cout << std::endl << COLOR_GREEN << "                 Server is running" << COLOR_RESET << std::endl;
        while (true)
        {
                Kqueue kqueue(server);
                if (kqueue.get_kqueue_return() == 0)
                {
                    std::cerr << "No events" << std::endl;
                }
                else
                {
                    std::cout << "kqueue_return: " << kqueue.get_kqueue_return() << std::endl;
                }
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
