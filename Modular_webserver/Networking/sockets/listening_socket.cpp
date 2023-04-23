#include "listening_socket.hpp"

netsock::listening_socket::listening_socket(int domain, int service, int protocol, int port, u_long interface, int backlog)
: binding_socket(domain, service, protocol, port, interface), backlog(backlog)
{
    start_listening();
    test_connection(listening);
}

void netsock::listening_socket::start_listening()
{
    listening = listen(get_connection(), backlog);
}