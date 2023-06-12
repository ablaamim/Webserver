#include "../Response.hpp"

#include <stdlib.h>

class CGIManager
{
    int                         fd[2];
    Response                    &resp;
    std::string                 extension;
    std::string                 interpreter;
    std::string                 cleanURI;
    std::string                 queryParams;
    std::vector<std::string>    env;
    char                        **execveArgs;
    char                        **execveEnv;
    
    public:

        CGIManager(CGIManager const &src);
        CGIManager(Response &resp);

        void            init();
        void            execute();
        void            parseOutput();
        void            setEnv();
        void            setExecveArgs();
        void            setInterpreter();
        void            setCleanURI();
        void            setQueryParams();
        void            setExecveEnv();
        void            setExtension();
        std::string     getRequestParam(std::string key);
        ~CGIManager();
};