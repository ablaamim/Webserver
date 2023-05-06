#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../MainInc/main.hpp"

class Webserv
{
    public :
        Webserv(char *config_file);
        ~Webserv();

        void    run();

        int     event_check(struct kevent *event, int kq_return);
        //int     accept_connection(Servers::socket_t *new_socket);
        int             kq;
        struct timespec timeout;
    
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