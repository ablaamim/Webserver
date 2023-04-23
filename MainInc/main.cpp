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
        //server.set_kq(kqueue());

        std::cout << std::endl << COLOR_GREEN << "                 Server is running" << COLOR_RESET << std::endl;
        while (true)
        {
            size_t kq_return = 0;
            
            struct kevent event[10000];
            
            struct timespec timeout = {1, 0};
            
            std::cout << std::endl << COLOR_BLUE<< "KQ VALUE FINAL VAL  = " << server.get_kq() << COLOR_RESET << std::endl;

            kq_return = kevent(server.get_kq(), NULL, 0, event, 10000, &timeout);
            if (kq_return == -1)
            {
                std::cerr << "kevent error" << std::endl;
                throw std::runtime_error("kevent");
            }
            if (kq_return == 0)
            {
                std::cout << COLOR_YELLOW << "timeout" << COLOR_RESET << std::endl;
                continue;
            }
            for (size_t i = 0; i < kq_return; i++)
            {
                std::cout << COLOR_BLUE << "EVENT COUGHT" << COLOR_RESET << std::endl;
                std::cout << COLOR_BLUE << "EVENT " << i << COLOR_RESET << std::endl;
                
                Servers::socket_t *new_socket = new Servers::socket_t;
                int option = 1;
                
                socklen_t len = sizeof(new_socket->address);

                //std::cout << "new socket fd = " << new_socket->socket_fd << std::endl;
                
                new_socket->socket_fd = accept(event[i].ident, (sockaddr *)&new_socket->address, &len);
                
                std::cout <<  COLOR_BLUE << "NEW SOCKET = " << new_socket->socket_fd << COLOR_RESET << std::endl;
                
                if (new_socket->socket_fd == -1)
                {
                    std::cerr << "accept error" << std::endl;
                    throw std::runtime_error("accept");
                }
                
                else 
                {
                    std::cout << COLOR_GREEN << "ACCEPTED" << COLOR_RESET << std::endl;
                }
                
                char buffer[1024];
                
                std::cout << COLOR_YELLOW << " -> NEXT SOCKET = " << event[i].ident << COLOR_RESET << std::endl;    

                //int n = read(event[i].ident, buffer, 1024);
                //if (n < 0)
                //{
                  //  throw std::runtime_error("read");
                //}
                
                //std::cout << "n = " << n << std::endl;
;               char str[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 28\r\n\r\n<h1>HELLO FROM WEBSERV</h1>";

                std::cout << sizeof("<h1>HELLO FROM WEBSERV</h1>") << std::endl;

                //int istr = strlen(str);
                
                int n = write(new_socket->socket_fd, &str, sizeof(str));
                std::cout << "n = " << n << std::endl;
                delete new_socket;                
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
