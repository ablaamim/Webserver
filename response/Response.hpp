#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../MainInc/main.hpp"
#include <dirent.h>
#include "../Request/Request.hpp"
# define NONE -1
# define FILE 0
# define DIRECTORY 1
# define CGI 2
# define REDIRECT 3
# define NOT_FOUND 4
# define FORBIDDEN 5

# define CHUNCK_SIZE 9000096

class Response
{
    public:

        Response(Request req, int id, configurationSA::location location, char **env);
        
        Response(void)
        {
            //std::cout << "Webserv defaul t constructor" << std::endl;
        };
        
        //Response(int id);
        Response(int id);
        Response(const Response &other);
        
        ~Response();

        Request                                                 _req;         // this is the request object that will be used to create the response
        configurationSA::location                               _location;    // this is the location object that will be used to create the response
        std::string                                             _client_ip;   // this is the client ip that will be used to create the response
        char                                                    **_env;
        
        void print_request()
        {
            // print params map from request
            std::cout << COLOR_GREEN << "Request :" << COLOR_RESET << std::endl;
            this->_req.print_params();
        }

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

        int                                                     fd; // this is the file descriptor that will be used to read / track the file                                                  
        int                                                     clientSocket;
        int                                                     resouceLength; 
        int                                                     currentLength;
        int                                                     lastChunkSize;
        int                                                     resourceType; 
        bool                                                    isCompleted = false;
        bool                                                    isChunked = false;
        std::string                                             resourceFullPath = "/www";
        std::string                                             httpVersion; 
        std::string                                             body; 

        std::pair   <std::string, std::string>                  status; 
        std::map    <std::string, std::string>                  headers;

        void    generateBody();
        void    serve();
        void    serve(std::pair<std::string, std::string> status);
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