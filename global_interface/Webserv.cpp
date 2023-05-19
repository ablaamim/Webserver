
#include "../MainInc/main.hpp"

std::map<int, int> clients_list;                // map of (client_socket, server_socket)
std::map<int, abstract_req> request_list;       // map of (client_socket, request_socket)
std::map<int, abstract_response> response_list; // map of (client_socket, response_socket)

//////////////////////////////////////////////////////// DEBUG FUNCTIONS /////////////////////////////////////////////////////////

void print_response_list(std::map<int, abstract_response> response_list)
{
    // TO BE CONTINUED
}

void print_client_list(std::map<int, int> clients_list)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << " -> Client list :" << COLOR_RESET << std::endl;
    for (std::map<int, int>::iterator iter = clients_list.begin(); iter != clients_list.end(); iter++)
    {
        std::cout << COLOR_GREEN << "client_socket : " << iter->first << " && " << " server_socket : " << iter->second << COLOR_RESET << std::endl;
    }
}

void print_request_list(std::map<int, abstract_req> request_list)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << " -> Request list :" << COLOR_RESET << std::endl;
    for (std::map<int, abstract_req>::iterator iter = request_list.begin(); iter != request_list.end(); iter++)
    {
        std::cout << COLOR_GREEN << "client_socket : " << iter->first << " && " << " request_socket : " << iter->second._fd << COLOR_RESET << std::endl;
    }
}

//////////////////////////////////////////////////////// END OF DEBUG FUNCTIONS /////////////////////////////////////////////////////////

void Webserv::change_events(uintptr_t ident, int16_t filter,
        uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    if (kevent(this->kq, &temp_event, 1, NULL, 0, NULL) == -1)
        throw Webserv::Webserv_err("kevent error change event");
}

void Webserv::disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str)
{
    std::cout << str << " : client disconnected: " << client_fd << std::endl;
    close(client_fd);
    clients.erase(client_fd);
}

void Webserv::delete_event(int fd, int16_t filter, std::string str)
{
    struct kevent temp_event;

    if (this->clients.find(fd) == this->clients.end())
        return ;
    EV_SET(&temp_event, fd, filter, EV_DELETE, 0, 0, NULL);
    if (kevent(this->kq, &temp_event, 1, NULL, 0, NULL) == -1)
        throw Webserv::Webserv_err(str + "kevent error delete event");
}

void Webserv::webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s)
{
    int client_socket;
    char buf[BUFFER_SIZE] = {0};
    int n = 0 , k = 1;

    if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
    {
        if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
            throw Webserv::Webserv_err("accept error");
        
        this->fd_accepted = client_socket;
        clients_list.insert(std::make_pair(client_socket, curr_event->ident));
        request_list.insert(std::make_pair(client_socket, abstract_req(client_socket)));
        
        std::cout << "accept new client: " << client_socket << std::endl;
        change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &k, sizeof(int));
        this->clients[client_socket] = "";
    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
    {
        n = recv(curr_event->ident, buf, BUFFER_SIZE - 1, 0);
        if (n <= 0)
        {
            std::cout << "read error " << strerror(errno) << std::endl;
            disconnect_client(curr_event->ident, this->clients, "read");
            return ;
        }
        buf[n] = '\0';
        this->clients[curr_event->ident].append(buf);
        std::cout << "received data from " << curr_event->ident << ": " 
             << std::endl << this->clients[curr_event->ident] << std::endl;
        //EV_SET(curr_event, curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
    }

                                        //// DEBUG ////
    
    // print_client_list(clients_list);
    // print_request_list(request_list);

    // //std::cout << "write event" << std::endl;
    // std::map<int, int>::iterator pair_contact = clients_list.find(this->fd_accepted);
    // std::cout << "map pair_contact val = " << pair_contact->second << std::endl;
    // std::map<int, abstract_req>::iterator pair_request = request_list.find(this->fd_accepted);
    // std::cout << "map pair_request val = " << pair_request->second._fd << std::endl;
}

void Webserv::webserv_evfilt_write(struct kevent *curr_event)
{
     std::cout << "write event" << std::endl;
    // std::map<int, int>::iterator pair_contact = clients_list.find(this->fd_accepted);

    // std::cout << "map iterator val = " << pair_contact->second << std::endl;
    if (this->clients.find(curr_event->ident) != this->clients.end())
    {
        delete_event(curr_event->ident, EVFILT_READ, "delete read event");
        if (this->clients[curr_event->ident] != "")
        {
            // std::cout << "received data from " << curr_event->ident << ": " 
            // << std::endl << this->clients[curr_event->ident] << std::endl;
            // if(this->clients[curr_event->ident].size() <= (3786 * (BUFFER_SIZE - 1)))
            //     return ;
            if (send(curr_event->ident, this->clients[curr_event->ident].c_str(), this->clients[curr_event->ident].size(), 0) < 0)
            {
                std::cout << "error " << strerror(errno) << std::endl;
                // std::cout << "write error" << curr_event->ident << std::endl;
                // disconnect_client(curr_event->ident, this->clients, "write");
            }
            else
            {
                std::cout << "writed data to " << curr_event->ident << ": " \
                << std::endl << this->clients[curr_event->ident] << std::endl;
                this->clients[curr_event->ident].clear();
            }
        }
    }
}

void Webserv::event_check(int new_events, std::vector<int> & fds_s)
{
    for (int i = 0; i < new_events; i++)
    {
        if (this->event_list[i].flags & EV_ERROR)
            disconnect_client(this->event_list[i].ident, this->clients, "EV_ERROR");
        else if (this->event_list[i].flags & EV_EOF)
        {
            delete_event(this->event_list[i].ident, EVFILT_READ, "read eof");
            disconnect_client(this->event_list[i].ident, this->clients, "EV_EOF");
        }
        else if (this->event_list[i].filter == EVFILT_READ)
            webserv_evfilt_read(&this->event_list[i], fds_s);
        else if (this->event_list[i].filter == EVFILT_WRITE)
        {
            //std::cout << "write event" << std::endl;
            webserv_evfilt_write(&this->event_list[i]);
        }
        else
            std::cout << COLOR_RED << "EVENT ERROR" << i << " " << this->event_list[i].filter << COLOR_RESET << std::endl;
    }
}

void Webserv::run(std::vector<int> & fds_socket)
{
    int new_events;
    
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(40) << "Server is running size " << fds_socket.size() << COLOR_RESET << std::endl;
    while (1337)
    {
        new_events = kevent(this->kq, NULL, 0, this->event_list, fds_socket.size(), NULL);
        if (new_events == -1)
            throw Webserv::Webserv_err("kevent failed");
        else
            event_check(new_events, fds_socket);
    }
}

Webserv::Webserv(configurationSA &config)
{
    Servers         server(config);
    this->kq = server.kq;
    this->fd_accepted = 0;
    this->event_list = new struct kevent [Servers::fd_vector.size()];
    this->run(Servers::fd_vector);
}

// default destructor
Webserv::~Webserv(){delete [] this->event_list;}