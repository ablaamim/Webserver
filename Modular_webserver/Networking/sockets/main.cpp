#include "listening_socket.hpp"

int main(void)
{
    std::cout << "Starting ..." << std::endl;

    netsock::binding_socket bs = netsock::binding_socket(AF_INET, SOCK_STREAM, 0, 81, INADDR_ANY);
    
    std::cout << "Listening ..." << std::endl;

    netsock::listening_socket ls = netsock::listening_socket(AF_INET, SOCK_STREAM, 0, 80, INADDR_ANY, 10);  

    std::cout << "Finished." << std::endl;
}