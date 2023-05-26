
#include "../MainInc/main.hpp"

std::map<int, int>               clients_list;                // map of (client_socket, server_socket)
//std::map<int, Request>           request_list;                // map of (client_socket, request_socket)
std::map<int, Response>          responsePool;                // map of (client_socket, Response)

//////////////////////////////////////////////////////// DEBUG FUNCTIONS /////////////////////////////////////////////////////////

void print_responsePool(std::map<int, Response> responsePool)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "ResponsePool list :" << COLOR_RESET << std::endl;
    for (std::map<int, Response>::iterator iter = responsePool.begin(); iter != responsePool.end(); iter++)
    {
        std::cout << COLOR_YELLOW << "[ client_socket : " << iter->first << " , " << " Response client Socket : " << iter->second.clientSocket << " ]" << COLOR_RESET << std::endl;
    }
}

void print_client_list(std::map<int, int> clients_list)
{
    std::cout << std::endl << std::endl << COLOR_BLUE << "Client list : " << COLOR_RESET;
    for (std::map<int, int>::iterator iter = clients_list.begin(); iter != clients_list.end(); iter++)
    {
        std::cout << COLOR_YELLOW << "[ client_socket : " << iter->first << " , " << " Server Socket : " << iter->second << " ]" << COLOR_RESET << std::endl;
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

void Webserv::change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata)
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
    int n = 0, k = 120;


    // first connection
    if(fds_s.end() != std::find(fds_s.begin(), fds_s.end(), curr_event->ident))
    {
        if((client_socket =  accept(curr_event->ident, NULL, NULL)) < 0)
            throw Webserv::Webserv_err("accept error");
        
        std::cout << " accept new client: " << client_socket << std::endl;
        
        
        change_events(client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        setsockopt(client_socket, SOL_SOCKET, SO_KEEPALIVE, &k, sizeof(int));
        this->clients[client_socket] = "";
        this->request[client_socket].fd_accept = client_socket;
        this->request[client_socket].fd_server = curr_event->ident;
        clients_list.insert(std::make_pair(client_socket, this->request[client_socket].fd_server));     

    }
    else if (this->clients.find(curr_event->ident)!= this->clients.end())
    {
        //std::cout << "read from client: " << curr_event->ident << std::endl;
        // already connected
        n = recv(curr_event->ident, buf, BUFFER_SIZE - 1, 0);
        if (n <= 0)
        {
            std::cout << "read error " << strerror(errno) << std::endl;
            disconnect_client(curr_event->ident, this->clients, "read");
            return ;
        }
        buf[n] = '\0';
        
        this->clients[curr_event->ident].append(buf);
        k = this->request[curr_event->ident].parse_request(buf);

        if (!k)
        {
            //std::cout << this->request[curr_event->ident] << std::endl;
            Request request = this->request[curr_event->ident];
            //std::cout << "request parsed" << std::endl;
         
            //request.print_params();
            std::cout << COLOR_YELLOW << "Client " << curr_event->ident << " is being created" << COLOR_RESET << std::endl;
            std::map<int, int>::iterator pair_contact = clients_list.find(curr_event->ident);
            configurationSA::Server     _obj_server = Select_server(config, server.find_ip_by_fd(pair_contact->second), server.find_port_by_fd(pair_contact->second), config.get_data(), "127.0.0.1");
            // find url in request obj
            std::string url = request.params["Url"];
            configurationSA::location   _obj_location = match_location(url, _obj_server); 
            //std::cout << "ACCEPTED PARAM RES = " << curr_event->ident << std::endl;
            Response newResponse(request, curr_event->ident, _obj_location, env);
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
            for (std::set<std::string>::iterator it = _obj_server.server_name.begin(); it != _obj_server.server_name.end(); it++)
                newResponse.kwargs.insert(std::make_pair("server_name", std::vector<std::string> (1, *it)));
            //newResponse.print_kwargs();
            responsePool.insert(std::make_pair(curr_event->ident, newResponse));
            change_events(curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
            delete_event(curr_event->ident, EVFILT_READ, "delete READ event");
        }
    }
    //print_client_list(clients_list);
    //print_request_list(request_list);
}

void Webserv::webserv_evfilt_write(struct kevent *curr_event, configurationSA &config, Servers &server, char **env)
{
    // std::cout << COLOR_RED << "----> WRITE EVENT" << COLOR_RESET <<std::endl;

    // // // //std::cout << "write event" << std::endl;
    // std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello Wor12!";
    //         if (send(curr_event->ident, hello.c_str(), hello.size(), 0) < 0)
    //         {
    //             std::cout << "error " << strerror(errno) << std::endl;
    //             disconnect_client(curr_event->ident, this->clients, "write");
    //         }
    //         else
    //         {
    //             delete_event(curr_event->ident, EVFILT_WRITE, "delete write event");
    //             this->request[curr_event->ident].reset_request();
    //             this->clients[curr_event->ident].clear();
    //         }
    if (this->clients.find(curr_event->ident) != this->clients.end())
    {
        // print client with his id
        if (this->clients[curr_event->ident] != "")
        {
            
            // find the client in the response pool
            std::map<int, Response>::iterator it = responsePool.find(curr_event->ident);
            std::cout << COLOR_RED << "Client " << it->second.clientSocket << " has been joined the response pool" << COLOR_RESET << std::endl;
            // debug
            if (it->second.isCompleted)
            {
                std::cout << COLOR_GREEN << "Client " << it->second.clientSocket << " has been served" << COLOR_RESET << std::endl;
                delete_event(curr_event->ident, EVFILT_WRITE, "delete write event");
                this->request[curr_event->ident].reset_request();
                responsePool.erase(it);
                disconnect_client(curr_event->ident, this->clients, "write");
                clients_list.erase(curr_event->ident);
                this->clients[curr_event->ident].clear();


            }
            else
            {
                // SAT MELI TESTER FEL BROWSER DIR RESSOURCEPATH FLAKHER DYAL URL EXAMPLE : localhost:8080/slayer.mp4
                

                // MAZAL MAYDAR GHDA UNKMLU
                
                //char buf[1024] = {0};
                std::cout << "SERVING" << std::endl;
                it->second.isCompleted = true;
                std::stringstream ss(this->clients[curr_event->ident]);
                std::string request_type, request_path, http_version;
                ss >> request_type >> request_path >> http_version;

                // Open video file
                std::ifstream video_file(responsePool[curr_event->ident].resourceFullPath, std::ios::binary);

                // Construct HTTP response headers
                std::stringstream response;
                response << responsePool[curr_event->ident].httpVersion << " 200 OK\r\n";
                response << "Content-Type: video/mp4\r\n";
                response << "Transfer-Encoding: chunked\r\n";
                response << "\r\n";

                // Send HTTP response headers to client
                send(curr_event->ident, response.str().c_str(), response.str().size(), 0);

                // Send video data to client in chunks
                char chunk_buffer[4096];
                while (!video_file.eof())
                {
                    video_file.read(chunk_buffer, sizeof(chunk_buffer));
                    int chunk_size = video_file.gcount();

                    // Send chunk size as hexadecimal string
                    std::stringstream chunk_size_ss;
                    chunk_size_ss << std::hex << chunk_size << "\r\n";
                    std::string chunk_size_str = chunk_size_ss.str();
                    send(curr_event->ident, chunk_size_str.c_str(), chunk_size_str.size(), 0);

                    // Send chunk data
                    send(curr_event->ident, chunk_buffer, chunk_size, 0);

                    // Send chunk terminator
                    send(curr_event->ident, "\r\n", 2, 0);
                }

                // // Send final chunk terminator
                send(curr_event->ident, "0\r\n\r\n", 5, 0);
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
        // else if (this->event_list[i].flags & EV_EOF)
        // {
        //     delete_event(this->event_list[i].ident, EVFILT_READ, "read eof ");
        //     clients_list.erase(this->event_list[i].ident);
        //     responsePool.erase(this->event_list[i].ident);
        //     //this->clients[this->event_list[i].ident].clear();
        //     disconnect_client(this->event_list[i].ident, this->clients, "EV_EOF");
        // }
        else if (this->event_list[i].filter == EVFILT_READ)
        {
            webserv_evfilt_read(&this->event_list[i], fds_s, config, server, env);        
        }
        else if (this->event_list[i].filter == EVFILT_WRITE)
            webserv_evfilt_write(&this->event_list[i], config, server, env);
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
    this->event_list = new struct kevent [Servers::fd_vector.size()];
    this->run(Servers::fd_vector, config, server, env);
}

// default destructor
Webserv::~Webserv(){delete [] this->event_list;}
Webserv::Webserv(){}