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

        Response(abstract_req req, int id, configurationSA::location location, std::string _client_ip, char **env) : _req(req), _location(location), _client_ip(_client_ip) , _env(env)
        {
            //std::cout << this->_client_ip << std::endl;
            //std::cout << "Webserv constructor" << std::endl;
        };
        Response(int id);
        Response(const Response &other);
        ~Response();

        abstract_req              _req;         // this is the request object that will be used to create the response
        configurationSA::location _location;    // this is the location object that will be used to create the response
        std::string               _client_ip;   // this is the client ip that will be used to create the response
        char                      **_env;                                                          
        
        int                                                     clientSocket;
        bool                                                    isCompleted; // true if the response is completed
        int                                                     resouceLength; 
        int                                                     currentLength; 
        int                                                     resourceType; 

        std::string                                             resourceFullPath; 
        std::string                                             httpVersion; 
        std::string                                             body; 

        std::pair   <std::string, std::string>                  status; 
        std::map    <std::string, std::string>                  headers;
        std::map    <std::string, std::vector<std::string> >    kwargs;
        
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