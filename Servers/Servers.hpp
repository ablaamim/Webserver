#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "../parsing/ConfigurationParser.hpp"
#include "../MainInc/main.hpp"

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

class configurationSA;

class Servers
{
    public :  
            int         socket_fd;
            sockaddr_in addr;
        class socket_t
        {
            public :
                int         socket_fd;
                sockaddr_in address;
                int         option;
                int         address_len;
                std::string ip;
                std::string port;
                size_t      data_s;
        };
    
    int get_kq(void)
    {
        return (kq);
    }

    void set_kq(int kq)
    {
        this->kq = kq;
    }

    // map of socket_t with the key being the socket file descriptor
    typedef std::map<int, socket_t> socket_type;
    
    private :

        socket_type socket_ip_port;
        int         kq;
        struct      timespec timeout;
        void        new_server_create_socket(std::string ip, std::string port);
        void        listen_for_connections();
        //void addEvent(int socket_fd, uint16_t filter, socket_t *socket_info);


    public :
        // Constructor
        Servers(configurationSA &config);
        ~Servers();
        
        Servers::socket_type get_socket_ip_port(void);
        //int get_kq(void);

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