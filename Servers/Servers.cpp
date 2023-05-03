
#include "../MainInc/main.hpp"
#include "../Servers/Servers.hpp"


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
    socket_info->success_flag = 1;

    Servers::fd_vector.push_back((int)socket_info->socket_fd);
    if (socket_info->socket_fd < 0)
        throw Server_err(SOCKET_CREATE_ERR);

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
        thr_exce_close("fnctl error" , socket_info->socket_fd);

    EV_SET(&ev, socket_info->socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    if (kevent(this->kq, &ev, 1, NULL, 0, NULL) == -1)
        throw Server_err("kqueue error");
}
