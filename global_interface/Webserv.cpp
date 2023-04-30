
#include "../MainInc/main.hpp"



int Webserv::event_check(struct kevent *event_list, int new_events, std::vector<int> & fds_s)
{
    struct kevent *current_event;
    Servers::socket_t *socket = new Servers::socket_t;
    int client_socket;
    std::map<int, string> clients;

    for (int i = 0; i < new_events; i++)
    {
        current_event = &event_list[i];   
        if (current_event->flags & EV_ERROR)
            throw std::runtime_error("EV_ERROR");
        else if (current_event->filter == EVFILT_READ)
        {
            if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
            {
                if((client_socket =  accept(current_event->ident, NULL, NULL)) < 0)
                    throw std::runtime_error("accept");
                std::cout << "accept new client: " << client_socket << endl;
            }
        }
        else
            std::cout << COLOR_RED << "EVENT ERROR" << i << " " << event_list[i].filter << COLOR_RESET << std::endl;
    }
    delete (socket);
    return (EXIT_FAILURE);
}

void Webserv::run(std::vector<int> & fds_socket)
{
    struct kevent *change_list;
    int new_events;
    
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(50) << "Server is running" << COLOR_RESET << std::endl;
    while (1337)
    {
        new_events = kevent(this->kq, change_list, 0, this->event_list, 0, &this->timeout);
        this->change_list.clear();
        if (new_events == -1)
            perror("kevent inside infinit loop");
        else if (!new_events)
            std::cout << COLOR_YELLOW << "timeout" << COLOR_RESET << std::endl;
        else
            event_check(event_list, new_events, fds_socket);
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

    // set timeout value for kevent function
    std::cout << std::endl << COLOR_BLUE << "-> KQ VAL IN WEBSERV CONSTRUCTOR = " << this->kq << COLOR_RESET << std::endl << std::endl;
    this->run(Servers::fd_vector);
}

// default destructor
Webserv::~Webserv()
{
    //std::cout << "Webserv destructor" << std::endl;
}