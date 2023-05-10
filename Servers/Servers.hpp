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

        std::map<int, socket_t*>    socket_map;

        void print_socket_map()
        {
            //std::cout << std::endl << "                  socket_map : " << std::endl;
            for (std::map<int, socket_t*>::iterator iter = socket_map.begin(); iter != socket_map.end(); iter++)
            {
                std::cout << "socket_fd: " << iter->first << std::endl;
                std::cout << "ip: " << iter->second->ip << std::endl;
                std::cout << "port: " << iter->second->port << std::endl;
                std::cout << "address: " << inet_ntoa(iter->second->address.sin_addr) << std::endl;
                std::cout << "-------------------------------------------------------" << std::endl;
            }
            std::cout << "-------------------------------------------------------" << std::endl;
        }

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