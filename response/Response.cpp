#include "../MainInc/main.hpp"


std::string getContentType(std::string path)
{
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos)
		return ("application/octet-stream");
	std::map<std::string, std::string>::iterator it = Response::mimeTypes.find(path.substr(pos));
	if (it != Response::mimeTypes.end())
		return (it->second);
	return ("application/octet-stream");
}

Response::Response(void) { /* DEFAULT CONSTRUCTOR */ };

Response::Response(std::string clientIP, std::string clientPort, Request req, int id, configurationSA::location& location, char **env) : _location(location)
{
	/*That's for now, we will complete initialization in Response::init()*/
	//std::cout << "IP AND PORT = " << clientIP << " " << clientPort << std::endl;
	this->_req = req;
	this->_env = env;
	this->clientSocket = id;
	this->ip = clientIP;
	this->port = clientPort;

}

void    Response::init()
{
    try
    {
        this->httpVersion = this->_req.version;
        this->status = std::make_pair(_CS_200, _CS_200_m);
        this->headers["Server"] = "Webserver/1.0";
		this->headers["Connection"] = "close";
        this->currentSize = 0;
        this->resourceSize = 0;
        this->lastChunkSize = 0;
        this->isCompleted = false;
        this->isChunked = false;
		this->customErrorFound = false;
		this->indexChecked = false;
		this->indexFound = false;
		this->fs = NULL;
		this->method = this->_req.method;
		this->isCGI = false;
		this->kwargsInsertion();
        this->checkRequest();
        this->setResourceInfo();

    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}

Response::~Response()
{
    if (this->fs)
	{
        this->fs->close();
		delete(this->fs);
		this->fs = NULL;
	}
}

Response::Response(const Response &other)
{
    this->httpVersion = other.httpVersion;
    this->status = other.status;
    this->headers = other.headers;
    this->resourceType = other.resourceType;
    this->resourceFullPath = other.resourceFullPath;
    this->resourceSize = other.resourceSize;
    this->currentSize = other.currentSize;
    this->lastChunkSize = other.lastChunkSize;
    this->isCompleted = other.isCompleted;
    this->isChunked = other.isChunked;
	this->customErrorFound = other.customErrorFound;
	this->indexChecked = other.indexChecked;
	this->indexFound = other.indexFound;
	this->clientSocket = other.clientSocket;
    this->resourceSize = other.resourceSize;
    this->method = other.method;
    this->_env = other._env;
    this->_req = other._req;
	this->kwargs = other.kwargs;
	this->isCGI = other.isCGI;
    this->_location = other._location;
    this->fs = other.fs;
	this->ip = other.ip;
	this->port = other.port;
}

void Response::insert_Location_kwargs(std::string key, std::vector<std::string> value)
{
	this->kwargs.insert(std::pair<std::string, std::vector<std::string> >(key, value));
}

void Response::kwargsInsertion()
{
	typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > NoneUniqueKey_t;
	for (NoneUniqueKey_t::iterator it = _location.NoneUniqueKey.begin(); it != _location.NoneUniqueKey.end(); it++)
	{
		std::string key = it->first;
		std::vector<std::string> values;
		std::map<std::string, std::vector<std::string> >::iterator it_map = it->second.begin();
		while (it_map != it->second.end())
		{
			values.push_back(it_map->first);
			std::vector<std::string>::iterator vec_iter = it_map->second.begin();
			while (vec_iter != it_map->second.end())
			{
				values.push_back(*vec_iter);
				vec_iter++;
			}
			it_map++;
		}
		this->kwargs.insert(std::make_pair(key, values));
	}
	for (std::map<std::string, std::vector<std::string> >::iterator it = _location.UniqueKey.begin(); it != _location.UniqueKey.end(); it++)
	{
		std::string key = it->first;
		std::vector<std::string> values = it->second;
		this->kwargs.insert(std::make_pair(key, values));
	}
}
