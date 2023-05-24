
#include "../MainInc/main.hpp"

std::map<int, int>               clients_list;                // map of (client_socket, server_socket)
std::map<int, abstract_req>      request_list;                // map of (client_socket, request_socket)
std::map<int, Response>          responsePool;                // mapp of (client_socket, Response)

//////////////////////////////////////////////////////// DEBUG FUNCTIONS /////////////////////////////////////////////////////////

void print_responsePool(std::map<int, Response> responsePool)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "ResponsePool list :" << COLOR_RESET << std::endl;
    for (std::map<int, Response>::iterator iter = responsePool.begin(); iter != responsePool.end(); iter++)
    {
        std::cout << COLOR_YELLOW << "[ client_socket : " << iter->first << " , " << " Response client socket : " << iter->second.clientSocket << " ]" << COLOR_RESET << std::endl;
    }
}

void print_client_list(std::map<int, int> clients_list)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "Client list : " << COLOR_RESET;
    for (std::map<int, int>::iterator iter = clients_list.begin(); iter != clients_list.end(); iter++)
    {
        std::cout << COLOR_YELLOW << "[ client_socket : " << iter->first << " , " << " server_socket : " << iter->second << " ]" << COLOR_RESET << std::endl;
    }
}

void print_request_list(std::map<int, abstract_req> request_list)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "Request list :" << COLOR_RESET;
    for (std::map<int, abstract_req>::iterator iter = request_list.begin(); iter != request_list.end(); iter++)
    {
        std::cout << COLOR_YELLOW << "[ client_socket : " << iter->first << " , " << " Request client socket : " << iter->second._fd << " ]" << COLOR_RESET << std::endl;
    }
}

//////////////////////////////////////////////////////// END OF DEBUG FUNCTIONS /////////////////////////////////////////////////////////

configurationSA::Server Select_server(configurationSA &config, std::string ip, std::string port, configurationSA::data_type Servers_vector, std::string hostname)
{
    //std::cout << COLOR_GREEN <<  "                 -> Select_server <-          " << std::endl << std::endl;
    
    configurationSA::data_type::iterator iter = Servers_vector.end();
    
    for (configurationSA::data_type::iterator it = Servers_vector.begin(); it != Servers_vector.end(); it++)
    {
        //std::cout << it->listen.count(ip) << std::endl;
        
        //std::cout << it->listen[ip].count(port) << std::endl;

        if (!it->listen.count(ip) || !it->listen[ip].count(port))
            continue ;
        
        //if (it->server_name.count(hostname))
            //return (*it);
        
        else if (iter == Servers_vector.end())
            iter = it;
        
        if (iter == Servers_vector.end())
        {
            std::cout << "ip" << ip << std::endl
            << "port" << port << std::endl
            << "hostname" << hostname << std::endl;
            throw Webserv::Webserv_err("Select_server : no server found");
        }

        //config.print_data_type();
    }
    return (*iter);
}

configurationSA::location match_location(std::string trgt, configurationSA::Server server)
{
    //std::cout << COLOR_GREEN <<  "                 -> match location <-          " << COLOR_RESET << std::endl << std::endl;
    
    configurationSA::location		result;

    std::vector<std::string>	splited_trgt = split(trgt, "/");

	for (std::vector<std::string>::reverse_iterator	re_it = splited_trgt.rbegin(); re_it != splited_trgt.rend(); re_it++)
	{
		std::string	current_location;
		for (std::vector<std::string>::iterator it = splited_trgt.begin(); std::reverse_iterator< std::vector<std::string>::iterator >(it) != re_it; it++)
			current_location += "/" + *it;
		
        if (server.location.count(current_location))
		{
			result.UniqueKey.insert(std::make_pair("root_to_delete", std::vector<std::string> (1, current_location)));
			result.insert(server.location[current_location]);
		}
	}
	
    result.UniqueKey.insert(std::make_pair("root_to_delete", std::vector<std::string> (1, "")));
	
    result.insert(server.location["/"]);

    // std::cout << std::endl << std::endl << COLOR_BLUE << "-----> Result list :" << COLOR_RESET << std::endl;
    // result.print_none_unique_key();
    // result.print_unique_key();

    // std::cout << std::endl << std::endl << COLOR_BLUE << "-----> END" << COLOR_RESET << std::endl;
	
    return (result);
}

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

void print_env(char **env)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "Env list :" << COLOR_RESET;
    for (int i = 0; env[i]; i++)
    {
        std::cout << COLOR_YELLOW << "[ " << env[i] << " ]" << COLOR_RESET << std::endl;
    }
}

void Webserv::webserv_evfilt_read(struct kevent *curr_event, std::vector<int> &fds_s, configurationSA &config, Servers &server, char **env)
{
    int client_socket;
    char buf[BUFFER_SIZE] = {0};
    int n = 0, k = 1;

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
    //print_env(env);
                                        //// DISCOMMENT THIS TO SEE THE CURRENT DATA ////
    
    //print_client_list(clients_list);
    //print_request_list(request_list);

    // std::endl(std::cout);

    try
    {
        // // //std::cout << "write event" << std::endl;
        std::map<int, int>::iterator pair_contact = clients_list.find(this->fd_accepted);
        //// // //std::cout << "map pair_contact val = " << pair_contact->second << std::endl;
        //
        //// // //std::cout << "ACCEPTED FD = " <<this->fd_accepted << std::endl;
        //
        std::map<int, abstract_req>::iterator pair_request = request_list.find(this->fd_accepted);
        //// // //std::cout << "map pair_request val = " << pair_request->second._fd << std::endl;
//
        configurationSA::Server     _obj_server = Select_server(config, server.find_ip_by_fd(pair_contact->second), server.find_port_by_fd(pair_contact->second), config.get_data(), "127.0.0.1");
        //
        //// // _obj_server.print_type_listen();
//
        //// // std::endl(std::cout);
//
        //// // _obj_server.print_server_name();
//
        //// // std::endl(std::cout);
        //
        
        configurationSA::location   _obj_location = match_location(pair_request->second.getResourceFullPath(), _obj_server);
        
        // if (_obj_location.error_if_empty_keys())
        // {
        //     std::cerr << "Empty location" << std::endl;
        //     disconnect_client(curr_event->ident, this->clients, "read");
        //     return ;
        // }
        
        //_obj_location.print_unique_key();
        
        //_obj_location.print_none_unique_key();
        
        // response_list.insert(std::make_pair(this->fd_accepted, abstract_response(pair_request->second, _obj_location, server.find_ip_by_fd(pair_contact->second), env)));
        // //response_list[this->fd_accepted].construct_data();
        // //print_response_list(response_list);
        // check if ResponsePool has this fd in it already
        
        // PROTOTYPE : Response(abstract_req req, int id, configurationSA::location location, std::string _client_ip, char **env) : _req(req), _location(location), _client_ip(_client_ip) , _env(env)

        // insert Unique key in kwargs  
        std::map<int, Response>::iterator it = responsePool.find(this->fd_accepted);
        if (it != responsePool.end())
        {
            while(!it->second.isCompleted)
            {
                it->second.serve();
            }
            if (it->second.isCompleted)
            {
                std::cout << COLOR_GREEN <<" >>>>>>>>>>> Response is Served Successfully!! :D <<<<<<<<<<<<" << std::endl << std::endl << COLOR_RESET;
                disconnect_client(curr_event->ident, this->clients, "read");
                responsePool.erase(it);
            }
        }
        else
        {
            std::cout << COLOR_GREEN <<" >>>>>>>>>>> New Response Is Being Created <<<<<<<<<<<<" << std::endl << std::endl << COLOR_RESET;
            
            // Better use this constructor here !
            Response newResponse(pair_request->second, this->fd_accepted, _obj_location, server.find_ip_by_fd(pair_contact->second), env);
            for (std::map<std::string, std::vector<std::string> >::iterator it = _obj_location.UniqueKey.begin(); it != _obj_location.UniqueKey.end(); it++)
            {
                newResponse.kwargs.insert(std::make_pair(it->first, it->second));
            }

            // Insert NoneUniqueKey key in kwargs map

            typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t; // map of none unique keys that have more than one value
            typedef std::map<std::string, std::vector<std::string> > NoneUniqueKe_t; // map of none unique keys that have more than one value

            for (NoneUniqueKey_t::iterator it = _obj_location.NoneUniqueKey.begin(); it != _obj_location.NoneUniqueKey.end(); it++)
            {
                for (std::map<std::string, std::vector<std::string> >::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
                {
                    newResponse.kwargs.insert(std::make_pair(it2->first, it2->second));
                }
            }

            // Insert Server_name in kwargs map

            for (std::set<std::string>::iterator it = _obj_server.server_name.begin(); it != _obj_server.server_name.end(); it++)
            {
                newResponse.kwargs.insert(std::make_pair("server_name", std::vector<std::string> (1, *it)));
            }
            responsePool.insert(std::make_pair(this->fd_accepted, newResponse));
            std::cout << "fd responsePool: " << responsePool[this->fd_accepted].httpVersion << std::endl;
        }
        print_responsePool(responsePool);
    }
    catch(const std::exception& e)
    {
        std::cout << strerror(errno);
        std::cerr << e.what() << '\n';
    }
}

void Webserv::webserv_evfilt_write(struct kevent *curr_event, configurationSA &config, Servers &server, char **env)
{
                                            //// DEBUG ////
    
    // print_client_list(clients_list);
    // print_request_list(request_list);

    // //std::cout << "write event" << std::endl;
    // std::map<int, int>::iterator pair_contact = clients_list.find(this->fd_accepted);
    // std::cout << "map pair_contact val = " << pair_contact->second << std::endl;
    // std::map<int, abstract_req>::iterator pair_request = request_list.find(this->fd_accepted);
    // std::cout << "map pair_request val = " << pair_request->second._fd << std::endl;
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

void Webserv::event_check(int new_events, std::vector<int> &fds_s, configurationSA &config, Servers &server, char **env)
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
            webserv_evfilt_read(&this->event_list[i], fds_s, config, server, env);
        else if (this->event_list[i].filter == EVFILT_WRITE)
        {
            //std::cout << "write event" << std::endl;
            webserv_evfilt_write(&this->event_list[i], config, server, env);
        }
        else
            std::cout << COLOR_RED << "EVENT ERROR" << i << " " << this->event_list[i].filter << COLOR_RESET << std::endl;
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
    this->fd_accepted = 0;
    this->event_list = new struct kevent [Servers::fd_vector.size()];
    this->run(Servers::fd_vector, config, server, env);
}

// default destructor
Webserv::~Webserv(){delete [] this->event_list;}