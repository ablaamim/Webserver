#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../MainInc/main.hpp"
#include <dirent.h>

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

        Response(int id, configurationSA::location location, char **env);
        Response(int id);
        Response(const Response &other);
        ~Response();
        
        configurationSA::location                               location;

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
        
        void    generate();
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