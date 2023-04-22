#include "Servers.hpp"
#include "../parsing/ConfigurationParser.hpp"

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

void     Servers::new_server_create_socket(std::string ip, std::string port)
{
    socket_t    socket_info;
    int         socket_fd;
    //this->kq = kqueue();

    socket_info.ip = ip;
    socket_info.port = port;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    socket_info.socket_fd = socket_fd;
    socket_info.option = 1;
    this->socket_fd = socket_fd;
    if (!socket_fd)
        throw Server_err(SOCKET_CREATE_ERR);
    
    // SOL_SOCKET is the socket layer itself
    
    // SO_REUSEADDR allows the local address to be reused when the server is restarted
    
    // setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &socket_info.option, sizeof(socket_info.option));
    
    // std::cout << "Socket fd = " << socket_fd << std::endl;
    // std::cout << "Socket option = " << socket_info.option << std::endl;
    
    // ONCE I THROW AN EXCEPTION HERE, I CAN'T RECOVER FROM IT AND THE SERVER CRASHES !!!!!!!!!!!!!!!!!!!!!!!!!

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_info.option, sizeof(socket_info.option)) < 0)
    {
        ///std::cout << "PROBLEM HERE ! " << std::endl;
        close(socket_fd);
        throw Server_err(SOCKET_OPTION_ERR);
    }
    
    socket_info.address.sin_family = AF_INET;
    socket_info.address.sin_addr.s_addr = inet_addr(ip.c_str());
    socket_info.address.sin_port = htons(atoi(port.c_str()));
    socket_info.address_len = sizeof(socket_info.address);
    addr = socket_info.address;
    if (inet_aton(ip.c_str(), &socket_info.address.sin_addr) == 0)
    {
        close(socket_fd);
        throw Server_err("Inet failure");
    }
    memset(socket_info.address.sin_zero, '\0', sizeof(socket_info.address.sin_zero));    
    if (bind(socket_fd, (struct sockaddr *) &socket_info.address, sizeof(socket_info.address)) < 0)
    {
        close(socket_fd);
        throw Server_err(SOCKET_BINDING_ERR);
    }
    
    if (listen(socket_fd, TIMEOUT) < 0)
    {
        close(socket_fd);
        throw Server_err(SOCKET_LISTEN_ERR);
    }
    
    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        close(socket_fd);
        throw Server_err("fnctl error");
    }
    
    // KQ IMPLEMENTATION HERE
    
    struct kevent event;

    EV_SET(&event, socket_fd, EVFILT_READ, EV_ADD, 0, 0, &socket_info);

    if (kevent(get_kq(), &event, 1, NULL, 0, NULL) < 0)
    {
        close(socket_fd);
        throw Server_err("kevent error");
    }
    std::cout << "----------------------------------------------------------------" << std::endl;
    std::cout << COLOR_GREEN << "Socket PORT    = "  << COLOR_BLUE << port << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "Socket IP ADDR = " << COLOR_BLUE << ip << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "Socket Fd      = " << COLOR_BLUE << socket_fd << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << "Socket Option  = " << COLOR_BLUE << socket_info.option << COLOR_RESET << std::endl;
    //std::cout << COLOR_GREEN << "kernel Queue   =  " << COLOR_BLUE << configueationSA::kq << COLOR_RESET <<std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

    socket_ip_port.insert(std::make_pair(socket_fd, socket_info));
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

Servers::Servers(configurationSA &config)
{
    configurationSA::data_type conf = config.get_data();
    std::set <std::pair<std::string, std::string> > bind_sockets_list;
    this->kq = kqueue();
    try
    {
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
        //listen_for_connections();
        //std::cout << socket_ip_port.size() << " sockets created" << std::endl;
        //std::cout << socket_ip_port.begin()->first << std::endl;
        //if (fcntl(socket_ip_port.begin()->first, F_SETFL, O_NONBLOCK) < 0)
            //throw Server_err("fnctl error");        
    }
    catch (const std::exception& e)
    {
        for (socket_type::iterator iter = socket_ip_port.begin(); iter != socket_ip_port.end(); iter++)
            close(iter->first);
        throw Server_err(e.what());
    }
}

Servers::~Servers()
{
    std::cout << "Servers destructor called" << std::endl;
    for (socket_type::iterator iter = socket_ip_port.begin(); iter != socket_ip_port.end(); iter++)
    {
        close(iter->first);
    }
    std::cout << "----------------------------------------" << std::endl;
}