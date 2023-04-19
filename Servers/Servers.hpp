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
            int         socket_fd;
            sockaddr_in address;
            int         option;
            int         address_len;
            std::string ip;
            int         port;
        };
    
    // map of socket_t with the key being the socket file descriptor
    typedef std::map<int, socket_t> socket_map_t;

    private :

        socket_map_t    sockIpPort;

        void new_server_create_socket(std::string ip, std::string port);

    public :

        Servers(configurationSA &config);
        ~Servers();
};
#endif