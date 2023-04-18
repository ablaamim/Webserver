#include "Servers.hpp"
#include "../parsing/ConfigurationParser.hpp"

void     Servers::new_server_create_socket(std::string ip, std::string port)
{
    socket_t    socketInfo;
    int         sockfd;

    socketInfo.ip = ip;
    socketInfo.port = std::stoi(port);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (!sockfd)
    {
        std::cout << "Error creating socket" << std::endl;
        exit (EXIT_FAILURE);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(socketInfo.opt), sizeof(socketInfo.opt) < 0))
    {
        close(sockfd);
    }

    socketInfo.addr.sin_family = AF_INET;
    socketInfo.addr.sin_addr.s_addr = inet_addr(socketInfo.ip.c_str());
    socketInfo.addr.sin_port = htons(socketInfo.port);
    socketInfo.addrlen = sizeof(socketInfo.addr);

    if (bind(sockfd, (struct sockaddr *)&socketInfo.addr, socketInfo.addrlen) < 0)
    {
        close(sockfd);
        //throw std::runtime_error("Error binding socket");
    }
    sockIpPort.insert(std::make_pair(sockfd, socketInfo));
}


Servers::Servers(configurationSA &config)
{
    std::cout << "Servers constructor called" << std::endl;

    configurationSA::data_type conf= config.get_data();
    std::set <std::pair<std::string, std::string> > bindSockets;
    
    // Loop through the configuration file and create a socket for each server
    for (configurationSA::data_type::iterator iterConf = conf.begin(); iterConf != conf.end(); iterConf++)
    {   
        //std::cout << "FIRST LOOP " << std::endl;
        for (configurationSA::Server::type_listen::iterator iterListen = iterConf->listen.begin(); iterListen != iterConf->listen.end(); iterListen++)
        {
            //std::cout << "SECOND LOOP " << std::endl;
            for (std::set<std::string>::iterator iterSet = iterListen->second.begin(); iterSet != iterListen->second.end(); iterSet++)
            {
                if (!bindSockets.count(std::make_pair(iterListen->first, *iterSet)))
                {
                    new_server_create_socket(iterListen->first, *iterSet);
                    bindSockets.insert(std::make_pair(iterListen->first, *iterSet));
                }
                std::cout << "------------------------" << std::endl;
                std::cout << "\rServer " << iterConf - conf.begin() << std::endl;
                std::cout << "IP  : " << iterListen->first << std::endl;
                std::cout << "Port: " << *iterSet << std::endl;
                std::cout << "------------------------" << std::endl;
                std::cout.flush();
                usleep(1000000);
            }
            /*
            std::cout << "------------------------" << std::endl;
            std::cout << "\rServer " << iterConf - conf.begin() << std::endl;
            std::cout << "IP  : " << iterListen->first << std::endl;
            std::cout << "------------------------" << std::endl;
            */
        }
    }
}

Servers::~Servers()
{
    std::cout << "Servers destructor called" << std::endl;
}