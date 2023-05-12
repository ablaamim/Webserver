#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../MainInc/main.hpp"

class Webserv
{
    public :
        Webserv(char *config_file);
        ~Webserv();

        void    webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s);
        void    webserv_evfilt_write(struct kevent *curr_event);
        void    change_events(std::vector<struct kevent>& change_list, uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void    disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str);
        void    run(std::vector<int> & fds_socket);
        void    event_check(int even_num, std::vector<int> & fds_socket);
        void    delete_event(int fd, int16_t filter, std::string str);
        
    private :
        int             kq;
        struct timespec timeout;
        std::vector<struct kevent> change_list;
        std::map<int, std::string> clients;
        struct kevent event_list[EVENT_LIST];

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