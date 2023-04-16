#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "../parsing/ConfigurationParser.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class Servers
{
    public :
    
        struct socket_t
        {
            int         sockfd;
            sockaddr_in addr;
            int         opt;
            int         addrlen;
            std::string ip;
            int         port;
        };
    
    typedef std::map<int, socket_t> socket_map_t;

    private :
        socket_map_t    sockIpPort;

        void NewServerCreateSocket(std::string ip, std::string port);


    public :
        Servers(configurationSA &config);
        ~Servers();
};
#endif