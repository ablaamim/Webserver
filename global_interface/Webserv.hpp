#ifndef WEBSERV_HPP
# define WEBSERV_HPP

//#include "../MainInc/main.hpp"
#include "../parsing/ConfigurationParser.hpp"

//class configurationSA;

class Webserv
{
    public :
        Webserv()
        {
            //std::cout << "Webserv default constructor" << std::endl;
        };

        Webserv(configurationSA &config);
        
        ~Webserv();

        void    webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s);
        void    webserv_evfilt_write(struct kevent *curr_event, configurationSA &config);
        void    change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void    disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str);
        void    run(std::vector<int> & fds_socket, configurationSA &config);
        void    event_check(int even_num, std::vector<int> & fds_socket, configurationSA &config);
        void    delete_event(int fd, int16_t filter, std::string str);
        
    private :
        int             kq;
        struct kevent *event_list;
        std::map<int, std::string> clients;

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