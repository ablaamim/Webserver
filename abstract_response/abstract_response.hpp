#ifndef ABSTRACT_RESPONSE_HPP
# define ABSTRACT_RESPONSE_HPP

#include "../MainInc/main.hpp"

class abstract_response
{
    public :
        
        // this constructor is used in Webserv::webserv_evfilt_read in order to create a new abstract_response object
        abstract_response(abstract_req req, configurationSA::location location, std::string _client_ip, char **env) : _req(req), _location(location), _client_ip(_client_ip) , _env(env)
        {
            //std::cout << "Webserv constructor" << std::endl;
        };

        ~abstract_response()
        {
            //std::cout << "Webserv default destructor" << std::endl;
        };

        abstract_req              _req;         // this is the request object that will be used to create the response
        configurationSA::location _location;    // this is the location object that will be used to create the response
        std::string               _client_ip;   // this is the client ip that will be used to create the response
        char                      **_env;       // this is the env that will be used to create the response

        std::vector<char>          data;
        // std::string               status;
        std::string                 header;
        // std::string               code;

        // void construct_data()
        // {
        //     this->header = abstract_req._version + " " + abstract_req._code + " " + abstract_req._status + "\r\n";
        //     std::cout << "header : " << header << std::endl;
        // }

        std::map <std::string, std::string> _headers;

		class response_err : public std::exception
        {
            public :
                response_err(std::string str) : _str(str) {};
                ~response_err() throw() {};
                const char *what() const throw() { return _str.c_str(); };
            private :
                std::string _str;
        };
};

#endif