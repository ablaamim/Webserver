#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../Servers/Servers.hpp"

class Webserv
{
    public :
        Webserv(char *config_file);
        ~Webserv();

        void    run();

        int     event_check(struct kevent *event, int kq_return);
        //int     accept_connection(Servers::socket_t *new_socket);

    private :
        int             kq;
        struct timespec timeout;
};

#endif