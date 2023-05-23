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

        std::string getMethod() const
        {
            return (this->method);
        }

        std::string getVersion() const
        {
            return (this->version);
        }

        std::string getResourceFullPath() const
        {
            return (this->resourceFullPath);
        }

        size_t getMaxBodySize() const
        {
            return (this->max_body_size);
        }

        int         _fd;
    
        private :
        
        //std::string target;
        
        std::string method = "GET";
        std::string version = "HTTP/1.1";
        size_t max_header_size = 8000;
        std::string resourceFullPath = "/www";
        size_t max_body_size = 1000000;

};

#endif