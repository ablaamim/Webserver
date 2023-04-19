#include "Kqueue.hpp"
#include "../Servers/Servers.hpp"

Kqueue::Kqueue(Servers &server)
{
    (void) server;
    kq = kqueue();
    if (kq == -1)
    {
        std::cerr << "kqueue() failed" << std::endl;
    }
    std::cout << "kq = " << kq << std::endl;
}

Kqueue::~Kqueue()
{
    close(kq);
    delete[] &event_list;
}

int Kqueue::get_kq_fd() const
{
    return (kq);
}