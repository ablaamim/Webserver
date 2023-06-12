#include "CGIManager.hpp"

std::string CGIManager::getRequestParam(std::string key)
{
    std::map<std::string, std::string>::iterator it = resp._req.params.find(key);
    if (it != resp._req.params.end())
        return it->second;
    return "";
}

void    CGIManager::setCleanURI()
{
    std::string cleanURI = resp.resourceFullPath;
    std::string::size_type pos = cleanURI.find("?");
    if (pos != std::string::npos)
        cleanURI.erase(pos);
    this->cleanURI = cleanURI;
}

void    CGIManager::setExtension()
{
    std::string extension = this->cleanURI;
    std::string::size_type pos = extension.find_last_of(".");
    if (pos != std::string::npos)
        this->extension = extension.substr(pos);
    else
        this->extension = "";
    if (this->extension != ".py" && this->extension != ".php" && this->extension != ".sh")
        this->resp.serveERROR("501", "Not Implemented");
}

void    CGIManager::setInterpreter()
{
    std::map<std::string, std::vector<std::string> >::iterator directive = resp.kwargs.find("cgi-bin");
    std::vector<std::string> directiveValues = directive->second;
    std::vector<std::string>::iterator it = std::find(directiveValues.begin(), directiveValues.end(), this->extension);
    if (it != directiveValues.end())
        this->interpreter = *(it + 1);
    resp.serveERROR("501", "Not Implemented");
}

void    CGIManager::setQueryParams()
{
    this->queryParams = "";
    std::string::size_type pos = this->resp.resourceFullPath.find("?");
    if (pos != std::string::npos)
        this->queryParams = this->resp.resourceFullPath.substr(pos + 1);
}

void    CGIManager::setEnv()
{
    this->env.push_back("REQUEST_METHOD=" + resp.method);
    this->env.push_back("PATH_INFO=" + resp.resourceFullPath);
    this->env.push_back("HTTP_USER_AGENT=" + getRequestParam("User-Agent"));
    this->env.push_back("SERVER_PROTOCOL=" + resp.httpVersion);
    this->env.push_back("HTTP_COOKIE=");
    this->env.push_back("REMOTE_ADDR=" + resp.ip); // CLIENT IP
    std::string remote_port = resp._req.params["Host"].substr(resp._req.params["Host"].find(":") + 1);
    this->env.push_back("REMOTE_PORT=" + remote_port); // CLIENT PORT
    this->env.push_back("SERVER_SOFTWARE=" + getRequestParam("Server"));
    this->env.push_back("SERVER_NAME=" + resp.ip);
    this->env.push_back("SERVER_PORT=" + resp.port);
    this->env.push_back("GATEWAY_INTERFACE=CGI-DIALNA");
    this->env.push_back("REDIRECT_STATUS=200");
    this->env.push_back("HTTP_ACCEPT=" + getRequestParam("Accept"));
    this->env.push_back("HTTP_CONNECTION=" + getRequestParam("Connection"));
    this->env.push_back("SCRIPT_NAME=" + getRequestParam("Url"));
    this->env.push_back("HTTP_ACCEPT_ENCODING=" + getRequestParam("Accept-Encoding"));
    for (int i = 0; resp._env[i]; i++)
        this->env.push_back(resp._env[i]);
    if (resp.method == GET)
    {
        this->env.push_back("QUERY_STRING=" + extractQueryParams(resp.resourceFullPath));
    }
    else if (resp.method == POST)
    {
        this->env.push_back("CONTENT_LENGTH=" + std::to_string(resp._req.content_length));
        // std::cout << "CONTENT TYPE ./We   = " << resp._req.content_type << std::endl;
        this->env.push_back("CONTENT_TYPE=" + getRequestParam("Content-Type"));
    }
}

void    CGIManager::setExecveArgs()
{
    this->execveArgs = new char *[3];
    this->execveArgs[0] = new char[this->interpreter.length() + 1];
    this->execveArgs[1] = new char[this->cleanURI.length() + 1];
    this->execveArgs[2] = NULL;
    strcpy(this->execveArgs[0], this->interpreter.c_str());
    strcpy(this->execveArgs[1], this->cleanURI.c_str());
}

void    CGIManager::setExecveEnv()
{
    this->execveEnv = new char *[this->env.size() + 1];
    for (int i = 0; i < this->env.size(); i++)
    {
        this->execveEnv[i] = new char[this->env[i].length() + 1];
        strcpy(this->execveEnv[i], this->env[i].c_str());
    }
    this->execveEnv[this->env.size()] = NULL;
}

// void executeCGI(Response &resp, const std::string &interpreter)
// {
//     int fd[2];
//     int tmpFD;
//     int childPID;
//     char **fullExecutable;
//     childPID = fork();
//     if (childPID == -1)
//         resp.serveERROR("500", "Internal Server Error");
//     else if (childPID == 0)
//     {
//         char **env = getEnvs(resp);
//         for (int i = 0; env[i]; i++)
//             std::cout << env[i] << std::endl;
//         // std::cout << "interpreter: " << interpreter << std::endl;
//         dup2(fd[1], 1);
//         close(fd[1]);
//         close(fd[0]);
//         if (resp.method == POST)
//         {
//             // std::cout << "Response file body name = " << resp._req.file_body_name << std::endl;
//             tmpFD = open(resp._req.file_body_name.c_str(), O_RDONLY);
//             // if (tmpFD == -1)
//             // {
//             //     std::cout << COLOR_YELLOW << "open() failed" << COLOR_RESET << std::endl;
//             //     resp.serveERROR("500", "Internal Server Error");
//             // }
//             dup2(tmpFD, 0);
//             close(tmpFD);
//         }
//         fullExecutable = getFullExecutable(interpreter, resp.resourceFullPath);
//         if (execve(interpreter.c_str(), fullExecutable, resp._env) == -1)
//         {
//             std::cout << COLOR_YELLOW << "execve() failed" << COLOR_RESET << std::endl;
//             exit(1);
//         }
//     }
//     waitpid(childPID, NULL, 0);
//     parseCGIoutput(resp, fd[0]);
//     resp.sendResponse(HEADERS_ONLY);
// }
