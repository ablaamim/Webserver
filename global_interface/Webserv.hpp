#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#include "../Servers/Servers.hpp"

class Webserv
{
    public :
        Webserv(char *config_file);
        ~Webserv();

        void    run(std::vector<int> & fds_socket);
        int     event_check(struct kevent *event, int kq_return, std::vector<int> & fds_socket);
        std::vector<struct kevent> change_list;
        struct kevent event_list[EVENT_LIST];



    private :
        int             kq;
        struct timespec timeout;
};

#endif