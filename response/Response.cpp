#include "Response.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>
#include <iomanip>

void   Response::initialize_mime_types()
{
    std::cout << COLOR_YELLOW << "initialize mime types" << COLOR_RESET <<std::endl;
    
    if (!mime_types.empty())
        return ;
    
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
    mime_types.insert(_mime_types, _mime_types + sizeof(_mime_types) / sizeof(_mime_types[0]));
}

// void    Response::checkRequest()
// {
//     std::vector<std::string> allowedMethods = this->kwargs["allowed_methods"];
//     if (std::find(allowedMethods.begin(), allowedMethods.end(), this->_req.params["Method"]) == allowedMethods.end())
//         this->serve(std::pair<std::string, std::string>("405", "Method Not Allowed"));
//     // if request method == POST
//     if (this->_req.params["Method"] == "POST")
//     {

//     }
// }

void    Response::serve(std::pair<std::string, std::string> status)
{
    std::string responseMessage;
    responseMessage += this->httpVersion + " " + status.first + " " + status.second + "\r\n";
    for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
        responseMessage += it->first + ": " + it->second + "\r\n";
    responseMessage += "\r\n";
    send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
}

std::string intToHexString(int number)
{
    std::stringstream stream;
    stream << std::hex << number;
    return stream.str();
}

size_t getFileSize(const std::string& filePath)
{
    std::ifstream file(filePath.c_str(), std::ios::binary);
    // if (!file)
    // {
    //     // Handle file open error
    //     throw std::runtime_error("Failed to open the file");
    // }
    
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.close();
    
    return static_cast<size_t>(length);
}

Response::Response(Request req, int id, configurationSA::location location, char **env) : _req(req), clientSocket(id) ,_location(location), _env(env)
{
    initialize_mime_types();

    //this->print_mime_types();
    

    //std::cout << _req.params["Method"] << std::endl;
    //std::cout << _req.params["Protocol Version"] << std::endl;

    if (_req.params["Url"].empty())
        _req.params["Url"] = "/slayer.mp4";
    else   
        this->resourceFullPath = _req.params["Url"];
    
    //std::cout << "resourceFullPath: " << this->resourceFullPath << std::endl;
    
    // IF URL HAS A '/' AT THE END SO WE REMOVE IT /// HARDCODE SADLY
    this->resourceFullPath = this->resourceFullPath.substr(1);
    
    this->headers["content-type"] = "video/mp4";
    
    this->resouceLength = getFileSize(this->resourceFullPath);
    std::cout << COLOR_RED <<"resource length: " << this->resouceLength << std::endl;
    
    this->httpVersion = _req.params["Protocol Version"];
    this->method = _req.params["Method"];

    //this->_req.print_params();


    this->lastChunkSize = 0;
    this->isCompleted = false;
    this->body = _req.params[_CONTENT_];

    this->_req.print_params();

    // this->headers.insert(std::pair<std::string, std::string>("Server", "webserv"));
    // this->status = std::pair<std::string, std::string>("200", "OK");
    // this->fd = open(this->resourceFullPath.c_str(), O_RDONLY);
    // std::cout << "fd: " << this->fd << std::endl;
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

void    Response::init()
{
    this->resourceType = getResourceType(this->resourceFullPath, this->kwargs);
    std::cout << "resource type: " << this->resourceType << std::endl;
    this->currentLength = 0;
    this->resouceLength = 0;
}

Response::~Response()
{
     //std::cout << "Response destructor" << std::endl;
}

void    Response::generateBody()
{
    char buf[CHUNCK_SIZE + 1] = {0};
    std::ifstream f;
    f.open(this->resourceFullPath.c_str(), std::ios::in | std::ios::binary);
    f.seekg(this->currentLength, std::ios::beg);
    f.read(buf, CHUNCK_SIZE);
    this->lastChunkSize = f.gcount();
    this->currentLength += this->lastChunkSize;
    this->body = std::string(buf);
    std::cout << "body: " << this->body << std::endl;
}

void    Response::serve()
{
    std::string responseMessage;
    if (!isChunked)
    {
        responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
        std::map<std::string, std::string>::iterator it = this->headers.begin();
        while (it != this->headers.end())
        {
            responseMessage += it->first + ": " + it->second + "\r\n";
            it++;
        }
        if (this->resouceLength >= CHUNCK_SIZE)
            this->isChunked = true;
    }
    generateBody();
    responseMessage += "\r\n";
    responseMessage.append(this->body);
    if (this->currentLength >= this->resouceLength)
    {
        this->isCompleted = true;
        responseMessage += "\r\n";
    }
    send(this->clientSocket, responseMessage.c_str(), responseMessage.length(), 0);
}

Response::Response(const Response &other)
{
    //std::cout << "Response copy constructor" << std::endl;
    // copy all the attributes of the other object to this object
    this->clientSocket = other.clientSocket;
    this->resourceFullPath = other.resourceFullPath;
    this->resouceLength = other.resouceLength;
    this->currentLength = other.currentLength;
    this->lastChunkSize = other.lastChunkSize;
    this->isCompleted = other.isCompleted;
    this->httpVersion = other.httpVersion;
    this->headers = other.headers;
    this->status = other.status;
    this->fd = other.fd;
    this->resourceType = other.resourceType;
    this->body = other.body;
    this->_req = other._req;
    this->_location = other._location;
    this->_client_ip = other._client_ip;
    this->_env = other._env;

}