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
        class socket_t
        {
            public :
                int         socket_fd;
                sockaddr_in address;
                int         option;
                int         address_len;
                std::string ip;
                std::string port;
        };
    
    // map of socket_t with the key being the socket file descriptor
    typedef std::map<int, socket_t> socket_map_t;

    private :

        socket_map_t    socket_ip_port;

        void new_server_create_socket(std::string ip, std::string port);
        void listen_for_connections();

    public :
        // Constructor
        Servers(configurationSA &config);
        ~Servers();


    // Exceptions
    class Server_err : public std::exception
    {
        private :
            std::string error;

        public :
            Server_err(std::string error) : error(error) {}
            virtual const char *what() const throw() { return error.c_str(); }
            ~Server_err() throw() {};
    };
};
#endif