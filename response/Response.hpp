#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../MainInc/main.hpp"
#include "utils.hpp"


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
        std::ifstream                                           *fs; // this is the file descriptor that will be used to read / track the file                                                  
        int                                                     clientSocket;
        size_t                                                  resourceSize; 
        size_t                                                  currentSize;
        size_t                                                  lastChunkSize;
        int                                                     resourceType; 
        bool                                                    isCompleted;
        bool                                                    isChunked;
        int                                                     referer;
        bool                                                    initialized;
        std::string                                             host;
        std::string                                             resourceFullPath;
        std::string                                             httpVersion; 
        std::string                                             body;
        std::string                                             method;
        std::pair   <std::string, std::string>                  status; 
        std::map    <std::string, std::string>                  headers;
        std::map<std::string, void(Response::*)()>              _methods;     // this is the map that will be used to call the right method
        std::map    <std::string, std::vector<std::string> >    kwargs;
        //std::map    <std::string, std::vector<std::string> >    *kwargs_alloc;

        
        void                     init_methods();
        void                     print_request();
        void                     print_methods();
        void                     insert_Location_kwargs(std::string key, std::vector<std::string> value);
        void                     print_kwargs();
        void                     serveDirectory(Response& resp);
        void                        serveRedirect();
        int                      getResourceType();
        void                     list_directories_recursive(std::string& path, std::vector<std::string>& directoryList);
        std::vector<std::string> listing_directory(std::string& path);
        void                     serveFile(Response& resp);



        /* serving client depending on the request method */

        void    serve(); // this is the main method that will be called to serve the client
        void    serveERROR(std::string errorCode, std::string errorMsg);
        void    serveGET();
        void    servePOST();
        void    serveDELETE();

        /* Initialization methods */

        void    init();


        void    checkRequest();
        void    setResourceInfo();
        void    checkResource();

        /* Custom Send, which send the reponse whether the body content is generated or not */

        void    sendResponse(int mode);

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

std::string     getContentType(std::string path);

#endif