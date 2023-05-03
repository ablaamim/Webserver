
#include "../MainInc/main.hpp"

void Webserv::change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    change_list.push_back(temp_event);
}


void Webserv::disconnect_client(int client_fd, std::map<int, std::string>& clients)
{
    std::cout << "client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}


int Webserv::event_check(struct kevent *event_list, int new_events, std::vector<int> & fds_s, std::vector<struct kevent> change_list)
{
    struct kevent *curr_event;
    Servers::socket_t *socket = new Servers::socket_t;
    int client_socket;
    std::map<int, std::string> clients;
    char buf[1024];

    for (int i = 0; i < new_events; i++)
    {
        curr_event = &event_list[i];   
        if (curr_event->flags & EV_ERROR)
            throw std::runtime_error("EV_ERROR");
        else if (curr_event->filter == EVFILT_READ)
        {
            if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
            {
                if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
                    throw std::runtime_error("accept");
                std::cout << "accept new client: " << client_socket << std::endl;
                change_events(change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
                change_events(change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
                clients[client_socket] = "";
            }
            else if (clients.find(curr_event->ident)!= clients.end())
            {
                int n = read(curr_event->ident, buf, sizeof(buf));
                if (n <= 0)
                {
                    if (n < 0)
                        std::cerr << "client read error!" << std::endl;
                    disconnect_client(curr_event->ident, clients);
                }
                else
                {
                    buf[n] = '\0';
                    clients[curr_event->ident] += buf;
                    std::cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << std::endl;
                }
            }
            else
                    std::cout << "sdasdasd" << std::endl;
        }
        else if (curr_event->filter == EVFILT_WRITE)
        {
            if (clients.find(curr_event->ident) != clients.end())
            {
                if (clients[curr_event->ident] != "")
                {
                    if (write(curr_event->ident, clients[curr_event->ident].c_str(),
                                    clients[curr_event->ident].size()) == -1)
                    {
                        std::cerr << "client write error!" << std::endl;
                        disconnect_client(curr_event->ident, clients);  
                    }
                    else
                        clients[curr_event->ident].clear();
                }
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
    int new_events;
    
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(50) << "Server is running" << " " << this->kq << COLOR_RESET << std::endl;
    while (1337)
    {
        new_events = kevent(this->kq, &change_list[0], change_list.size(), this->event_list, EVENT_LIST, &this->timeout);
        change_list.clear();
        if (new_events == -1)
            perror("kevent");
        else if (!new_events)
            //continue;
            std::cout << COLOR_YELLOW << "timeout" << COLOR_RESET << std::endl;
        else
            event_check(event_list, new_events, fds_socket, change_list);
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
Webserv::~Webserv(){}