# include "../Response.hpp"

class Response;

CGIManager::CGIManager(const CGIManager &src)
{
    this->fd[0] = src.fd[0];
    this->fd[1] = src.fd[1];
    this->env = src.env;
    this->execveArgs = src.execveArgs;
    this->execveEnv = src.execveEnv;
    this->pid = src.pid;
    this->isExecuted = src.isExecuted;
    this->status = src.status;
    this->inputFd = src.inputFd;
    this->outputFd = src.outputFd;
}

CGIManager::CGIManager()
{
    this->fd[0] = -1;
    this->fd[1] = -1;
    this->pid = -1;
    this->isExecuted = false;
    this->status = NONE;
    this->inputFd = -1;
    this->outputFd = -1;
    this->execveArgs = NULL;
    this->execveEnv = NULL;
}

void CGIManager::init(Response &resp)
{
    try
    {
        if (!fileExists(resp.resourceFullPath.c_str()))
            resp.serveERROR(_CS_404, _CS_404_m);
        setEnv(resp);
        setExecveArgs(resp);
        setExecveEnv();
    }
    catch (const std::exception &e)
    {
        throw CGI_exception(e.what());
    }
}

CGIManager::~CGIManager()
{
    if (this->fd[0] != -1)
        close(this->fd[0]);
    if (this->fd[1] != -1)
        close(this->fd[1]);
    if (this->inputFd != -1)
        close(this->inputFd);
    if (this->outputFd != -1)
        close(this->outputFd);
    if (this->execveArgs != NULL)
        delete[] this->execveArgs;
    if (this->execveEnv != NULL)
        delete[] this->execveEnv;
}

void Response::serveCGI()
{
    try
    {
        std::cout << COLOR_YELLOW << "serveCGI()" << COLOR_RESET << std::endl;
        if (cgi.isExecuted == false)
            cgi.init(*this);
        cgi.execute(*this);
    }
    catch (const std::exception &e)
    {
        throw Response_err(e.what());
    }
}