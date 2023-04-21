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
            size_t kq_return = 0;
            
            struct kevent event[10000];
            
            struct timespec timeout = {1, 0};
            
            //std::cout << "KQ VALUE = " << server.get_kq() << std::endl;

            kq_return = kevent(server.get_kq(), NULL, 0, event, 10000, &timeout);
            if (kq_return == -1)
            {
                std::cerr << "kevent error" << std::endl;
                throw std::runtime_error("kevent");
            }
            if (kq_return == 0)
            {
                std::cout << "timeout" << std::endl;
                continue;
            }
            for (size_t i = 0; i < kq_return; i++)
            {
                std::cout << "event " << i << std::endl;
            }
            //system("leaks webserv");
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
