
#include "../MainInc/main.hpp"

int Webserv::event_check(struct kevent *event, int kq_return)
{
    std::cout << COLOR_BLUE << "EVENT CHECK" << COLOR_RESET << std::endl;

    for (int i = 0; i < kq_return; i++)
    {
        if (reinterpret_cast<Servers::socket_t *>(event[i].udata)->success_flag)
        {
            std::cout << COLOR_BLUE << "EVENT COUGHT " << i << COLOR_RESET << std::endl;
            Servers::socket_t *socket = new Servers::socket_t;
            int option = 1;
            socklen_t len = sizeof(socket->address);
            socket->socket_fd = accept(event[i].ident, (sockaddr *)&socket->address, &len);
            if (socket->socket_fd == -1)
            {
                std::cerr << "accept error" << std::endl;
                throw Webserv_err("accept");
            }
            else
            {
                std::cout << COLOR_BLUE << "ACCEPTED" << COLOR_RESET << std::endl;
            }
            if (setsockopt(socket->socket_fd, SOL_SOCKET, SO_NOSIGPIPE, &option, sizeof(option)) == -1)
            {
                std::cerr << "setsockopt error" << std::endl;
                throw Webserv_err("setsockopt");
            }
            if (fcntl(socket->socket_fd, F_SETFL, O_NONBLOCK) < 0)
            {
                throw Webserv_err("fnctl error");
            }
            //char str[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>HELLO</h1></body></html>";
            //write(socket->socket_fd, str, strlen(str));
            
        }
        else
            std::cout << COLOR_RED << "EVENT ERROR" << i << " " << event[i].filter << COLOR_RESET << std::endl;
    }

    return (EXIT_FAILURE);
}

void Webserv::run()
{  
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(40) << "Server is running" << COLOR_RESET << std::endl;
    while (1337)
    {
        //system("leaks Webserv");
        std::cout << std::endl << COLOR_BLUE << std::setw(40) << "MULTIPLEXING NEXT IS NEXT STEP" << COLOR_RESET << std::endl;
        sleep(1);
    }
}

Webserv::Webserv(char *config_file)
{
    // Parse config file and create a configurationSA object
    configurationSA config(config_file);
    
    this->timeout.tv_sec = 1;
    this->timeout.tv_nsec = 0;

    // Create a server object with the configurationSA object
    Servers         server(config);
    this->kq = server.kq;

    //std::cout << std::endl << COLOR_BLUE << "-> KQ VAL IN WEBSERV CONSTRUCTOR = " << this->kq << COLOR_RESET << std::endl << std::endl;
}

// default destructor
Webserv::~Webserv()
{
    //std::cout << "Webserv destructor" << std::endl;
}