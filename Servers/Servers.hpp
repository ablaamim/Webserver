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
                size_t      data_s;
                int         success_flag;
        };
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