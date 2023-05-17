#include "Response.hpp"

Response::Response(Webserv& webserv, struct kevent& curr_event) : _webserv(webserv), _curr_event(curr_event)
{
    std::cout << "Response constructor" << std::endl;
    
    //this->_location = location;
    
    //this->_location.print_unique_key();

    this->method = GET;
    
    this->isCompleted = false;
    this->httpVersion = "HTTP/1.1";
    this->body = "";
    this->length = 0; // this is the actual length (not the resource length)
    this->status = std::make_pair("200", "OK");
    this->statusList[200] = "OK";
    this->headers["Server"] = "webserv";

    /*
        this function will lookup and match the URI (resource path comming from requestObj) 
        and set the matched resource full path, type, length. 
    */
    this->matchResourceWithLocation();
}

void    Response::matchResourceWithLocation()
{
    /*
    
            SET THOSE ATTRIBUTES BELOW DEPENDING ON MATCHED LOCATION

    */

    this->fullPath = ""; // full path of the resouce
    this->resourceType = NONE; // see ./defines/defines.hpp for other possible types
    this->resouceLength = 0; // the file size for example

    /*
        this is a dynamic map, needs to be filled with other attributes of the matched location
        if resourceType is Directory it should be like this:
        this->kwargs["autoindex"] = "OFF";
        this->kwargs["index"] = "index.php"
        ...
    */
    this->kwargs["autoindex"] = "OFF"; // this is just an example.
    // this->allowedMethods, this should be filled with the matched location allowed methods
}

Response::~Response()
{
    std::cout << "Response destructor" << std::endl;
}

void    Response::generate()
{
    /*
        this function will serve the client request (e.g POST, DELETE, GENERATING BODY CONTENT)
        needs request.method, resouceType, resouceLength, resouceFullPath.
    */
    if (this->method == GET)
        this->handleGet();
    else if (this->method == POST)
        this->handlePost();
    else if (this->method == DELETE)
        this->handleDelete();
    else
        this->status = std::make_pair("405", "Method Not Allowed");
    /* 
        we will set isCompleted to true for now,
        but its will stay false unless current length >= resourceLength.
        usefull for chuncked responses and used for closing/disconnecting connection or not.
    */
    this->isCompleted = true;
    this->send();
}

void    Response::send()
{
    /*
        This function will convert the response attributes to an http response string
        and send it, 
    */
    std::string responseMessage;
    std::map<std::string, std::string>::iterator it;
    responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
    // adding headers
    for (it = this->headers.begin(); it != this->headers.end(); it++)
        responseMessage += it->first + ": " + it->second + "\r\n";
    // adding generated body content
    responseMessage += "\r\n" + this->body;
    // this needs to be optimized
    if (write(this->_curr_event.ident, responseMessage.c_str(), responseMessage.size()) < 0)
        std::cout << "write error" << std::endl;

    // we will decide at this point to close / disconnect the client if his request was 100% served
    if (this->isCompleted)
    {
        this->_webserv.delete_event(this->_curr_event.ident, EVFILT_WRITE);
        this->_webserv.disconnect_client(this->_curr_event.ident, this->_webserv.get_clients());
        this->_webserv.delete_client(this->_curr_event.ident);
    }
}
