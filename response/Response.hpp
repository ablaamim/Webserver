#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../MainInc/main.hpp"

class Response
{
    public:
        Response(Request req, int id, configurationSA::location location, char **env); 
        Response(void);
        Response(int id);
        Response(const Response &other);
        ~Response();

        Request                                                 _req;
        configurationSA::location                               _location;
        char                                                    **_env;
        std::ifstream                                           *fs;                                                 
        int                                                     clientSocket;
        size_t                                                  resourceSize; 
        size_t                                                  currentSize;
        size_t                                                  lastChunkSize;
        int                                                     resourceType;

        
        bool                                                    isCompleted;
        bool                                                    isChunked;
        bool                                                    indexChecked;
        bool                                                    isCGI;
        bool                                                    customErrorFound;

        std::string                                             host;
        std::string                                             resourceFullPath;
        std::string                                             httpVersion; 
        std::string                                             body;
        std::string                                             method;
        std::pair   <std::string, std::string>                  status;
        static std::map <std::string, std::string>              mimeTypes;
        std::map    <std::string, std::string>                  headers;
        std::map<std::string, void(Response::*)()>              _methods;
        std::map    <std::string, std::vector<std::string> >    kwargs;

        
        void                                                    init_methods();
        void                                                    print_request();
        void                                                    print_methods();
        void                                                    insert_Location_kwargs(std::string key, std::vector<std::string> value);
        void                                                    print_kwargs();
        void                                                    serveDirectory(Response& resp);
        void                                                    serveRedirect();
        void                                                    list_directories_recursive(std::string& path, std::vector<std::string>& directoryList);
        std::vector<std::string>                                listing_directory(std::string& path);
        void                                                    serveFile(Response& resp);
        void                                                    kwargsInsertion();
       
        
        /* serving client depending on the request method */

        void            serve();        
        void            serveGET();
        void            servePOST();
        void            serveDELETE();
        void            serveCGI();
        void            serveERROR(std::string errorCode, std::string errorMsg);

        /* Initialization methods */

        void            init();
        static void     initMimeTypes();

        /* Methods to check the request and the resource */

        void            checkRequest();
        void            setResourceInfo();
        void            checkResource();
        int             getResourceType();
    
        /* Custom Send, which send the reponse whether the body content is generated or not */

        void            sendResponse(int mode);

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

/* Utils */

std::string                                                     getContentType(std::string path);
bool                                                            isDirectory(std::string path);
bool                                                            fileExists(const char *path);
std::string                                                     pathJoin(std::string path1, std::string path2);
void                                                            lookForIndex(Response &resp);

#endif