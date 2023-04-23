#include "simple_socket.hpp"

netsock::simple_socket::simple_socket(int domain, int service, int protocol, int port, u_long interface)
{
    // Define the socket address structure.
    address.sin_family = domain;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = htonl(interface);
    // Establish the socket connection.
    sock = socket(domain, service, protocol);
    test_connection(sock);
}

void netsock::simple_socket::test_connection(int item_to_test)
{
    if (item_to_test < 0)
    {
        perror("Error connecting to network ...");
        exit(EXIT_FAILURE);
    }
}