#include "../../Servers/Servers.hpp"

int    Kqueue::check_events(struct kevent *event, int kqueue_return)
{
    for (int i = 0; i < kqueue_return; i++)
    {
        if (event[i].flags & EV_EOF)
        {
            std::cout << "Connection closed" << std::endl;
        }
        else if (event[i].flags & EV_ERROR)
        {
            std::cout << "Error" << std::endl;
        }
        else if (event[i].filter == EVFILT_READ)
        {
            std::cout << "Read" << std::endl;
        }
        else if (event[i].filter == EVFILT_WRITE)
        {
            std::cout << "Write" << std::endl;
        }
        else
        {
            std::cout << "Unknown" << std::endl;
        }
    }
    return (0);
}

int Kqueue::get_kqueue_return() const
{
    return (kqueue_return);
}

Kqueue::Kqueue(Servers &servers)
{
    this->kqfd = kqueue();
    this->kqueue_return = 0;
    struct timespec timeout;
    timeout.tv_sec = 2;
    timeout.tv_nsec = 0;
    if (kqfd == -1)
    {
        std::cerr << "kqueue failed" << std::endl;
        //throw std::runtime_error("kqueue");
    }
    struct kevent event[10000];
    this->kqueue_return = kevent(kqfd, NULL, 0, event, 10000, &timeout);
    if (kqueue_return == -1)
    {
        throw 
    }
    //std::cout << "kqueue_return: " << kqueue_return << std::endl;
    check_events(event, kqueue_return);

}

Kqueue::~Kqueue()
{
    close(kqfd);
}

int Kqueue::get_kq_fd() const
{
    return (kqfd);
}