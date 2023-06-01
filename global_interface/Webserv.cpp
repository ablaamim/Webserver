#include "../MainInc/main.hpp"

std::map<int, int>                  clients_list;                // map of (client_socket, server_socket)
std::map<int, Response>             responsePool;                // map of (client_socket, Response)


void Webserv::print_request()
{
    std::map<int, Request>::iterator it = this->request.begin();
    while (it != this->request.end())
    {
        this->request[it->first].print_params();
        it++;
    }
}

void Webserv::client_cleanup(int client_fd)
{
    this->request[client_fd].reset_request();
    responsePool.erase(client_fd);
    clients_list.erase(client_fd);

    this->clients[client_fd].clear();
    delete_event(client_fd, EVFILT_WRITE, "delete Write event");
    disconnect_client(client_fd, this->clients, "write");
}

void print_responsePool(std::map<int, Response> responsePool)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "ResponsePool list :" << COLOR_RESET << std::endl;
    for (std::map<int, Response>::iterator iter = responsePool.begin(); iter != responsePool.end(); iter++)

        std::cout << COLOR_YELLOW << "[ client_socket : " << iter->first << " , " << " Response client Socket : " << iter->second.clientSocket << " ]" << COLOR_RESET << std::endl;
}

void print_client_list(std::map<int, int> clients_list)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "Client list : " << COLOR_RESET;
    for (std::map<int, int>::iterator iter = clients_list.begin(); iter != clients_list.end(); iter++)
        std::cout << COLOR_YELLOW << "[ client_socket : " << iter->first << " , " << " Server Socket : " << iter->second << " ]" << COLOR_RESET << std::endl;
}

void Webserv::entry_point(struct kevent *curr_event, Request request, configurationSA &config, Servers &server, char **env)
{
    Request req = this->request[curr_event->ident];
    typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t; // map of none unique keys that have more than one value
    std::map<std::string, std::vector<std::string> > newKwargs; // map of none unique keys that have more than one value

    std::map<int, int>::iterator pair_contact = clients_list.find(curr_event->ident);
    configurationSA::Server     _obj_server = Select_server(server.find_ip_by_fd(pair_contact->second), server.find_port_by_fd(pair_contact->second), config.get_data(), "127.0.0.1");
    configurationSA::location   _obj_location = match_location(request.path, _obj_server); 

    Response newResponse(request, curr_event->ident, _obj_location, env);
    
    try
    {
        // allocate memory for kwargs
        
        //newResponse.kwargs_alloc = new std::map<std::string, std::vector<std::string> >;
        
        for (std::map<std::string, std::vector<std::string> >::iterator it = _obj_location.UniqueKey.begin(); it != _obj_location.UniqueKey.end(); it++)
        {
            newResponse.kwargs_alloc->insert(std::make_pair(it->first, it->second));
            newResponse.kwargs.insert(std::make_pair(it->first, it->second));
        }
        
        for (NoneUniqueKey_t::iterator it = _obj_location.NoneUniqueKey.begin(); it != _obj_location.NoneUniqueKey.end(); it++)
        {
            std::string key = it->first;
            std::vector<std::string> values;
            std::map<std::string, std::vector<std::string> > ::iterator it_map = it->second.begin();
            while(it_map != it->second.end())
            {
                values.push_back(it_map->first);
                std::vector<std::string>::iterator vec_iter = it_map->second.begin();
                while (vec_iter != it_map->second.end())
                {
                    values.push_back(*vec_iter);
                    vec_iter++;
                }
                it_map++;
            }
            newResponse.kwargs.insert(std::make_pair(key, values));
            newResponse.kwargs_alloc->insert(std::make_pair(key, values));
        }
        for (std::set<std::string>::iterator it = _obj_server.server_name.begin(); it != _obj_server.server_name.end(); it++)
        {
            newResponse.kwargs_alloc->insert(std::make_pair("server_name", std::vector<std::string> (1, *it)));
            newResponse.kwargs.insert(std::make_pair("server_name", std::vector<std::string> (1, *it)));
        }
        newResponse.init();
        // for (std::map<std::string, std::vector<std::string> >::iterator it = newResponse.kwargs.begin(); it != newResponse.kwargs.end(); it++)
        // {
        //     std::cout << COLOR_YELLOW << "[ " << it->first << " ]" << COLOR_RESET << std::endl;
        //     for (std::vector<std::string>::iterator it_vec = it->second.begin(); it_vec != it->second.end(); it_vec++)
        //         std::cout << COLOR_YELLOW << "[ " << *it_vec << " ]" << COLOR_RESET << std::endl;
        // }
        // for (std::map<std::string, std::vector<std::string> >::iterator it = newResponse.kwargs_alloc->begin(); it != newResponse.kwargs_alloc->end(); it++)
        // {
        //     std::cout << COLOR_YELLOW << "[ " << it->first << " ]" << COLOR_RESET << std::endl;
        //     for (std::vector<std::string>::iterator it_vec = it->second.begin(); it_vec != it->second.end(); it_vec++)
        //         std::cout << COLOR_YELLOW << "[ " << *it_vec << " ]" << COLOR_RESET << std::endl;
        // }
        responsePool.insert(std::make_pair(curr_event->ident, newResponse));
    }
    catch(const std::exception& e)
    {
        client_cleanup(curr_event->ident);
    }
}

configurationSA::Server Webserv::Select_server(std::string ip, std::string port, configurationSA::data_type Servers_vector, std::string hostname)
{
    
    configurationSA::data_type::iterator iter = Servers_vector.end();
    
    for (configurationSA::data_type::iterator it = Servers_vector.begin(); it != Servers_vector.end(); it++)
    {
        if (!it->listen.count(ip) || !it->listen[ip].count(port))
            continue ;
        else if (iter == Servers_vector.end())
            iter = it;
        if (iter == Servers_vector.end())
        {
            std::cout << "ip" << ip << std::endl
            << "port" << port << std::endl
            << "hostname" << hostname << std::endl;
            throw Webserv::Webserv_err("Select_server : no server found");
        }
    }
    return (*iter);
}

configurationSA::location Webserv::match_location(std::string trgt, configurationSA::Server server)
{
    configurationSA::location		result;
    std::vector<std::string>	    splited_trgt = split(trgt, "/");

	for (std::vector<std::string>::reverse_iterator	re_it = splited_trgt.rbegin(); re_it != splited_trgt.rend(); re_it++)
	{
		std::string	current_location;
		for (std::vector<std::string>::iterator it = splited_trgt.begin(); std::reverse_iterator< std::vector<std::string>::iterator >(it) != re_it; it++)
			current_location += "/" + *it;		
	}	
    result.insert(server.location["/"]);
    return (result);
}

void Webserv::change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
{
    struct kevent temp_event;

    EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
    if (kevent(this->kq, &temp_event, 1, NULL, 0, NULL) == -1)
        throw Webserv::Webserv_err("kevent error change event");
}

void Webserv::disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str)
{
    std::cout << COLOR_RED << str << " : client disconnected: " << client_fd << COLOR_RESET << std::endl;
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

void print_env(char **env)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "Env list :" << COLOR_RESET;
    for (int i = 0; env[i]; i++)
        std::cout << COLOR_YELLOW << "[ " << env[i] << " ]" << COLOR_RESET << std::endl;
}

void Webserv::webserv_evfilt_read(struct kevent *curr_event, std::vector<int> &fds_s, configurationSA &config, Servers &server, char **env)
{
    int client_socket;
    char buf[BUFFER_SIZE] = {0};
    int n = 0, k = 120;

    if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
    {
        if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
            throw Webserv::Webserv_err("accept error");
        std::cout << COLOR_YELLOW << "accept new client: " << client_socket << COLOR_RESET << std::endl;
        change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &k, sizeof(int));
        this->clients[client_socket] = "";
        this->request[client_socket].fd_accept = client_socket;
        this->request[client_socket].fd_server = curr_event->ident;
        clients_list.insert(std::make_pair(client_socket, this->request[client_socket].fd_server));     

    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
    {
        n = recv(curr_event->ident, buf, BUFFER_SIZE - 1, 0);
        if (n <= 0)
        {
            disconnect_client(curr_event->ident, this->clients, "read");
            return ;
        }
        buf[n] = '\0';
        this->clients[curr_event->ident].append(buf);
        if (!this->request[curr_event->ident].parse_request(buf))
        {
            //this->request[curr_event->ident].print_params();
            change_events(curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
            entry_point(curr_event, this->request[curr_event->ident], config, server, env);
            delete_event(curr_event->ident, EVFILT_READ, "delete READ event");
        }          
    }
}

void Webserv::webserv_evfilt_write(struct kevent *curr_event)
{
    if (this->clients.find(curr_event->ident) != this->clients.end())
    {
        if (this->clients[curr_event->ident] != "")
        {
            std::map<int, Response>::iterator it = responsePool.find(curr_event->ident);
            try
            {
                if (it->second.isCompleted)
                    client_cleanup(curr_event->ident);
                else
                    it->second.serve();
            }
            catch(const std::exception& e)
            {
                std::cout << COLOR_RED << "Error: " << e.what() << COLOR_RESET << std::endl;
                client_cleanup(curr_event->ident);
            }
        }
    }
}

void Webserv::event_check(int new_events, std::vector<int> &fds_s, configurationSA &config, Servers &server, char **env)
{
    for (int i = 0; i < new_events; i++)
    {
        if (this->event_list[i].flags & EV_ERROR)
            disconnect_client(this->event_list[i].ident, this->clients, "EV_ERROR");
        else if (this->event_list[i].filter == EVFILT_READ)
        {
            webserv_evfilt_read(&this->event_list[i], fds_s, config, server, env);        
        }
        else if (this->event_list[i].filter == EVFILT_WRITE)
            webserv_evfilt_write(&this->event_list[i]);
        else if (this->event_list[i].flags & EV_EOF)
        {
            clients_list.erase(this->event_list[i].ident);
            responsePool.erase(this->event_list[i].ident);
            delete_event(this->event_list[i].ident, EVFILT_READ, "si eof ");
            this->clients[this->event_list[i].ident].clear();
            disconnect_client(this->event_list[i].ident, this->clients, "EV_EOF");
        }
    }
}

void Webserv::run(std::vector<int> &fds_socket, configurationSA &config, Servers &server, char **env)
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
            event_check(new_events, fds_socket, config, server, env);
    }
}

Webserv::Webserv(configurationSA &config, char **env)
{
    Servers         server(config);
    this->kq = server.kq;
    this->log_fd = open("log.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
    this->event_list = new struct kevent [Servers::fd_vector.size()];
    this->run(Servers::fd_vector, config, server, env);
}

Webserv::Webserv()
{
    return ;
}

Webserv::~Webserv()
{   
    delete [] this->event_list;
}