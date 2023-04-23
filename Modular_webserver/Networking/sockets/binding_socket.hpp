# ifndef BINDING_SOCKET_HPP
# define BINDING_SOCKET_HPP

#include <stdio.h>
#include "simple_socket.hpp"

namespace netsock
{
    class binding_socket : public simple_socket
    {
        public :
            
                binding_socket(int domain, int service, int protocol, int port, u_long interface);
                
                // virtual function from parent class :
                int connect_to_network(int sock, struct sockaddr_in address);
    };
};

#endif