
#include "../MainInc/main.hpp"

<<<<<<< HEAD
=======
Servers::socket_type Servers::get_socket_ip_port(void)
{
    return (socket_ip_port);
}

/*
int Servers::get_kq()
{
    return (kq);
}
*/

Servers::Servers()
{
}

void     Servers::new_server_create_socket(std::string ip, std::string port)
{
    socket_t    *socket_info = new socket_t;
    
    socket_info->ip = ip;
    socket_info->port = port;
    socket_info->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    socket_info->option = 1;
    socket_info->success_flag = 1;
    if (!socket_info->socket_fd)
        throw Server_err(SOCKET_CREATE_ERR);
    
    // SOL_SOCKET is the socket layer itself

    // SO_REUSEADDR allows the local address to be reused when the server is restarted
    
    // setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socket_info.option, sizeof(socket_info.option));
    
    // std::cout << "Socket fd = " << socket_fd << std::endl;
    // std::cout << "Socket option = " << socket_info.option << std::endl;
    
    // ONCE I THROW AN EXCEPTION HERE, I CAN'T RECOVER FROM IT AND THE SERVER CRASHES !!!!!!!!!!!!!!!!!!!!!!!!!

    if (setsockopt(socket_info->socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_info->option, sizeof(socket_info->option)) < 0)
    {
        close(socket_info->socket_fd);
        throw Server_err(SOCKET_OPTION_ERR);
    }
    
    memset(socket_info->address.sin_zero, '\0', sizeof(socket_info->address.sin_zero));    
    socket_info->address.sin_family = AF_INET;
    socket_info->address.sin_addr.s_addr = inet_addr(ip.c_str());
    socket_info->address.sin_port = htons(atoi(port.c_str()));
    socket_info->address_len = sizeof(socket_info->address);
    if (bind(socket_info->socket_fd, (struct sockaddr *) &socket_info->address, sizeof(socket_info->address)) < 0)
        throw Server_err(SOCKET_BINDING_ERR);
    
    if (listen(socket_info->socket_fd, TIMEOUT) < 0)
    {
        close(socket_info->socket_fd);
        throw Server_err(SOCKET_LISTEN_ERR);
    }
    
    if (fcntl(socket_info->socket_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        close(socket_info->socket_fd);
        throw Server_err("fnctl error");
    }
    delete socket_info;
}

void Servers::listen_for_connections()
{
    /*
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Listening for connections" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    */
    for (Servers::socket_type::iterator iter = socket_ip_port.begin(); iter != socket_ip_port.end(); iter++)
    {
        //std::cout << "Listening for connections on socket " << iter->first << std::endl;
        if (listen(iter->first, TIMEOUT) < 0)
        {
            close(iter->first);
            throw Server_err(SOCKET_LISTEN_ERR);
        }    
    }
}
>>>>>>> origin

Servers::Servers(configurationSA &config)
{
    configurationSA::data_type conf = config.get_data();
    std::set <std::pair<std::string, std::string> > bind_sockets_list;
    try
    {
        this->kq = kqueue();
        if (this->kq < 0)
            throw Server_err("kqueue error");
        for (configurationSA::data_type::iterator iterConf = conf.begin(); iterConf != conf.end(); iterConf++)
        { 
            for (configurationSA::Server::type_listen::iterator iterListen = iterConf->listen.begin(); iterListen != iterConf->listen.end(); iterListen++)
            {
                for (std::set<std::string>::iterator iterSet = iterListen->second.begin(); iterSet != iterListen->second.end(); iterSet++)
                {
                    if (!bind_sockets_list.count(std::make_pair(iterListen->first, *iterSet)))
                    {
                        std::cout << std::endl << COLOR_BLUE << "Creating socket for " << iterListen->first << " : " << *iterSet << COLOR_RESET <<std::endl;
                        new_server_create_socket(iterListen->first, *iterSet);
                        bind_sockets_list.insert(std::make_pair(iterListen->first, *iterSet));
                    }
                    std::cout << COLOR_GREEN << "\rLoading~"   << COLOR_RESET << "   listening on        " << COLOR_RED << iterListen->first << "         :  " <<  *iterSet << COLOR_RESET;                
                    std::cout.flush();
                    usleep(100000);
                }
            }
            std::cout << "\rServer "  << iterConf - conf.begin() << COLOR_GREEN <<"      Up               " << COLOR_RESET << std::endl;
        }
        //print_socket_map();

    }
    catch (const std::exception& e)
    {
        for (std::vector<int>::iterator iter = fd_vector.begin(); iter != fd_vector.end(); iter++)
            close(*iter);
        throw Server_err(e.what());
    }
}

Servers::~Servers()
{
    for (std::vector<int>::iterator iter = fd_vector.begin(); iter != fd_vector.end(); iter++)
        close(*iter);
}

void    Servers::thr_exce_close(std::string str, int socket_fd)
{
    close(socket_fd);
    throw Server_err(str);
}

void     Servers::new_server_create_socket(std::string ip, std::string port)
{
    socket_t    *socket_info = new socket_t;
    struct kevent ev;
    
    socket_info->ip = ip;
    socket_info->port = port;
    socket_info->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    socket_info->option = 1;

    Servers::fd_vector.push_back((int)socket_info->socket_fd);
    if (socket_info->socket_fd < 0)
        throw Server_err(SOCKET_CREATE_ERR);

    // SOL_SOCKET - socket level
    
    if (setsockopt(socket_info->socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_info->option, sizeof(socket_info->option)) < 0)
        thr_exce_close(SOCKET_OPTION_ERR , socket_info->socket_fd);
    
    memset(socket_info->address.sin_zero, '\0', sizeof(socket_info->address.sin_zero));    
    socket_info->address.sin_family = AF_INET;
    socket_info->address.sin_addr.s_addr = inet_addr(ip.c_str());
    socket_info->address.sin_port = htons(atoi(port.c_str()));
    socket_info->address_len = sizeof(socket_info->address);
    
    if (bind(socket_info->socket_fd, (struct sockaddr *) &socket_info->address, sizeof(socket_info->address)) < 0)
        throw Server_err(SOCKET_BINDING_ERR);
    
    if (listen(socket_info->socket_fd, EVENT_LIST) < 0)
        thr_exce_close(SOCKET_LISTEN_ERR , socket_info->socket_fd);
    
    if (fcntl(socket_info->socket_fd, F_SETFL, O_NONBLOCK) < 0)
        thr_exce_close("fnctl failed" , socket_info->socket_fd);

    EV_SET(&ev, socket_info->socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(this->kq, &ev, 1, NULL, 0, NULL) == -1)
        throw Server_err("kqueue error");
    socket_map.insert(std::make_pair(socket_info->socket_fd, socket_info));
    delete socket_info;
}
