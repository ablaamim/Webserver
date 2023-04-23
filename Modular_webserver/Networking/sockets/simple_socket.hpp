#ifndef SIMPLE_SOCKET_HPP
#define SIMPLE_SOCKET_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

namespace netsock
{
    // SOCKET OBJECT :
    
    class simple_socket
    {
        public :

            simple_socket(int domain, int service, int protocol, int port, u_long interface);
            
            virtual int         connect_to_network(int sock, struct sockaddr_in address) = 0;
            void                test_connection(int);
        
            // Getters and setters :
            struct sockaddr_in  get_address() { return address; }
            int                 get_sock() { return sock; }
            int                 get_connection() { return connection; }
            void                set_connection(int new_connection) { connection = new_connection; }
            void                set_sock(int new_sock) { sock = new_sock; }
            void                set_address(struct sockaddr_in new_address) { address = new_address; }

        private :

            int                 sock;
            int                 connection;
            struct sockaddr_in  address;
    };
};

#endif