# include "../Response.hpp"

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
    this->env.push_back("HTTP_COOKIE=" + getRequestParam("Cookie", resp));
    this->env.push_back("REMOTE_ADDR=" + resp.ip);
    std::string remote_port = resp._req.params["Host"].substr(resp._req.params["Host"].find(":") + 1);
    this->env.push_back("REMOTE_PORT=" + remote_port);
    this->env.push_back("SERVER_SOFTWARE=" + getRequestParam("Server", resp));
    this->env.push_back("SERVER_NAME=" + resp.ip);
    this->env.push_back("REDIRECT_STATUS=200");
    this->env.push_back("SCRIPT_FILENAME=" + resp.resourceFullPath);
    this->env.push_back("SERVER_PORT=" + resp.port);
    this->env.push_back("GATEWAY_INTERFACE=CGI-DIALNA");
    this->env.push_back("HTTP_ACCEPT=" + getRequestParam("Accept", resp));
    this->env.push_back("HTTP_CONNECTION=" + getRequestParam("Connection", resp));
    this->env.push_back("HTTP_ACCEPT_ENCODING=" + getRequestParam("Accept-Encoding", resp));
    this->env.push_back("HTTP_ACCEPT_LANGUAGE=" + getRequestParam("Accept-Language", resp));
    this->env.push_back("HTTP_HOST=" + getRequestParam("Host", resp));
    this->env.push_back("HTTP_REFERER=" + getRequestParam("Referer", resp));
    this->env.push_back("QUERY_STRING=" + resp.queryParams);
    if (resp.method == POST)
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


void CGIManager::parseOutput(Response &resp)
{
    int rd = -1;
    char buffer[CHUNCK_SIZE];
    std::string str;
    rd = runSystemCall(read(this->fd[0], buffer, CHUNCK_SIZE));
    resp.body.append(buffer, rd);
    if (rd == 0)
    {
        resp.isCompleted = true;
        runSystemCall(close(this->fd[0]));
    }
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
                {
                    std::string contentLength = getRequestParam("Content-Length", resp);
                    if (!contentLength.empty())
                    {
                        int contentLengthInt = std::stoi(contentLength);
                        if (contentLengthInt > 0)
                            setInputFd(resp);
                    }
                }
                if (execve(this->execveArgs[0], this->execveArgs, this->execveEnv) == -1)
                    exit(EXIT_FAILURE);
            }
            runSystemCall(close(this->fd[1]));
        }
        /* If WNOHANG was given, and if there is at least one process (usually a child) whose status information is not available, waitpid() returns 0. */
        sleep(1);
        if (waitpid(this->pid, &this->status, WNOHANG))
        {
            if (WIFEXITED(this->status))
            {
                if (WEXITSTATUS(this->status) == EXIT_FAILURE)
                    resp.serveERROR(_CS_500, _CS_500_m);
            }
            parseOutput(resp);
            resp.sendCGIResponse();
        }
        if (this->pid != -1)
        {
            kill (this->pid, SIGKILL);
            resp.serveERROR(_CS_504, _CS_504_m);
        }
        
        /* else, No response will be sent for now, maybe later when write event of this client get triggered again */
    }
    catch (const std::exception &e)
    {
        resp.serveERROR(_CS_500, _CS_500_m);
    }
}
