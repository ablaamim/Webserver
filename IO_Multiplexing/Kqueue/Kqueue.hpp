#ifndef KQUEUE_HPP

# include "../../Servers/Servers.hpp"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

class Kqueue
{
    private :
    
        int kqfd;
        int kqueue_return;
        struct kevent *events;

    public :
        // Constructor
        Kqueue(Servers &server);
        ~Kqueue();

        int get_kq_fd() const;
        int get_kqueue_return() const;
        int check_events(struct kevent *events, int kqueue_return);
};

#endif