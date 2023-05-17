
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

void Webserv::delete_event(int fd, int16_t filter)
{
    struct kevent temp_event;

    EV_SET(&temp_event, fd, filter, EV_DELETE, 0, 0, NULL);
    if (kevent(this->kq, &temp_event, 1, NULL, 0, NULL) == -1)
        throw Webserv::Webserv_err("kevent error");
}

void Webserv::webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s)
{
    int client_socket;
    char buf[BUFFER_SIZE] = {0};
    int n = 0;

    if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
    {
        if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
            throw Webserv::Webserv_err("accept error");
        std::cout << "accept new client: " << client_socket << std::endl;
        change_events(this->change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        change_events(this->change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        this->clients[client_socket] = "";
    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
    {
        n = read(curr_event->ident, buf, BUFFER_SIZE - 1);
        //std::cout << "read " << n << " bytes from " << curr_event->ident << std::endl;
        if (n < 0)
        {
            delete_event(curr_event->ident, EVFILT_READ);
            disconnect_client(curr_event->ident, this->clients);
            return ;
        }
        else if (n == 0)
        {
            delete_event(curr_event->ident, EVFILT_READ);
            disconnect_client(curr_event->ident, this->clients);
            return ;
        }
        while (n > 0)
        {
            buf[n] = '\0';
            this->clients[curr_event->ident] = buf;
        
            std::cout << "received data from " << curr_event->ident << ": " << this->clients[curr_event->ident] << std::endl;
            n = read(curr_event->ident, buf, BUFFER_SIZE - 1);

            int socket_filedes = curr_event->ident;

            Response response(*this, *curr_event);
            response.generate();
            //std::cout << "response generated" << std::endl;
        }
    }
}

void Webserv::webserv_evfilt_write(struct kevent *curr_event)
{
    if (this->clients.find(curr_event->ident) != this->clients.end())
    {
        if (this->clients[curr_event->ident] != "")
        {
            if (write(curr_event->ident, this->clients[curr_event->ident].c_str(), this->clients[curr_event->ident].size()) < 0)
            {
                std::cout << "write error" << std::endl;
                delete_event(curr_event->ident, EVFILT_WRITE);
                disconnect_client(curr_event->ident, this->clients);
                return ;  
            }
            else
                this->clients[curr_event->ident].clear();
        }
    }
}

void Webserv::event_check(struct kevent *event_list, int new_events, std::vector<int> & fds_s)
{
    for (int i = 0; i < new_events; i++)
    {
        if (event_list[i].flags & EV_ERROR)
            throw Webserv::Webserv_err("EV_ERROR");
        else if (event_list[i].filter == EVFILT_READ)
            webserv_evfilt_read(&event_list[i], fds_s);
        else if (event_list[i].filter == EVFILT_WRITE)
            webserv_evfilt_write(&event_list[i]);
        //else
            //std::cout << COLOR_RED << "EVENT ERROR" << i << " " << event_list[i].filter << COLOR_RESET << std::endl;
    }
}

void Webserv::run(std::vector<int> & fds_socket)
{
    int new_events;
    
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(40) << "Server is running" << COLOR_RESET << std::endl;
    while (1337)
    {
        new_events = kevent(this->kq, &this->change_list[0], this->change_list.size(), this->event_list, EVENT_LIST, &this->timeout);
        //std::cout << "KEVENt RETURN = " << new_events << std::endl;
        this->change_list.clear();
        if (new_events == -1)
            throw Webserv::Webserv_err("kevent failed");
        else
            event_check(event_list, new_events, fds_socket);
        //system("leaks Webserv");
    }
}

Webserv::Webserv(char *config_file)
{
    // Parse config file and create a configurationSA object
    configurationSA config(config_file);
    //this->location = configurationSA::location();
    this->timeout.tv_sec = 3;
    this->timeout.tv_nsec = 0;

    // Create a server object with the configurationSA object
    Servers         server(config);
    this->kq = server.kq;
    this->run(Servers::fd_vector);
}

// default destructor
Webserv::~Webserv(){}

Webserv::Webserv(){}


std::map<int, std::string>    &Webserv::get_clients()
{
    return (this->clients);
}


void    Webserv::delete_client(int id)
{
    this->clients[id].clear();
}