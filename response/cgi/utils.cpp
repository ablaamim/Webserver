#include "../../MainInc/main.hpp"
#include "CGIManager.hpp"

std::string CGIManager::getRequestParam(std::string key, Response &resp)
{
    std::map<std::string, std::string>::iterator it = resp._req.params.find(key);
    if (it != resp._req.params.end())
        return it->second;
    return "";
}


void CGIManager::setEnv(Response &resp)
{
    for (int i = 0; resp._env[i]; i++)
        this->env.push_back(resp._env[i]);
    this->env.push_back("REQUEST_METHOD=" + resp.method);
    this->env.push_back("PATH_INFO=" + resp.resourceFullPath);
    this->env.push_back("HTTP_USER_AGENT=" + getRequestParam("User-Agent", resp));
    this->env.push_back("SERVER_PROTOCOL=" + resp.httpVersion);
    this->env.push_back("HTTP_COOKIE=");
    this->env.push_back("REMOTE_ADDR=" + resp.ip); // CLIENT IP
    std::string remote_port = resp._req.params["Host"].substr(resp._req.params["Host"].find(":") + 1);
    this->env.push_back("REMOTE_PORT=" + remote_port); // CLIENT PORT
    this->env.push_back("SERVER_SOFTWARE=" + getRequestParam("Server", resp));
    this->env.push_back("SERVER_NAME=" + resp.ip);
    this->env.push_back("SERVER_PORT=" + resp.port);
    this->env.push_back("GATEWAY_INTERFACE=CGI-DIALNA");
    this->env.push_back("REDIRECT_STATUS=200");
    this->env.push_back("HTTP_ACCEPT=" + getRequestParam("Accept", resp));
    this->env.push_back("HTTP_CONNECTION=" + getRequestParam("Connection", resp));
    this->env.push_back("SCRIPT_NAME=" + getRequestParam("Url", resp));
    this->env.push_back("HTTP_ACCEPT_ENCODING=" + getRequestParam("Accept-Encoding", resp));
    if (resp.method == GET)
    {
        this->env.push_back("QUERY_STRING=" + resp.queryParams);
    }
    else if (resp.method == POST)
    {
        this->env.push_back("CONTENT_LENGTH=" + std::to_string(resp._req.content_length));
        this->env.push_back("CONTENT_TYPE=" + getRequestParam("Content-Type", resp));
    }
}

void CGIManager::setExecveArgs(Response &resp)
{
    this->execveArgs = new char *[3];
    this->execveArgs[0] = new char[resp.cgiInterpreter.length() + 1];
    this->execveArgs[1] = new char[resp.resourceFullPath.length() + 1];
    this->execveArgs[2] = NULL;
    strcpy(this->execveArgs[0], resp.cgiInterpreter.c_str());
    strcpy(this->execveArgs[1], resp.resourceFullPath.c_str());
}

void CGIManager::setExecveEnv()
{
    this->execveEnv = new char *[this->env.size() + 1];
    for (size_t i = 0; i < this->env.size(); i++)
    {
        this->execveEnv[i] = new char[this->env[i].length() + 1];
        strcpy(this->execveEnv[i], this->env[i].c_str());
    }
    this->execveEnv[this->env.size()] = NULL;
}

void CGIManager::setInputFd(Response &resp)
{
    this->inputFd = runSystemCall(open(resp._req.file_body_name.c_str(), O_RDONLY));
    runSystemCall(dup2(this->inputFd, 0));
    runSystemCall(close(this->inputFd));
}

int CGIManager::runSystemCall(int returnCode)
{
    if (returnCode == -1)
        throw CGI_exception("System call failed");
    return (returnCode);
}

void CGIManager::parseHeader(std::string str, Response &resp)
{
    size_t line, indx;
    std::string str1;
    std::string del = "\r\n";

    while ((line = str.find(del)) != std::string::npos)
    {
        str1 = str.substr(0, line);
        indx = str1.find(": ");
        if (indx != std::string::npos)
            resp.headers[str1.substr(0, indx)] = str1.substr(indx + del.length());
        str = str.substr(line + del.length());
    }
    if (str != "")
    {
        indx = str.find(": ");
        if (indx != std::string::npos)
            resp.headers[str.substr(0, indx)] = str.substr(indx + del.length());
    }
    // for (std::map<std::string, std::string>::iterator it = resp.headers.begin(); it != resp.headers.end(); ++it)
    //     std::cout << COLOR_GREEN << it->first << "  :  " << it->second << COLOR_RESET << std::endl;
}

void CGIManager::parseOutput(Response &resp)
{
    int rd = -1;
    char buffer[BUFFER_SIZE];
    std::string str;
    size_t line;
    std::string del = "\r\n\r\n";

    while ((rd = runSystemCall(read(this->fd[0], buffer, BUFFER_SIZE - 1))) > 0)
    {
        buffer[rd] = '\0';
        str = std::string(buffer, rd);
        line = str.rfind(del);
        if (line != std::string::npos)
        {
            // std::cout << "Parse cgi header her" << std::endl;
            parseHeader(str.substr(0, line), resp);
            str = str.substr(line + del.length());
        }
        resp.body.append(str);
        // std::cout << COLOR_BLUE << str.length() << " cgi output : " << str << COLOR_RESET << std::endl;
    }
    resp.headers["Content-Length"] = std::to_string(resp.body.length());
    runSystemCall(close(this->fd[0]));
}

void CGIManager::execute(Response &resp)
{
    try
    {
        if (this->isExecuted == false)
        {
            runSystemCall(pipe(this->fd));
            this->pid = runSystemCall(fork());
            this->isExecuted = true;
            if (this->pid == 0)
            {
                runSystemCall(close(this->fd[0]));
                runSystemCall(dup2(this->fd[1], 1));
                runSystemCall(close(this->fd[1]));
                if (resp._req.method == POST)
                    setInputFd(resp);
                if (execve(this->execveArgs[0], this->execveArgs, this->execveEnv) == -1)
                {
                    exit(EXIT_FAILURE);
                    throw CGI_exception("Execve failed");
                }
            }
            runSystemCall(close(this->fd[1]));
            sleep(1);
        }
        /* If WNOHANG was given, and if there is at least one process (usually a child) whose status information is not available, waitpid() returns 0. */
        if (waitpid(this->pid, &this->status, WNOHANG) != 0)
        {
            parseOutput(resp);
            resp.sendResponse(FULL);
        }
        /* else, No response will be sent for now, maybe later when write event of this client get triggered again */
    }
    catch (const std::exception &e)
    {
        resp.serveERROR(_CS_500, _CS_500_m);
    }
}
