#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../MainInc/main.hpp"

class socket_t;

class Webserv
{
    public :
        Webserv(char *config_file);
        ~Webserv();

        void    run();

        int     event_check(struct kevent *event, unsigned int kq_return);
        //void    add_event(int socket_fd, uint16_t filter, Servers::socket_t *socket_info);


    private :
        int             kq;
        struct timespec timeout;
};

#endif