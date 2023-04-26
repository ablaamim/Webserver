#include "Webserv.hpp"
#include "../MainInc/main.hpp"

/*
void Webserv::add_event(int socket_fd, uint16_t filter, Servers::socket_t *socket_info)
{
    struct kevent event;
    EV_SET(&event, socket_fd, filter, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, socket_info);
    if (kevent(this->kq, &event, 1, NULL, 0, NULL) == -1)
    {
        std::cerr << "kevent error" << std::endl;
        throw std::runtime_error("kevent");
    }
}
*/

int Webserv::event_check(struct kevent *event, unsigned int  kq_return)
{
    std::cout << COLOR_BLUE << "EVENT CHECK" << COLOR_RESET << std::endl;

    for (unsigned int i = 0; i < kq_return; i++)
    {
        std::cout << COLOR_BLUE << "EVENT COUGHT" << COLOR_RESET << std::endl;
        std::cout << COLOR_BLUE << "EVENT " << i << COLOR_RESET << std::endl;
        Servers::socket_t *new_socket = reinterpret_cast<Servers::socket_t *>(event[i].udata);
        if (new_socket->success_flag)
        {
            std::cout << COLOR_BLUE << "SUCCESS FLAG" << COLOR_RESET << std::endl;
            
            std::cout << COLOR_BLUE << "ACCEPT CONNECTION" << COLOR_RESET << std::endl;
            
            Servers::socket_t *new_socket = new Servers::socket_t;
            int option = 1;
            sockaddr_in address;
            socklen_t len = sizeof(address);
            new_socket->socket_fd = accept(event[i].ident, (sockaddr *)&address, &len);

            if (new_socket->socket_fd == -1)
            {
                std::cerr << "accept error" << std::endl;
                throw std::runtime_error("accept");
            }
            if (setsockopt(new_socket->socket_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) == -1)
            {
                std::cerr << "setsockopt error" << std::endl;
                throw std::runtime_error("setsockopt");
            }
            if (fcntl(new_socket->socket_fd, F_SETFL, O_NONBLOCK) == -1)
            {
                std::cerr << "fcntl error" << std::endl;
                throw std::runtime_error("fcntl");
            }
        }
    }

    return (EXIT_FAILURE);
}

void Webserv::run()
{
    
    std::cout << std::endl << COLOR_GREEN << "                 Server is running" << COLOR_RESET << std::endl;
    
    // Server loop
    while (1337)
    {
        try
        {
            unsigned int kq_return = kevent(this->kq, NULL, 0, NULL, EVENT_LIST, &this->timeout);
            struct kevent event[EVENT_LIST];
            if (kq_return == -1)
            {
                // C++ 11 feature : throw exception i will replace it with a custom exception class later.
                std::cerr << "kevent error" << std::endl;
                throw std::runtime_error("kevent");
            }
            if (kq_return == 0)
            {
                std::cout << COLOR_YELLOW << "timeout" << COLOR_RESET << std::endl;     
                continue;
            }
            //std::cout << COLOR_BLUE << "KQ RETURN = " << kq_return << COLOR_RESET << std::endl;
            event_check(event, kq_return);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }   
    }
}

Webserv::Webserv(char *config_file)
{
    // set kqueue value using kqueue() function.
    this->kq = kqueue();

    // Parse config file and create a configurationSA object
    configurationSA config(config_file);
    
    // Create a server object with the configurationSA object
    Servers         server(config);
    server.kq = this->kq;

    // set timeout value for kevent function
    this->timeout.tv_sec = 1;
    this->timeout.tv_nsec = 0;

    std::cout << std::endl << COLOR_BLUE << "--------------------> KQ VALUE = " << this->kq << COLOR_RESET << std::endl << std::endl;
}

// default destructor
Webserv::~Webserv()
{
    //std::cout << "Webserv destructor" << std::endl;
}