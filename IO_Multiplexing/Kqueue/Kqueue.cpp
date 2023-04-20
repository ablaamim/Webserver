#include "../../Servers/Servers.hpp"

int    Kqueue::check_events(struct kevent *event, int kqueue_return)
{
    for (int i = 0; i < kqueue_return; i++)
    {
        std::cout << "Kqueue event" << std::endl;
    }
    return (0);
}

int Kqueue::get_kqueue_return() const
{
    return (kqueue_return);
}

Kqueue::Kqueue(Servers &server)
{
    (void) server;
    // Init kqueue :
    
    kq = kqueue();

    if (kq == -1)
    {
        //throw std::runtime_error("Kqueue failed to init");
        perror("Kqueue failed to init");
        exit (EXIT_FAILURE);
    }
    
    this->kqueue_return = 0;
    
    struct kevent events[10000];
    
    struct timespec timeout;
    
    timeout.tv_sec = 2;
    
    timeout.tv_nsec = 0;

    kqueue_return = kevent(kq, NULL, 0, events, 10000, &timeout);
    
    std::cout << std::endl << "KQ VAL = " << kq << std::endl;
    
    if (kqueue_return < 0)
    {
        //throw std::runtime_error("Kqueue failed to init");
        perror("Kqueue failed to init");
        exit (EXIT_FAILURE);
    }
    else
    {
        check_events(events, kqueue_return);
    }
    //std::cout << "KQUEUE RET VAL = " << kqueue_return << std::endl;
}

Kqueue::~Kqueue()
{
    close(kq);
}

int Kqueue::get_kq_fd() const
{
    return (kq);
}