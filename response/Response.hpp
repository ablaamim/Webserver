#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../MainInc/main.hpp"

class Response;

class CGIManager
{
public:
    int fd[2];
    int inputFd;
    int outputFd;
    int pid;
    int isExecuted;
    int status;
    std::vector<std::string> env;
    char **execveArgs;
    char **execveEnv;

    CGIManager(CGIManager const &src);
    CGIManager();

    void init(Response &resp);
    void execute(Response &resp);
    void parseOutput(Response &resp);
    void parseHeader(std::string str, Response &resp);
    void setEnv(Response &resp);
    void setExecveArgs(Response &resp);
    void setExecveEnv();
    std::string getRequestParam(std::string key, Response &resp);
    void setInputFd(Response &resp);
    int runSystemCall(int returnCode);

    ~CGIManager();

    class CGI_exception : public std::exception
    {
    private:
        std::string _msg;

    public:
        CGI_exception(std::string msg) : _msg(msg) {}
        CGI_exception() : _msg("CGI exception") {}
        virtual ~CGI_exception() throw() {}
        virtual const char *what() const throw() { return _msg.c_str(); }
    };
};

class Response
{
    public:
        Response(std::string clientIP, std::string clientPort, Request req, int id, configurationSA::location &location, char **env); 
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
        std::string                                             port;
        std::string                                             ip;
        
        /* cgi  */

        CGIManager                                              cgi;
        bool                                                    isCGI;
        std::string                                             fileExtension;
        std::string                                             cgiInterpreter;


        bool                                                    isCompleted;
        bool                                                    isChunked;
        bool                                                    indexChecked;
        
        bool                                                    customErrorFound;

        std::string                                             cleanPath;
        std::string                                             queryParams;
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
        void            sendCGIResponse();

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
std::string                                                     getFileExtension(const std::string path);
std::string                                                     getInterpreter(Response &resp, const std::string &fileExtension);
std::string                                                     extractQueryParams(std::string &path);
bool                                                            needsRedirection(Response& resp);
std::string                                                     getTodayDate();
void                                                            setQueryParams(Response& resp);
void                                                            setCleanPath(Response& resp);
void                                                            lookForCGI(Response& resp);

#endif