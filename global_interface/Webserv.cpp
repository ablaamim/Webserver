
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

int Webserv::event_check(struct kevent *event, int kq_return)
{
    std::cout << COLOR_BLUE << "EVENT CHECK" << COLOR_RESET << std::endl;

    for (int i = 0; i < kq_return; i++)
    {
        //std::cout << COLOR_BLUE << "EVENT COUGHT" << COLOR_RESET << std::endl;
        //std::cout << COLOR_BLUE << "EVENT " << i << COLOR_RESET << std::endl;
        Servers::socket_t *new_socket = reinterpret_cast<Servers::socket_t *>(event[i].udata);
        if (new_socket->success_flag)
        {
            std::cout << COLOR_BLUE << "EVENT COUGHT" << COLOR_RESET << std::endl;
            std::cout << COLOR_BLUE << "EVENT " << i << COLOR_RESET << std::endl;
            Servers::socket_t *socket = new Servers::socket_t;
            int option = 1;
            socklen_t len = sizeof(socket->address);
            socket->socket_fd = accept(event[i].ident, (sockaddr *)&socket->address, &len);
            if (socket->socket_fd == -1)
            {
                std::cerr << "accept error" << std::endl;
                throw std::runtime_error("accept");
            }
            else
            {
                std::cout << COLOR_BLUE << "ACCEPTED" << COLOR_RESET << std::endl;
            }
            if (setsockopt(socket->socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &option, sizeof(option)) == -1)
            {
                std::cerr << "setsockopt error" << std::endl;
                throw std::runtime_error("setsockopt");
            }
            if (fcntl(socket->socket_fd, F_SETFL, O_NONBLOCK) < 0)
            {
                throw std::runtime_error("fcntl");
            }
            struct kevent event;
            EV_SET(&event, socket->socket_fd, EVFILT_READ, EV_ADD, 0, 0,reinterpret_cast<void *>(socket));
            
            
            int kq_return = kevent(this->kq, &event, 1, NULL, 0, NULL);
            if (kq_return == -1)
            {
                std::cout << COLOR_BLUE << "KQ VALUE = " << this->kq << COLOR_RESET << std::endl;

                std::cerr << "->kevent error!!!<-" << std::endl;
                throw std::runtime_error("kevent");
            }
            //char str[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>HELLO</h1></body></html>";
            //write(socket->socket_fd, str, strlen(str));
            
        }
        else if (!new_socket->success_flag)
        {
            std::cout << COLOR_BLUE << "EVENT COUGHT" << COLOR_RESET << std::endl;
            std::cout << COLOR_BLUE << "EVENT " << i << COLOR_RESET << std::endl;
            if (event[i].filter == EVFILT_READ)
            {
                std::cout << "EVFILT_READ" << std::endl;
            }
            else if (event[i].filter == EVFILT_WRITE)
            {
                std::cout << "EVFILT_WRITE" << std::endl;
                //char buffer[BUFFER_SIZE] = {0};
                //int write_return = write(event[i].ident, buffer, BUFFER_SIZE); 
            }
        }
    }

    return (EXIT_FAILURE);
}

void Webserv::run()
{
    
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(50) << "Server is running" << COLOR_RESET << std::endl;

            std::cout<< std::endl << COLOR_BLUE<< "-> KQ VALUE BEFORE LOOP " << this->kq << std::endl;
    while (1337)
    {
        try
        {
            struct kevent event[EVENT_LIST];

            std::cout << std::endl;
            int kq_return = kevent(this->kq, event, 0, 0, EVENT_LIST, &this->timeout);
            
            std::cout<< std::endl << COLOR_BLUE<< "-> KQ VALUE INSIDE LOOP " << this->kq << std::endl;

            if (kq_return == -1)
            {
                std::cout << "KQ VAL AFTER KVENT = " << this->kq << std::endl;
                // C++ 11 feature : throw exception i will replace it with a custom exception class later.
                std::cerr << "kevent error!!! <-" << std::endl;
                throw std::runtime_error("kevent");
            }
            if (kq_return == 0)
            {
                std::cout << COLOR_YELLOW << "timeout" << COLOR_RESET << std::endl;     
                continue;
            }
            std::cout << COLOR_BLUE << "KQ RETURN = " << kq_return << COLOR_RESET << std::endl;
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
    // Parse config file and create a configurationSA object
    configurationSA config(config_file);
    
    // Create a server object with the configurationSA object
    Servers         server(config);
    this->kq = server.kq;
    
    // set timeout value for kevent function
    this->timeout.tv_sec = 1;
    this->timeout.tv_nsec = 0;

    std::cout << std::endl << COLOR_BLUE << "-> KQ VAL IN WEBSERV CONSTRUCTOR = " << this->kq << COLOR_RESET << std::endl << std::endl;
}

// default destructor
Webserv::~Webserv()
{
    //std::cout << "Webserv destructor" << std::endl;
}