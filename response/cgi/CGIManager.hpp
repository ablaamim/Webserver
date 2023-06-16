#include "../Response.hpp"

#include <stdlib.h>

class CGIManager
{
    int                         fd[2];
    int                         inputFd;
    int                         outputFd;
    int                         pid;
    Response                    &resp;
    std::vector<std::string>    env;
    char                        **execveArgs;
    char                        **execveEnv;
    
    public:

        CGIManager(CGIManager const &src);
        CGIManager(Response &resp);

        void            init();
        void            execute();
        void            parseOutput();
        void            parseHeader(std::string str1);
        void            setEnv();
        void            setExecveArgs();
        void            setExecveEnv();
        std::string     getRequestParam(std::string key);
        void            setInputFd();
        int             runSystemCall(int returnCode);

        ~CGIManager();

    class CGI_exception : public std::exception
    {
        private : 
            std::string _msg;

        public :
            CGI_exception(std::string msg) : _msg(msg) {}
            CGI_exception() : _msg("CGI exception") {}
            virtual ~CGI_exception() throw() {}
            virtual const char* what() const throw() { return _msg.c_str(); }
    };
};