#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../MainInc/main.hpp"

# define NONE -1
# define FILE 0
# define DIRECTORY 1
# define CGI 2
# define REDIRECT 3
# define NOT_FOUND 4
# define FORBIDDEN 5

# define CHUNCK_SIZE 1024

class Response
{
    public:

        Response(Request req, int id, configurationSA::location location, char **env); 
        Response(void);
        Response(int id);
        Response(const Response &other);
        ~Response();

        Request                                                 _req;         // this is the request object that will be used to create the response
        configurationSA::location                               _location;    // this is the location object that will be used to create the response
        char                                                    **_env;
        std::ifstream                                           fs; // this is the file descriptor that will be used to read / track the file                                                  
        int                                                     clientSocket;
        size_t                                                  resourceSize; 
        int                                                     currentSize;
        int                                                     lastChunkSize;
        int                                                     resourceType; 
        bool                                                    isCompleted;
        bool                                                    isChunked;
        std::string                                             host;
        std::string                                             resourceFullPath;
        std::string                                             httpVersion; 
        std::string                                             body;
        std::string                                             method;
        std::pair   <std::string, std::string>                  status; 
        std::map    <std::string, std::string>                  headers;
        std::map<std::string, void(Response::*)()>              _methods;     // this is the map that will be used to call the right method
        std::map    <std::string, std::vector<std::string> >    kwargs;
        const static std::string	_css;
        
        void    init_methods();
        void    print_request();
        void    print_methods();
        void    insert_Location_kwargs(std::string key, std::vector<std::string> value);
        void    print_kwargs();
        bool    isDirectory(std::string path);
        int     getResourceType(std::string path, std::map<std::string, std::vector<std::string> > kwargs);
        /* serving methods */
        void    serve();
        void    serveFile();
        void    serveEmpty();
        /* methods */
        void    handleGet();
        void    handlePost();
        void    handleDelete();
        /* Initialization methods */
        void    init();
        void    openFile();
        void    checkRequest();
        void    setResourceInfo();
        void    checkResource();
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