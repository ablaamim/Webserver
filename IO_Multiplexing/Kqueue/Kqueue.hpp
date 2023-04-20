#ifndef KQUEUE_HPP

# include "../../Servers/Servers.hpp"
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

class Servers;

class Kqueue
{
    private :
    
        int kq;
        int kqueue_return;

    public :
        // Constructor
        Kqueue(Servers &server);
        ~Kqueue();

        int get_kq_fd() const;
        int get_kqueue_return() const;
        int check_events(struct kevent *events, int kqueue_return);
};

#endif