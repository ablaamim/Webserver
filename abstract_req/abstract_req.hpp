#ifndef ABSTRACT_REQ_HPP
# define ABSTRACT_REQ_HPP

#include "../MainInc/main.hpp"

class abstract_req
{
    public :
        abstract_req(int fd = -1)
        {
            _fd = fd;
            //std::cout << "Webserv default constructor" << std::endl;
        };
        ~abstract_req()
        {
            //std::cout << "Webserv default destructor" << std::endl;
        }

        int _fd;
};

#endif