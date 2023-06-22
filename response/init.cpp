#include "Response.hpp"

void    Response::init()
{
    try
    {
        this->httpVersion = this->_req.version;
        this->status = std::make_pair(_CS_200, _CS_200_m);
        this->headers["Server"] = "Webserver/1.0";
        this->headers["Content-Type"] = "text/html";
		this->headers["Date"] = getTodayDate();
        this->currentSize = 0;
        this->resourceSize = 0;
        this->lastChunkSize = 0;
        this->isCompleted = false;
        this->isChunked = false;
		this->customErrorFound = false;
		this->indexChecked = false;
		this->fs = NULL;
		this->method = this->_req.method;
		this->isCGI = false;
        this->cgi = CGIManager();
        if (_req.error)
            this->serveERROR(std::to_string(_req.error), _req.erro_msg);
        this->kwargsInsertion();
        setQueryParams(*this);
        setCleanPath(*this);
        this->setResourceInfo();
        if (needsRedirection(*this))
            return ;
        lookForIndex(*this);
        lookForCGI(*this);
        if (this->isCGI)
            this->headers.erase("Content-Type");
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}