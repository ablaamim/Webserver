#ifndef LISTENING_SOCKET_HPP

# define LISTENING_SOCKET_HPP

# include "binding_socket.hpp"

namespace netsock
{
    class listening_socket : public binding_socket
    {
        private :
        
            int backlog;
            int listening;
        
        public :
            
            listening_socket(int domain, int service, int protocol, int port, u_long interface, int backlog);
            
            // virtual function from parent class :
            void start_listening();
    };
};

#endif