#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../MainInc/main.hpp"
#include "../parsing/ConfigurationParser.hpp"

class Webserv : public configurationSA
{
    public :
        Webserv();
        Webserv(char *config_file);
        ~Webserv();

        void    webserv_evfilt_read(struct kevent *curr_event, std::vector<int> & fds_s);
        void    webserv_evfilt_write(struct kevent *curr_event);
        void    change_events(uintptr_t ident, int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
        void    disconnect_client(int client_fd, std::map<int, std::string>& clients, std::string str);
        void    run(std::vector<int> & fds_socket);
        void    event_check(struct kevent *event, int kq_return, std::vector<int> & fds_socket);
        void    delete_event(int fd, int16_t filter);

        void    delete_client(int id);
        std::map<int, std::string>    &get_clients();
        
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