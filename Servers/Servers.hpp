#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "../MainInc/main.hpp"

class configurationSA;

class Servers
{
    public :  
        
        int                     kq;
        static std::vector<int> fd_vector;
        
        Servers(configurationSA &config);
        ~Servers();
        
        void    new_server_create_socket(std::string ip, std::string port);
        void    thr_exce_close(std::string str, int socket_fd);

        
        class socket_t
        {
            public :
                int         socket_fd;
                sockaddr_in address;
                int         option;
                int         address_len;
                std::string ip;
                std::string port;
        };

    // map of socket_t with the key being the socket file descriptor
    typedef std::map<int, socket_t> socket_type;
    struct      timespec timeout;
    
    private :

        socket_type socket_ip_port;
        void        new_server_create_socket(std::string ip, std::string port);
        void        listen_for_connections();


    public :
        // Constructor
        Servers(configurationSA &config);
        ~Servers();
        
        Servers::socket_type get_socket_ip_port(void);

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