#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../parsing/ConfigurationParser.hpp"
#include "../Servers/Servers.hpp"

class Webserv
{
    public :

        int                        kq;
        struct kevent*             event_list;
        std::map<int, std::string> clients;
    
        int                        fd_accepted;

        Webserv()
        {
            //std::cout << "Webserv default constructor" << std::endl;
        };

        Webserv(configurationSA &config);
        
        ~Webserv();

        void    webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s, configurationSA &config, Servers &server);
        void    webserv_evfilt_write(struct kevent *curr_eventg,configurationSA &config, Servers &server);
        void    change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void    disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str);
        
        void    run(std::vector<int> & fds_socket, configurationSA &config, Servers &server);
        
        void    event_check(int even_num, std::vector<int> & fds_socket, configurationSA &config, Servers &server);
        void    delete_event(int fd, int16_t filter, std::string str);
    
 

        void print_clients()
        {
            std::cout << std::endl << std::endl << COLOR_BLUE << "Clients map" << COLOR_RESET << std::endl;
            for (std::map<int, std::string>::iterator iter = clients.begin(); iter != clients.end(); iter++)
            {
                std::cout << "fd: " << iter->first << " ip: " << iter->second << std::endl;
            }
        };

    class Webserv_err : public std::exception
    {
        private :
            std::string error;
        
        public :
            Webserv_err(std::string error) : error(error) {}
            virtual const char *what() const throw() { return error.c_str(); }
            ~Webserv_err() throw() {};
    };
};

#endif