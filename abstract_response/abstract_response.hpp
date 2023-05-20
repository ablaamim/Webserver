#ifndef ABSTRACT_RESPONSE_HPP
# define ABSTRACT_RESPONSE_HPP

#include "../MainInc/main.hpp"

class abstract_response
{
    public :
        
        // this constructor is used in Webserv::webserv_evfilt_read in order to create a new abstract_response object
        abstract_response(abstract_req req, configurationSA::location location, std::string _client_ip) : _req(req), _location(location), _client_ip(_client_ip)
        {
            //std::cout << "Webserv default constructor" << std::endl;
        };

        ~abstract_response()
        {
            //std::cout << "Webserv default destructor" << std::endl;
        };

        abstract_req              _req;         // this is the request object that will be used to create the response
        configurationSA::location _location;    // this is the location object that will be used to create the response
        std::string               _client_ip;   // this is the client ip that will be used to create the response

		class response_err : public std::exception
        {
            public :
                response_err(std::string str) : _str(str) {};
                ~response_err() throw() {};
                const char *what() const throw() { return _str.c_str(); };
            private :
                std::string _str;
        };

        // this function is used in Webserv::webserv_evfilt_read in order to create a new abstract_response object
        void create_response();

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response();

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response(int client_fd);

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response(int client_fd, std::string str);

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response(int client_fd, std::string str, std::string str2);

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response(int client_fd, std::string str, std::string str2, std::string str3);

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response(int client_fd, std::string str, std::string str2, std::string str3, std::string str4);

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response(int client_fd, std::string str, std::string str2, std::string str3, std::string str4, std::string str5);

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client
        void send_response(int client_fd, std::string str, std::string str2, std::string str3, std::string str4, std::string str5, std::string str6);

        // this function is used in Webserv::webserv_evfilt_write in order to send the response to the client

};

#endif