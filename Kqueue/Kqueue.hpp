#ifndef KQUEUE_HPP

# include "../Servers/Servers.hpp"
# include <sys/event.h>

class Kqueue
{
    private :

        int kq;
        std::vector<struct kevent> event_list;


    public :
        // Constructor
        Kqueue(Servers &server);
        ~Kqueue();

        int get_kq_fd() const;
};

#endif