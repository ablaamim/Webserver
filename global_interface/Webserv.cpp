#include "../MainInc/main.hpp"

std::map<int, int>                  clients_list;               
std::map<int, Response>             responsePool;              

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
    disconnect_client(client_fd, this->clients, "write");
    delete_event(client_fd, EVFILT_WRITE, "delete Write event");
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

void Webserv::entry_point(struct kevent *curr_event, Request request, configurationSA::location &_obj_location, char **env, Servers &server, configurationSA::Server &_obj_server)
{
    (void) _obj_server;
    Request req = this->request[curr_event->ident];
    typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t; // map of none unique keys that have more than one value
    std::map<std::string, std::vector<std::string> > newKwargs; 
    std::map<int, int>::iterator pair_contact = clients_list.find(curr_event->ident);
    Response newResponse(server.find_ip_by_fd(pair_contact->second), server.find_port_by_fd(pair_contact->second), request, curr_event->ident, _obj_location, env);
    
    try
    {   
        for (std::map<std::string, std::vector<std::string> >::iterator it = _obj_location.UniqueKey.begin(); it != _obj_location.UniqueKey.end(); it++)
        {
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
        }
        newResponse.init();
        responsePool.insert(std::make_pair(curr_event->ident, newResponse));
    }
    catch(const std::exception& e)
    {
        if (newResponse.customErrorFound == false)
        {
            client_cleanup(curr_event->ident);
        }
            
    }
}

configurationSA::Server Webserv::Select_server(std::string ip, std::string port, configurationSA::data_type Servers_vector, std::string hostName)
{
    (void) ip;
    configurationSA::data_type::iterator   first_occurence = Servers_vector.end();

    if (hostName.find(':') != std::string::npos)
        hostName = hostName.substr(0, hostName.find(':'));
   	for (configurationSA::data_type::iterator it = Servers_vector.begin(); it != Servers_vector.end(); it++)
   	{
        if (it->server_name == "")
            it->server_name = "localhost";
		if (it->server_name == hostName)
           return (*it);
        if (it->listen.begin()->first == port)
            first_occurence = it;
		else if (first_occurence == Servers_vector.end())
        {
			first_occurence = it;
        }
   	}
   	return (*first_occurence);
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
    (void) str;
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

void Webserv::start_reading_from_client(struct kevent *curr_event,configurationSA &config, Servers &server, char **env)
{
    char buf[BUFFER_SIZE] = {0};
    int n ,k;
    std::map<int, int>::iterator pair_contact;
    configurationSA::Server     _obj_server;
    configurationSA::location   _obj_location ;

    
    n = recv(curr_event->ident, buf, BUFFER_SIZE - 1, 0);
    if (n <= 0)
    {
        disconnect_client(curr_event->ident, this->clients, "read error ");
        this->request[curr_event->ident].reset_request();
        return ;
    }
    buf[n] = '\0';
    this->clients[curr_event->ident] = buf;
    k = this->request[curr_event->ident].parse_request(std::string(buf,n));
    if (this->request[curr_event->ident].headers_done)
    {
        pair_contact = clients_list.find(curr_event->ident);
        _obj_server = Select_server(server.find_ip_by_fd(pair_contact->second), server.find_port_by_fd(pair_contact->second), \
        config.get_data(), this->request[curr_event->ident].params["Host"]);
        _obj_location = match_location(this->request[curr_event->ident].path, _obj_server); 
       check_before_get_chuncked_messages(_obj_location , this->request[curr_event->ident]);
    }
    if (k == _PARSE_REQUEST_DONE || this->request[curr_event->ident].error)
    {
        change_events(curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        delete_event(curr_event->ident, EVFILT_READ, "delete READ event");
        entry_point(curr_event, this->request[curr_event->ident], _obj_location, env,server, _obj_server);
    }   
}


void Webserv::webserv_evfilt_read(struct kevent *curr_event, std::vector<int> &fds_s, configurationSA &config, Servers &server, char **env)
{
    int client_socket;
    int k = 0;

    (void)config; (void)server; (void)env;
    if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
    {
        if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
            throw Webserv::Webserv_err("accept error");
        change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &k, sizeof(int));
        this->clients[client_socket] = "";
        this->request[client_socket].fd_server = curr_event->ident;
        clients_list.insert(std::make_pair(client_socket, this->request[client_socket].fd_server));    

    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
        start_reading_from_client(curr_event, config, server, env);
        
}

void    Webserv::fill_request_err(std::string err, std::string msg, Request & request)
{
    request.error = std::stoi(err);
    request.erro_msg = msg;
}


void    Webserv::check_methods(configurationSA::location &_obj_location, Request & request)
{
    std::vector<std::string> allowedMethods = _obj_location.UniqueKey["allowed_methods"];
    
    if (std::find(allowedMethods.begin(), allowedMethods.end(), request.method) == allowedMethods.end())
        fill_request_err(_CS_405, _CS_405_m, request);
    else if (request.method != GET && request.method != POST && request.method != DELETE)
        fill_request_err(_CS_501, _CS_501_m, request);
    if (request.params.find("Host") == request.params.end())
        fill_request_err(_CS_400, _CS_400_m, request);
}

void    Webserv::checkHTTP(Request & request)
{
    if (request.version.substr(0, 5) != "HTTP/")
        fill_request_err(_CS_400, _CS_400_m, request);
    if (request.version.substr(5) != "1.1")
        fill_request_err(_CS_505, _CS_505_m, request);
}

void    Webserv::check_Transfer_Encoding(Request & request)
{
    it_param transfer = request.params.find("Transfer-Encoding");
    it_param content = request.params.find("Content-Length");

    if (transfer != request.params.end() && transfer->second != "chunked")
        fill_request_err(_CS_501, _CS_501_m, request);
    if (transfer == request.params.end() && content == request.params.end() && request.method == POST)
        fill_request_err(_CS_400, _CS_400_m, request);
}

void    Webserv::check_Content_Length(Request & request, configurationSA::location &_obj_location)
{
    it_param content = request.params.find("Content-Length");

    if (request.method != POST)
        return;
    if (content == request.params.end())
        fill_request_err(_CS_411, _CS_411_m, request);
    else if (std::atof(content->second.c_str()) > std::atof(_obj_location.UniqueKey["max_body_size"][0].c_str()))
        fill_request_err(_CS_413, _CS_413_m, request);
}

void    Webserv::check_uri_allowed_characters(Request &request)
{
    std::string allowed_characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=";
    std::string::iterator it = request.path.begin();

    while (it != request.path.end())
    {
        if (allowed_characters.find(*it) == std::string::npos)
        {
            fill_request_err(_CS_400, _CS_400_m, request);
            return;
        }
        it++;
    }
}

void    Webserv::check_before_get_chuncked_messages(configurationSA::location &_obj_location, Request & request)
{
    check_methods(_obj_location, request);
    checkHTTP(request);
    check_Transfer_Encoding(request);
    check_Content_Length(request, _obj_location);
    check_uri_allowed_characters(request);
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
                {
                    client_cleanup(curr_event->ident);
                }
                else
                    it->second.serve();
            }
            catch(const std::exception& e)
            {
                if (it->second.customErrorFound == false)
                {
                    client_cleanup(curr_event->ident);
                }
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
            clients_list.erase(this->event_list[i].ident);
            this->request[this->event_list[i].ident].reset_request();
            responsePool.erase(this->event_list[i].ident);
            disconnect_client(this->event_list[i].ident, this->clients, "EV_EOF");
            delete_event(this->event_list[i].ident, EVFILT_READ, "si eof ");
        }
        else if (this->event_list[i].filter == EVFILT_READ)
            webserv_evfilt_read(&this->event_list[i], fds_s, config, server, env);
        else if (this->event_list[i].filter == EVFILT_WRITE)
            webserv_evfilt_write(&this->event_list[i]);
        else
            std::cout << "event not known" << std::endl;
    }
}

void Webserv::run(std::vector<int> &fds_socket, configurationSA &config, Servers &server, char **env)
{
    int new_events;
    
    Response::initMimeTypes();
    std::cout << std::endl << COLOR_GREEN << std::setfill(' ') << 
    std::setw(40) << "Server is running\n" << COLOR_RESET << std::endl;
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
    this->event_list = new struct kevent [Servers::fd_vector.size()];
    this->run(Servers::fd_vector, config, server, env);
}

Webserv::Webserv(){}

Webserv::~Webserv(){delete [] this->event_list;}