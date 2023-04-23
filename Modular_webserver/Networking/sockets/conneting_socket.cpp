#include "connecting_socket.hpp"

netsock::connecting_socket::connecting_socket(int domain, int service, int protocol, int port, u_long interface)
: simple_socket(domain, service, protocol, port, interface)
{
    set_connection(connect_to_network(get_sock(), get_address()));
    test_connection(get_connection());
}

// definition of the virtual function connect_to_network() :

int netsock::connecting_socket::connect_to_network(int sock, struct sockaddr_in address)
{
    return (connect(sock, (struct sockaddr *) &address, sizeof(address)));
}