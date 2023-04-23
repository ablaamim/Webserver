#ifndef CONNECTING_SOCKET_HPP
# define CONNECTING_SOCKET_HPP

# include "simple_socket.hpp"

namespace netsock
{
    class connecting_socket : public simple_socket
    {
        public :
            
            connecting_socket(int domain, int service, int protocol, int port, u_long interface)
            : simple_socket(domain, service, protocol, port, interface) {}
            
            // virtual function from parent class :
            int connect_to_network(int sock, struct sockaddr_in address);
    };
};

#endif