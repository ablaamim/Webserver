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
	this->_req = req;
	this->_env = env;
	this->clientSocket = id;
	this->ip = clientIP;
	this->port = clientPort;
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
	this->cleanPath = other.cleanPath;
	this->queryParams = other.queryParams;
	this->clientSocket = other.clientSocket;
    this->resourceSize = other.resourceSize;
    this->method = other.method;
    this->_env = other._env;
    this->_req = other._req;
	this->kwargs = other.kwargs;
	this->isCGI = other.isCGI;
	this->fileExtension = other.fileExtension;
	this->cgiInterpreter = other.cgiInterpreter;
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
