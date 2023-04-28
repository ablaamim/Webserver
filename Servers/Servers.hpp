#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "../MainInc/main.hpp"
#include "../defines/defines.hpp"

class configurationSA;

class Servers
{
    public :  
        int     kq;
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
                int         success_flag;
        };

    // map of socket_t with the key being the socket file descriptor
    typedef std::map<int, socket_t> socket_type;
    
    static std::vector<int>  fd_vector;
    
    struct      timespec timeout;
    
    void print_fd_vector(void)
    {
        std::cout << "fd_vector = ";
        for (std::vector<int>::iterator it = fd_vector.begin(); it != fd_vector.end(); it++)
            std::cout << *it << " ";
        std::cout << std::endl;
    }
    
    private :

        socket_type socket_ip_port;
        void        new_server_create_socket(std::string ip, std::string port);
        void        listen_for_connections();


    public :
        // Constructor
        Servers(configurationSA &config);
        ~Servers();
        
        Servers::socket_type get_socket_ip_port(void);
        std::vector<struct kevent> change_list;
        struct kevent event_list[EVENT_LIST];


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