#ifndef EPOLL_HPP
# define EPOLL_HPP

#include "../MainInc/main.hpp"
#include <sys/epoll.h>

class Epoll
{
    private :
        int epoll_fd;
        int epoll_return;
        struct epoll_event event;
        struct epoll_event *events;

    public :
        // Constructor
        Epoll(Servers &server);
        ~Epoll();

        int get_epoll_fd() const;
        int get_epoll_return() const;
};

#endif