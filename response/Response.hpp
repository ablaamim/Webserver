#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../MainInc/main.hpp"
#include <dirent.h>
#include "../abstract_req/abstract_req.hpp"

# define CHUNK_SIZE 1024
# define NONE -1
# define FILE 0
# define DIRECTORY 1
# define CGI 2
# define REDIRECT 3
# define NOT_FOUND 4
# define FORBIDDEN 5

class Response
{
    public:

        Response(abstract_req req, int id, configurationSA::location location, std::string _client_ip, char **env) : _req(req), clientSocket(id) ,_location(location), _client_ip(_client_ip) , _env(env)
        {
            // init method map
            if (!this->_methods.empty())
                this->_methods.clear();
            this->_methods.insert(std::pair<std::string, void(Response::*)()>("GET", &Response::handleGet));
            this->_methods.insert(std::pair<std::string, void(Response::*)()>("POST", &Response::handlePost));
            this->_methods.insert(std::pair<std::string, void(Response::*)()>("DELETE", &Response::handleDelete));
            //std::cout << this->_client_ip << std::endl;
            //std::cout << "Webserv constructor" << std::endl;
        };
        Response(void)
        {
            //std::cout << "Webserv default constructor" << std::endl;
        };
        Response(int id);
        Response(const Response &other);
        ~Response();

        abstract_req                                            _req;         // this is the request object that will be used to create the response
        configurationSA::location                               _location;    // this is the location object that will be used to create the response
        std::string                                             _client_ip;   // this is the client ip that will be used to create the response
        char                                                    **_env;


        std::map<std::string, void(Response::*)()>              _methods;     // this is the map that will be used to call the right method

        void print_methods()
        {
            std::map<std::string, void(Response::*)()>::iterator it = this->_methods.begin();
            while (it != this->_methods.end())
            {
                std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
                it++;
            }
        }

        std::map    <std::string, std::vector<std::string> >    kwargs;

        void insert_Location_kwargs(std::string key, std::vector<std::string> value)
        {
            this->kwargs.insert(std::pair<std::string, std::vector<std::string> >(key, value));
        }

        void print_kwargs()
        {
            std::map<std::string, std::vector<std::string> >::iterator it = this->kwargs.begin();
            while (it != this->kwargs.end())
            {
                std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET;
                for (size_t i = 0; i < it->second.size(); i++)
                    std::cout << COLOR_YELLOW << it->second[i] << " " << COLOR_RESET;
                std::cout << std::endl;
                it++;
            }
        }
                                                          
        
        int                                                     clientSocket;
        bool                                                    isCompleted = false; // true if the response is completed
        int                                                     resouceLength; 
        int                                                     currentLength; 
        int                                                     resourceType; 

        std::string                                             resourceFullPath = "/www";
        std::string                                             httpVersion; 
        std::string                                             body; 

        std::pair   <std::string, std::string>                  status; 
        std::map    <std::string, std::string>                  headers;

        void constructData()
        {
            // CONSTRUCT DATA HERE
        }
        
        //void    generate();
        void    serve();

        void    handleGet();
        void    handlePost();
        void    handleDelete();

        void    init();

        class  Response_err : public std::exception
        {
            public :
                Response_err(std::string msg) : _msg(msg) {}
                virtual ~Response_err() throw() {}
                virtual const char* what() const throw() { return _msg.c_str(); }
            private :
                std::string _msg;
        };
};


#endif