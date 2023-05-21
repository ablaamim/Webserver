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

        int         _fd;
    
        
        //std::string target;
        
        std::string method = "GET";
        std::string version = "HTTP/1.1";
        size_t max_header_size = 8000;
        
        // std::string host_ip = "127.0.0.1";
        // std::string host_port = "8080";
        // std::string path = "/";
        // size_t max_body_size = 1000000;

};

#endif