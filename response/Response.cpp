#include "../MainInc/main.hpp"


std::map<std::string, std::string>  mime_types;                // map of (extension, mime_type)

Response::Response(void)
{};

void Response::init_methods()
{
    if  (this->_methods.empty())
    {
        return ;
    }
    std::pair<std::string, void(Response::*)()> _methods[] =
    {
        std::make_pair("GET", &Response::handleGet),
        std::make_pair("POST", &Response::handlePost),
        std::make_pair("DELETE", &Response::handleDelete),
    };
    this->_methods.insert(_methods, _methods + sizeof(_methods) / sizeof(_methods[0]));
}

void Response::print_request()
{
    std::cout << COLOR_GREEN << "Request :" << COLOR_RESET << std::endl;
    this->_req.print_params();
}

void Response::print_methods()
{
    std::map<std::string, void(Response::*)()>::iterator it = this->_methods.begin();
    while (it != this->_methods.end())
    {
        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
        it++;
    }
}

void Response::print_kwargs()
{
    std::map<std::string, std::vector<std::string> >::iterator it = this->kwargs.begin();
    while (it != this->kwargs.end())
    {
        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET;
        for (size_t i = 0; i < it->second.size(); i++)
        {
            std::cout << COLOR_YELLOW << it->second[i] << " " << COLOR_RESET;
            std::cout << std::endl;
            it++;
        }
    }
}

void Response::insert_Location_kwargs(std::string key, std::vector<std::string> value)
{
    this->kwargs.insert(std::pair<std::string, std::vector<std::string> >(key, value));
}

void print_mime_types()
{
    if (mime_types.empty())
        std::cout << "mime_types is empty" << std::endl;
    else
    {
        std::map<std::string, std::string>::iterator it = mime_types.begin();
        while (it != mime_types.end())
        {
            std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
            it++;
        }
    }
}

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

void    Response::openFile()
{
    /*
        Here, we will try to open the given file.
        If we fail to open given file, we will throw an exception.
        and set the status code of the instance accordingly.
        for example, if we fail to open the file, we will set the status code to 404.
        then, catch it outside of this function and send the response.
        if everything goes well, we will set the size of the file to the instance.
    */
    std::ifstream file(this->resourceFullPath.c_str(), std::ios::binary);
    
    if (file.good())
    {
        file.seekg(0, std::ios::end);
        std::streampos length = file.tellg();
        this->resourceSize = static_cast<size_t>(length);
    }
    else
        this->status = std::make_pair("404", "Not Found");
    //std::cout << "Status: " << this->status.first << std::endl;
    if (this->status.first != "200")
    {
        throw Response_err("File not found");
    }
}

void    Response::checkRequest()
{
    std::vector<std::string> allowedMethods = this->kwargs["allowed_methods"];

    if (_req.method != "GET" && _req.method != "POST" && _req.method != "DELETE")
    {
        this->status = std::make_pair("501", "Not Implemented");        
        throw Response_err("Method not implemented");
    }
    if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_req.params["Method"]) == allowedMethods.end())
    {
        this->status = std::make_pair("405", "Method Not Allowed");
        throw Response_err("Method not allowed");
    }
    if (this->status.first != "200")
        throw Response_err("Method not allowed");
    if (this->_req.version != "HTTP/1.1")
    {
        this->status = std::make_pair("505", "HTTP Version Not Supported");
        throw Response_err("HTTP Version not supported");
    }
    //this->_req.params["Host"] = ""; // for testing purposes
    if (this->_req.params["Host"] != "localhost" || this->_req.params["Host"] == "")
    {
        this->status = std::make_pair("400", "Bad Request");
        throw Response_err("Bad Request");
    }
    //if (this->_req.params[]
}

void    Response::serveEmpty()
{
    std::string responseMessage;
    responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
    for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
        responseMessage += it->first + ": " + it->second + "\r\n";
    responseMessage += "\r\n";
    send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
}

void    Response::setResourceInfo()
{
    this->resourceType = FILE;
    this->resourceFullPath = this->kwargs["root"][0].append(_req.path);    
    this->headers["Content-Type"] = mime_types[this->resourceFullPath.substr(this->resourceFullPath.find_last_of('.'))];
    //this->headers["Content-Length"] = std::to_string(this->resourceSize);
    //std::cout << "CONTENT LENGTH = " << this->headers["Content-Length"] << std::endl;
}  


void    Response::checkResource()
{
    /* try to check the file and set data about it (e.g. ifstream instance, size ... ) */
    try
    {
        if (resourceType == FILE)
            this->openFile();
    }
    catch(const std::exception& e)
    {
        // this will be caught in Webserver.cpp, and serveEmpty() will be called. instead of serve()
        throw Response_err(e.what());
    }
    
    /* try to check the directory */
    // do it here ...
    // else checks in the future
}

void    Response::init()
{
    mime_types.insert(_mime_types, _mime_types + sizeof(_mime_types) / sizeof(_mime_types[0]));
    this->httpVersion = this->_req.version;
    this->status = std::make_pair("200", "OK");
    this->headers["Server"] = "Webserver/1.0";
    this->currentSize = 0;
    this->resourceSize = _req.get_body_size();
    this->lastChunkSize = 0;
    this->isCompleted = false;
    this->isChunked = false;
    
    //std::cout << "RESSOURCE SIZE = " << this->resourceSize << std::endl;

    try
    {
        //std::cout << "CONTENT = " << this->_req.get_content();
        this->checkRequest();
        this->init_methods();
        this->setResourceInfo();
        this->checkResource();
    }
    catch(const std::exception& e)
    {
        // this will be caught in Webserver.cpp, and serveEmpty() will be called. instead of serve()
        throw Response_err(e.what());
    }
}

Response::Response(Request req, int id, configurationSA::location location, char **env) : _req(req), _location(location), _env(env), clientSocket(id)
{
    /*
        Here, we will initialize the response instance.
        if any error occurs, we will throw an exception
        and catch it in (Webserver.cpp) and send serveEmpty() which will serve a response
        that has the actual status code only (no body),
        we will see how to render the body later (error pages)
    */
};


bool Response::isDirectory(std::string path)
{
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else
        return false;
}

int Response::getResourceType(std::string path, std::map<std::string, std::vector<std::string> > kwargs)
{
    if (kwargs.find("cgi-bin") != kwargs.end())
        return CGI;
    else if (kwargs.find("return") != kwargs.end())
        return REDIRECT;
    if (isDirectory(path))
        return DIRECTORY;
    return FILE;
}

Response::~Response()
{
    if (this->fs.is_open())
        this->fs.close();
}

void    Response::serveFile()
{
    std::string responseMessage;
    char buf[CHUNCK_SIZE];
    if (!isChunked)
    {
        responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
        std::map<std::string, std::string>::iterator it = this->headers.begin();
        while (it != this->headers.end())
        {
            responseMessage += it->first + ": " + it->second + "\r\n";
            it++;
        }
        if (this->resourceSize >= CHUNCK_SIZE)
            this->isChunked = true;
        responseMessage += "\r\n";
        send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
    }
    this->fs.seekg(this->currentSize, std::ios::beg);
    this->fs.read(buf, CHUNCK_SIZE);
    this->lastChunkSize = this->fs.gcount();
    this->currentSize += this->lastChunkSize;
    send(this->clientSocket, buf, this->lastChunkSize, 0);
    if (this->currentSize >= static_cast<int>(this->resourceSize))
        this->isCompleted = true;
}

void    Response::serve()
{
    /* we will check for methods, in the future and respond */
    if (this->resourceType == FILE)
        serveFile();
}

Response::Response(const Response &other)
{
    //std::cout << "Response copy constructor" << std::endl;
    // copy all the attributes of the other object to this object
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
    // this->fs = std::ifstream(other.resourceFullPath);

}