#ifndef SERVERS_HPP
# define SERVERS_HPP

#include "../MainInc/main.hpp"

class configurationSA;

class Servers
{
    public :  
        
        int                     kq;
        static std::vector<int> fd_vector;
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
    
    socket_type socket_map;

    void print_server()
    {
        std::cout << std::endl << std::endl << COLOR_BLUE << "Server map" << COLOR_RESET << std::endl;
        for (socket_type::iterator iter = socket_map.begin(); iter != socket_map.end(); iter++)
        {
            std::cout << "fd: " << iter->first << " ip: " << iter->second.ip << " port: " << iter->second.port << std::endl;
        }
    };
    
    struct      timespec timeout;

    void print_socket_map()
    {
        std::cout << std::endl << std::endl << COLOR_BLUE << "Socket map" << COLOR_RESET << std::endl;
        for (socket_type::iterator iter = socket_map.begin(); iter != socket_map.end(); iter++)
        {
            std::cout << "fd: " << iter->first << " ip: " << iter->second.ip << " port: " << iter->second.port << std::endl;
        }
    };
    
    socket_type socket_ip_port; // map of socket_t with the key being the fd of the socket that listens on the ip and port

    void        new_server_create_socket(std::string ip, std::string port);

    socket_type::iterator get_socket_by_fd(int fd)
    {
        socket_type::iterator iter = socket_map.begin();
        for (; iter != socket_map.end(); iter++)
        {
            if (iter->first == fd)
                return (iter);
        }
        return (iter);
    }

    std::string find_ip_by_fd(int fd)
    {
        return (socket_map[fd].ip);
    }

    std::string find_port_by_fd(int fd)
    {
        return (socket_map[fd].port);
    }

    
    public :
        // Constructor
        Servers(configurationSA &config);
        ~Servers();
        
        Servers::socket_type get_socket_ip_port(void)
        {
            return (socket_ip_port);
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