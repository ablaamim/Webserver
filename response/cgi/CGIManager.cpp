#include "CGIManager.hpp"

CGIManager::CGIManager(const CGIManager& src) : resp(src.resp)
{
    this->fd[0] = src.fd[0];
    this->fd[1] = src.fd[1];
    this->extension = src.extension;
    this->interpreter = src.interpreter;
    this->cleanURI = src.cleanURI;
    this->queryParams = src.queryParams;
    this->env = src.env;
    this->execveArgs = src.execveArgs;
    this->execveEnv = src.execveEnv;
}

CGIManager::CGIManager(Response &resp) : resp(resp)
{
    this->fd[0] = -1;
    this->fd[1] = -1;
}

void    CGIManager::init()
{
    try
    {
        std::cout << COLOR_YELLOW << "init()" << COLOR_RESET << std::endl;
        this->setCleanURI();
        std::cout << "cleanURI: " << cleanURI << std::endl;
        this->setExtension();
        std::cout << "extension: " << extension << std::endl;
        this->setInterpreter();
        std::cout << "interpreter: " << interpreter << std::endl;
        this->setQueryParams();
        std::cout << "queryParams: " << queryParams << std::endl;
        this->setEnv();
        this->setExecveArgs();
        this->setExecveEnv();
    }
    catch(const std::exception& e)
    {
        throw std::logic_error(e.what());
    }
}

CGIManager::~CGIManager()
{
    if (this->fd[0] != -1)
        close(this->fd[0]);
    if (this->fd[1] != -1)
        close(this->fd[1]);
}

void    Response::serveCGI()
{
    try
    {
        std::cout << COLOR_YELLOW << "serveCGI()" << COLOR_RESET << std::endl;
        CGIManager cgi(*this);
        cgi.init();
        cgi.execute();
        this->sendResponse(FULL);
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}