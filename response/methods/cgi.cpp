#include "../Response.hpp"
#include <stdlib.h>

char    **getEnvs(Response& resp)
{
    char **env = NULL;
    std::vector<std::string> envs;

    resp._req.print_params();
    
    std::cout << std::endl << std::endl << std::endl;
    
    for (int i = 0; resp._env[i]; i++)
        envs.push_back(resp._env[i]);
    
    envs.push_back("REQUEST_METHOD=" + resp.method);
    envs.push_back("PATH_INFO=" + resp.resourceFullPath);
    envs.push_back("HTTP_USER_AGENT=" + resp._req.params["User-Agent"]);
    envs.push_back("SERVER_PROTOCOL=" + resp.httpVersion);
    envs.push_back("HTTP_COOKIE=");
    envs.push_back("REMOTE_ADDR=" + resp.ip); // CLIENT IP
    std::string remote_port = resp._req.params["Host"].substr(resp._req.params["Host"].find(":") + 1);
    //std::cout << remote_port << std::endl;
    envs.push_back("REMOTE_PORT=" + remote_port); // CLIENT PORT
    envs.push_back("SERVER_SOFTWARE=" + resp.headers["Server"]);
    //std::cout << "IP AND PORT" << resp.ip << " " << resp.port << std::endl << std::endl;
    envs.push_back("SERVER_NAME=" + resp.ip);
    envs.push_back("SERVER_PORT=" + resp.port);
    envs.push_back("GATEWAY_INTERFACE=CGI-DIALNA");
    envs.push_back("REDIRECT_STATUS=200");
    envs.push_back("HTTP_ACCEPT=" + resp._req.params["Accept"]);
    envs.push_back("HTTP_CONNECTION=" + resp._req.params["Connection"]);
    envs.push_back("SCRIPT_NAME=" + resp._req.params["Url"]);
    envs.push_back("HTTP_ACCEPT_ENCODING=" + resp._req.params["Accept-Encoding"]);
    
    if (resp.method == GET)
    {
        envs.push_back("QUERY_STRING=" + extractQueryParams(resp.resourceFullPath));
    }
    
    else if (resp.method == POST)
    {
        envs.push_back("CONTENT_LENGTH=" + std::to_string(resp._req.content_length));
        //std::cout << "CONTENT TYPE ./We   = " << resp._req.content_type << std::endl;
        envs.push_back("CONTENT_TYPE=" + resp._req.params["Content-Type"]);
    }
    
    env = new char*[envs.size() + 1];
    
    for (size_t i = 0; i < envs.size(); i++)
    {
        env[i] = new char[envs[i].length() + 1];
        strcpy(env[i], envs[i].c_str());
    }
    env[envs.size()] = NULL;
    return env;
}

void    parseCGIoutput(Response& resp, int outputFd)
{
    close(outputFd);
}

char    **getFullExecutable(const std::string& interpreter, std::string& resourceFullPath)
{
    char **fullExecutable = new char*[3];
 
    fullExecutable[0] = new char[interpreter.length() + 1];
    fullExecutable[1] = new char[resourceFullPath.length() + 1];
    fullExecutable[2] = NULL;
    strcpy(fullExecutable[0], interpreter.c_str());
    strcpy(fullExecutable[1], resourceFullPath.c_str());
    return fullExecutable;
}

void    executeCGI(Response &resp, const std::string& interpreter)
{
    int fd[2];
    int tmpFD;
    int childPID;
    char    **fullExecutable;
    childPID = fork();
    if (childPID == -1)
        resp.serveERROR("500", "Internal Server Error");
    else if (childPID == 0)
    {
        char **env = getEnvs(resp);
        for (int i = 0; env[i]; i++)
            std::cout << env[i] << std::endl;
        //std::cout << "interpreter: " << interpreter << std::endl;
        dup2(fd[1], 1);
        close(fd[1]);
        close(fd[0]);
        if (resp.method == POST)
        {
            //std::cout << "Response file body name = " << resp._req.file_body_name << std::endl;
            tmpFD = open(resp._req.file_body_name.c_str(), O_RDONLY);
            // if (tmpFD == -1)
            // {
            //     std::cout << COLOR_YELLOW << "open() failed" << COLOR_RESET << std::endl;
            //     resp.serveERROR("500", "Internal Server Error");
            // }
            dup2(tmpFD, 0);
            close(tmpFD);
        }
        fullExecutable = getFullExecutable(interpreter, resp.resourceFullPath);
        if (execve(interpreter.c_str(), fullExecutable, resp._env) == -1)
        {
            std::cout << COLOR_YELLOW << "execve() failed" << COLOR_RESET << std::endl;
            exit(1);
        }
    }
    waitpid(childPID, NULL, 0);
    parseCGIoutput(resp, fd[0]);
    resp.sendResponse(HEADERS_ONLY);
}

void    Response::serveCGI()
{
    try 
    {
        std::string fileExtension = getFileExtension(this->resourceFullPath);
        std::string interpreter;
        //std::cout << "fileExtension: " << fileExtension << std::endl;
        if (fileExtension != ".php" && fileExtension != ".py" && fileExtension != ".sh")
        {
            std::cout << "error" << std::endl;
            this->serveERROR("501", "Not Implemented");
        }
        else
        {
            interpreter = getInterpreter(*this, fileExtension);
            //std::cout << "interpreter: " << interpreter << std::endl;
            if (interpreter.empty())
                this->serveERROR("501", "Not Implemented");
            else
                executeCGI(*this, interpreter);
        }
    }
    catch (std::exception& e)
    {
        //std::cout << "exception: " << e.what() << std::endl;
        throw Response::Response_err(e.what());
    }
}