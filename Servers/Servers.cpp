#include "Servers.hpp"
#include "../parsing/ConfigurationParser.hpp"

void     Servers::new_server_create_socket(std::string ip, std::string port)
{
    socket_t    socket_info;
    int         socket_fd;

    socket_info.ip = ip;
    socket_info.port = std::stod(port);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    socket_info.socket_fd = socket_fd;
    socket_info.option = 1;

    if (!socket_fd)
    {
        throw Server_err("Error creating socket");
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(socket_info.option), sizeof(socket_info.option) < 0))
    {
        close(socket_fd);
        //throw Server_err("Error setting socket options");
    }

    socket_info.address.sin_family = AF_INET;
    socket_info.address.sin_addr.s_addr = inet_addr(socket_info.ip.c_str());
    socket_info.address.sin_port = htons(socket_info.port);
    socket_info.address_len = sizeof(socket_info.address);

    if (bind(socket_fd, (struct sockaddr *)&socket_info.address, socket_info.address_len) < 0)
    {
        close(socket_fd);
        //throw Server_err("Error binding socket");
    }
        
    sockIpPort.insert(std::make_pair(socket_fd, socket_info));
    
    //std::cout << "Socket created" << std::endl;
}


Servers::Servers(configurationSA &config)
{
    std::cout << "Servers constructor called" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    int number_of_sockets = 0;

    configurationSA::data_type conf = config.get_data();

    std::set <std::pair<std::string, std::string> > bind_sockets;
    try
    {
        // Loop through the configuration file and create a socket for each server
        for (configurationSA::data_type::iterator iterConf = conf.begin(); iterConf != conf.end(); iterConf++)
        { 
            //std::cout << "FIRST LOOP " << std::endl;
            for (configurationSA::Server::type_listen::iterator iterListen = iterConf->listen.begin(); iterListen != iterConf->listen.end(); iterListen++)
            {
                //std::cout << "SECOND LOOP " << std::endl;
                for (std::set<std::string>::iterator iterSet = iterListen->second.begin(); iterSet != iterListen->second.end(); iterSet++)
                {
                
                    if (!bind_sockets.count(std::make_pair(iterListen->first, *iterSet)))
                    {
                        new_server_create_socket(iterListen->first, *iterSet);
                        bind_sockets.insert(std::make_pair(iterListen->first, *iterSet));
                    }
                    //std::cout << "Number of sockets " << sockIpPort.size() << std::endl;

                    std::cout << "\rServer "  << iterConf - conf.begin() << " IP " << iterListen->first << " Port " << *iterSet << std::endl;                
                    std::cout << "----------------------------------------" << std::endl;
                    std::cout.flush();
                    usleep(1000000);
                }
            
                std::cout << "Number of sockets " << ++number_of_sockets << std::endl;
                std::cout << "----------------------------------------" << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        throw Server_err(e.what());
    }
}

Servers::~Servers()
{
    std::cout << "Servers destructor called" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}