#include "../../MainInc/main.hpp"
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
    if (fileExists(this->cleanURI.c_str()) == false)
        this->resp.serveERROR(_CS_404, _CS_404_m);
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
        this->resp.serveERROR(_CS_501, _CS_501_m);
}

void    CGIManager::setInterpreter()
{
    std::map<std::string, std::vector<std::string> >::iterator directive = resp.kwargs.find("cgi-bin");
    std::vector<std::string> directiveValues = directive->second;
    std::vector<std::string>::iterator it = std::find(directiveValues.begin(), directiveValues.end(), this->extension);
    if (it != directiveValues.end())
        this->interpreter = *(it + 1);
    else
        this->resp.serveERROR(_CS_501, _CS_501_m);
}

void    CGIManager::setQueryParams()
{
    this->queryParams = "";
    std::string::size_type pos = this->resp.resourceFullPath.find("?");
    std::cout << "pos: " << pos << std::endl;
    if (pos != std::string::npos)
        this->queryParams = this->resp.resourceFullPath.substr(pos + 1);
}

void    CGIManager::setEnv()
{
    for (int i = 0; resp._env[i]; i++)
        this->env.push_back(resp._env[i]);
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
    if (resp.method == GET)
    {
        this->env.push_back("QUERY_STRING=" + this->queryParams);
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
    for (size_t i = 0; i < this->env.size(); i++)
    {
        this->execveEnv[i] = new char[this->env[i].length() + 1];
        strcpy(this->execveEnv[i], this->env[i].c_str());
    }
    this->execveEnv[this->env.size()] = NULL;
}

void    CGIManager::setInputFd()
{
    this->inputFd = runSystemCall(open(this->resp._req.file_body_name.c_str(), O_RDONLY));
    runSystemCall(dup2(this->inputFd, 0));
    runSystemCall(close(this->inputFd));
}

int    CGIManager::runSystemCall(int returnCode)
{
    if (returnCode == -1)
        throw std::logic_error("System call failed");
    return (returnCode);
}


void    CGIManager::parseOutput()
{
    int rd = -1;
    char buffer[BUFFER_SIZE];
    int  header = 0;
    std::string str, str1;
    size_t line;


    while((rd = runSystemCall(read(this->fd[0], buffer, BUFFER_SIZE - 1))) > 0)
    {
        buffer[rd] = '\0';
        str = std::string(buffer, rd);
        if (!header)
        {
            if ((line = str.rfind("\r\n\r\n")) != std::string::npos)
            {
                str1 = str.substr(line + 4);
                str = str.substr(0 ,line);
            }
        }
        this->resp.body.append(buffer, rd);
        std::cout << COLOR_BLUE << " : " << buffer << COLOR_RESET << std::endl;
        //resp.headers["Content"]
    }
    runSystemCall(close(this->fd[0]));
}

void    CGIManager::execute()
{
    try
    {
        runSystemCall(pipe(this->fd));
        this->pid = runSystemCall(fork());
        if (this->pid == 0)
        {
            runSystemCall(close(this->fd[0]));
            runSystemCall(dup2(this->fd[1], 1));
            runSystemCall(close(this->fd[1]));
            if (this->resp._req.method == POST)
                setInputFd();
            runSystemCall(execve(this->execveArgs[0], this->execveArgs, this->execveEnv));
            exit(EXIT_FAILURE);
        }
        else
        {
            runSystemCall(close(this->fd[1]));
            waitpid(this->pid, 0, 0);
           // parseOutput();
        }
    }
    catch(const std::exception& e)
    {
        this->resp.serveERROR(_CS_500, _CS_500_m);
    }
}
