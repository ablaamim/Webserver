#include "Response.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>
#include <iomanip>

std::map<std::string, std::string>  mime_types;

std::pair<std::string, std::string> _mime_types[] =
{
    std::make_pair(".mp4", "video/mp4"),
    std::make_pair(".aac", "audio/aac"),
	std::make_pair(".abw", "application/x-abiword"),
	std::make_pair(".arc", "application/octet-stream"),
	std::make_pair(".avo", "video/x-msvideo"),
	std::make_pair(".bin", "application/octet-stream"),
	std::make_pair(".bmp", "image/bmp"),
	std::make_pair(".bz", "application/x-bzip"),
	std::make_pair(".bz2", "application/x-bzip2"),
	std::make_pair(".csh", "application/x-csh"),
	std::make_pair(".css", "text/css"),
	std::make_pair(".csv", "text/csv"),
	std::make_pair(".doc", "application/msword"),
	std::make_pair(".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"),
	std::make_pair(".eot", "application/vnd.ms-fontobject"),
	std::make_pair(".epub", "application/epub+zip"),
	std::make_pair(".gif", "image/gif"),
	std::make_pair(".htm", "text/html"),
	std::make_pair(".html", "text/html"),
	std::make_pair(".ico", "image/x-icon"),
	std::make_pair(".ics", "text/calendar"),
	std::make_pair(".jar", "application/java-archive"),
	std::make_pair(".jpeg", "image/jpeg"),
	std::make_pair(".jpg", "image/jpeg"),
	std::make_pair(".js", "application/javascript"),
	std::make_pair(".json", "application/json"),
	std::make_pair(".mid", "audio/midi"),
	std::make_pair(".midi", "audio/midi"),
	std::make_pair(".mpeg", "video/mpeg"),
	std::make_pair(".mpkg", "application/vnd.apple.installer+xml"),
	std::make_pair(".odp", "application/vnd.oasis.opendocument.presentationa"),
	std::make_pair(".ods", "application/vnd.oasis.opendocument.spreadsheet"),
	std::make_pair(".odt", "application/vnd.oasis.opendocument.text"),
	std::make_pair(".oga", "audio/ogg"),
	std::make_pair(".ogv", "video/ogg"),
	std::make_pair(".ogx", "application/ogg"),
	std::make_pair(".otf", "font/otf"),
	std::make_pair(".png", "image/png"),
	std::make_pair(".pdf", "application/pdf"),
	std::make_pair(".ppt", "application/vnd.ms-powerpoint"),
	std::make_pair(".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"),
	std::make_pair(".rar", "application/x-rar-compressed"),
	std::make_pair(".rtf", "application/rtf"),
	std::make_pair(".sh", "application/x-sh"),
	std::make_pair(".svg", "image/svg+xml"),
	std::make_pair(".swf", "application/x-shockwave-flash"),
	std::make_pair(".tar", "application/x-tar"),
	std::make_pair(".tif", "image/tiff"),
	std::make_pair(".tiff", "image/tiff"),
	std::make_pair(".ts", "application/typescript"),
	std::make_pair(".ttf", "font/ttf"),
	std::make_pair(".vsd", "application/vnd.visio"),
	std::make_pair(".wav", "audio/x-wav"),
	std::make_pair(".weba", "audio/webm"),
	std::make_pair(".webm", "video/webm"),
	std::make_pair(".webp", "image/webp"),
	std::make_pair(".woff", "font/woff"),
	std::make_pair(".woff2", "font/woff2"),
	std::make_pair(".xhtml", "application/xhtml+xml"),
	std::make_pair(".xls", "application/vnd.ms-excel"),
	std::make_pair(".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"),
	std::make_pair(".xml", "application/xml"),
	std::make_pair(".xul", "application/vnd.mozilla.xul+xml"),
	std::make_pair(".zip", "application/zip"),
	std::make_pair(".3gp", "video/3gpp"),
	std::make_pair(".3g2", "video/3gpp2"),
	std::make_pair(".7z", "application/x-7z-compressed")
};

Response::Response(void) { /* DEFAULT CONSTRUCTOR */ };

void Response::insert_Location_kwargs(std::string key, std::vector<std::string> value)
{
    this->kwargs.insert(std::pair<std::string, std::vector<std::string> >(key, value));
}

void    Response::init()
{
    try
    {
        mime_types.insert(_mime_types, _mime_types + sizeof(_mime_types) / sizeof(_mime_types[0]));
        this->httpVersion = this->_req.version;
        this->status = std::make_pair("200", "OK");
        this->headers["Server"] = "Webserver/1.0";
        this->currentSize = 0;
        this->resourceSize = 0;
        this->lastChunkSize = 0;
        this->isCompleted = false;
        this->isChunked = false;
		this->fs = NULL;
		this->method = this->_req.method;
		//this->print_kwargs();
        this->checkRequest();
        this->setResourceInfo();
    }
    catch(const std::exception& e)
    {
        throw Response_err(e.what());
    }
}

Response::Response(Request req, int id, configurationSA::location location, char **env)
{
	//std::cout << "CONSTRUCTOR" << std::endl;
	this->_req = req;
	this->_location = location;
	this->_env = env;
	this->clientSocket = id;
	//this->kwargs_alloc = new std::map<std::string, std::vector<std::string> >();
}


Response::~Response()
{
	//std::cout << "DESTRUCTOR" << std::endl;
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
    this->clientSocket = other.clientSocket;
    this->resourceSize = other.resourceSize;
    this->method = other.method;
    this->_env = other._env;
    this->_req = other._req;
    this->_location = other._location;
    this->fs = new std::ifstream(other.resourceFullPath,std::ifstream::binary);
}

std::string     getContentType(std::string path)
{
	
	size_t pos = path.find_last_of(".");
	if (pos == std::string::npos)
		return ("application/octet-stream");
	std::map<std::string, std::string>::iterator it = mime_types.find(path.substr(pos));
	if (it != mime_types.end())
		return (it->second);
	return ("application/octet-stream");
}