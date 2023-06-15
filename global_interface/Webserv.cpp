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
    std::cout << COLOR_RED << "Client " << client_fd << " is being cleanup " << COLOR_RESET << std::endl;
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
    Request req = this->request[curr_event->ident];
    typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t; // map of none unique keys that have more than one value
    std::map<std::string, std::vector<std::string> > newKwargs; // map of none unique keys that have more than one value
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
        for (std::set<std::string>::iterator it = _obj_server.server_name.begin(); it != _obj_server.server_name.end(); it++)
        {
            newResponse.kwargs.insert(std::make_pair("server_name", std::vector<std::string> (1, *it)));
        }
        newResponse.init();
        responsePool.insert(std::make_pair(curr_event->ident, newResponse));
    }
    catch(const std::exception& e)
    {
        if (newResponse.customErrorFound == false)
        {
            std::cout << COLOR_RED << "Error : " << e.what() << COLOR_RESET << std::endl;
            client_cleanup(curr_event->ident);
        }
            
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
    std::map<int, int>::iterator pair_contact;
    configurationSA::Server     _obj_server;
    configurationSA::location   _obj_location ;

    (void)config; (void)server; (void)env;
    if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
    {
        if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
            throw Webserv::Webserv_err("accept error");
        std::cout << COLOR_YELLOW << "Client Id : " << client_socket << COLOR_RESET << std::endl;
        change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &k, sizeof(int));
        this->clients[client_socket] = "";
        this->request[client_socket].fd_server = curr_event->ident;
        clients_list.insert(std::make_pair(client_socket, this->request[client_socket].fd_server));    

    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
    {
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
            _obj_server = Select_server(server.find_ip_by_fd(pair_contact->second), server.find_port_by_fd(pair_contact->second), config.get_data(), "127.0.0.1");
            _obj_location = match_location(this->request[curr_event->ident].path, _obj_server); 
            check_before_get_chuncked_messages(_obj_location , this->request[curr_event->ident]);
        }
        if (k == _PARSE_REQUEST_DONE || this->request[curr_event->ident].error)
        {
            //this->request[curr_event->ident].print_params();
            change_events(curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
            delete_event(curr_event->ident, EVFILT_READ, "delete READ event");
            entry_point(curr_event, this->request[curr_event->ident], _obj_location, env,server, _obj_server);
            std::cout << COLOR_RED << "END of read" << COLOR_RESET <<std::endl;
        }          
    }
}

void    Webserv::check_methods(configurationSA::location &_obj_location, Request & request)
{
    std::vector<std::string> allowedMethods = _obj_location.UniqueKey["allowed_methods"];
    
    if (request.method != GET && request.method != POST && request.method != "DELETE")
    {
        request.erro_msg = _CS_501_m;
        request.error = std::stoi(_CS_501);
    }
    if (std::find(allowedMethods.begin(), allowedMethods.end(), request.method) == allowedMethods.end())
    {
        request.erro_msg = _CS_405_m;
        request.error = std::stoi(_CS_405);
    }
    if (request.params.find("Host") == request.params.end())
    {
        request.erro_msg = _CS_400_m;
        request.error = std::stoi(_CS_400);
    }
}

void    Webserv::checkHTTP(Request & request)
{
    if (request.version.substr(0, 5) != "HTTP/")
    {
        request.erro_msg = _CS_505_m;
        request.error = std::stoi(_CS_505);
    }
    if (request.version.substr(5) != "1.1")
    {
        request.erro_msg = _CS_400_m;
        request.error = std::stoi(_CS_400);
    }
}

void    Webserv::check_Transfer_Encoding(Request & request)
{
    it_param transfer = request.params.find("Transfer-Encoding");

    if (transfer != request.params.end() && transfer->second != "chunked")
    {
        request.erro_msg = _CS_501_m;
        request.error = std::stoi(_CS_501);
    }
}

void    Webserv::check_Content_Length(Request & request, configurationSA::location &_obj_location)
{
    it_param content = request.params.find("Content-Length");

    if (request.method != POST)
    {
        if (content != request.params.end())
        {
            request.erro_msg = _CS_400_m;
            request.error = std::stoi(_CS_400);
        }
        return;
    }
    if (content == request.params.end())
    {
        request.erro_msg = _CS_411_m;
        request.error = std::stoi(_CS_411);
    }
    else if (std::atof(content->second.c_str()) > std::atof(_obj_location.UniqueKey["max_body_size"][0].c_str()))
    {
        request.erro_msg = _CS_413_m;
        request.error =  std::stoi(_CS_413);
    }
}

void check_uri_length(Request &request)
{
    if (request.path.length() > 2048)
    {
        request.erro_msg = _CS_414_m;
        request.error = std::stoi(_CS_414);
    }
}

void    Webserv::check_before_get_chuncked_messages(configurationSA::location &_obj_location, Request & request)
{
    check_methods(_obj_location, request);
    checkHTTP(request);
    check_Transfer_Encoding(request);
    check_Content_Length(request, _obj_location);
    check_uri_length(request);
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
                std::cout << COLOR_RED << "Error: " << e.what() << COLOR_RESET << std::endl;
                if (it->second.customErrorFound == false)
                {
                    std::cout << "Error: " << e.what() << std::endl;
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
        //system("leaks Webserv");
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