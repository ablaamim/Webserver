
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

int Webserv::event_check(struct kevent *event_list, int new_events, struct kevent *current_event)
{
    //std::cout << COLOR_BLUE << "EVENT CHECK" << COLOR_RESET << std::endl;
    for (int i = 0; i < new_events; i++)
    {
        current_event = &event_list[i];   
        if (current_event->flags & EV_ERROR)
        {
            //std::cerr << "EV_ERROR" << std::endl;
            throw std::runtime_error("EV_ERROR");
        }
        else if (current_event->filter == EVFILT_READ)
        {
            std::cout << COLOR_BLUE << "EVENT COUGHT " << i << COLOR_RESET << std::endl;
            Servers::socket_t *socket = new Servers::socket_t;
            int option = 1;
            socklen_t len = sizeof(socket->address);
            event_list[i].ident = accept(event_list[i].ident, (sockaddr *)&socket->address, &len);
            if (socket->socket_fd == -1)
            {
                std::cerr << "accept error" << std::endl;
                throw std::runtime_error("accept");
            }
            else
            {
                std::cout << COLOR_BLUE << "SOCKET ACCEPTED" << COLOR_RESET << std::endl;
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
            EV_SET(&event, socket->socket_fd, EVFILT_WRITE, EV_ADD, 0, 0,reinterpret_cast<void *>(socket));
            int new_event = kevent(this->kq, &event, 1, NULL, 0, 0);
            if (new_event == -1)
            {
                std::cout << COLOR_BLUE << "KQ VALUE = " << this->kq << COLOR_RESET << std::endl;

                std::cerr << "->kevent error!!!<-" << std::endl;
                throw std::runtime_error("kevent");
            }
            //char str[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>HELLO</h1></body></html>";
            //write(socket->socket_fd, str, strlen(str));
            
        }
        else
            std::cout << COLOR_RED << "EVENT ERROR" << i << " " << event_list[i].filter << COLOR_RESET << std::endl;
    }

    return (EXIT_FAILURE);
}

void Webserv::run()
{
    struct kevent *curr_event;
    int new_events;
    
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(50) << "Server is running" << COLOR_RESET << std::endl;
    while (1337)
    {
        new_events = kevent(this->kq, &change_list[0], change_list.size(), this->event_list, 0, &this->timeout);
        
        this->change_list.clear();

        if (new_events == -1)
            perror("kevent inside infinit loop");
        else if (new_events == 0)
        {
            std::cout << COLOR_YELLOW << "timeout" << COLOR_RESET << std::endl;
            sleep(1);
        }
        else
            event_check(event_list, new_events, curr_event);
    }
}

Webserv::Webserv(char *config_file)
{
    // Parse config file and create a configurationSA object
    configurationSA config(config_file);
    this->timeout.tv_sec = 3;
    this->timeout.tv_nsec = 0;
    // Create a server object with the configurationSA object
    Servers         server(config);
    this->kq = server.kq;
    this->change_list = server.change_list;
    memcpy(this->event_list, server.event_list, sizeof(server.event_list));
    server.print_fd_vector();
    
    // set timeout value for kevent function


    std::cout << std::endl << COLOR_BLUE << "-> KQ VAL IN WEBSERV CONSTRUCTOR = " << this->kq << COLOR_RESET << std::endl << std::endl;
}

// default destructor
Webserv::~Webserv()
{
    //std::cout << "Webserv destructor" << std::endl;
}