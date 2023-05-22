#include "Response.hpp"

Response::Response(/*Webserv& webserv, struct kevent &curr_event, */configurationSA &config)  /* : _webserv(webserv), _curr_event(curr_event)*/
{
    std::cout << "Response constructor" << std::endl;

    // begin of request attributes:

    this->method = GET;
    this->host_ip = "localhost";
    this->host_port = "8088";
    this->resoucePath = "/";

    // end of request attributes

    configurationSA::data_type conf = config.get_data();

    if (conf.size() == 0)
        throw Response::Response_err("Error: no configuration found");
    
    config.print_data_type();

    std::cout << "\n\n";
    
    
    for (configurationSA::data_type::iterator iterConf = conf.begin(); iterConf != conf.end(); iterConf++)
    {
        for (configurationSA::Server::type_location::iterator iterLocation = iterConf->location.begin(); iterLocation != iterConf->location.end(); iterLocation++)
        {
            std::cout << "location : " << iterLocation->first << std::endl;
        }
    }

    /*
        this function will lookup and match the URI (resource path comming from requestObj) 
        and set the matched resource full path, type, length. 
    */
    
    this->matchResourceWithLocation();
}

 Response::~Response()
{
     std::cout << "Response destructor" << std::endl;
}

void    Response::matchResourceWithLocation()
{
    /*
        LOOP TROUGH LOCATIONS AND FIND THE MATCHED LOCATION FOR THE RESOURCE PATH
        SET THOSE ATTRIBUTES BELOW DEPENDING ON MATCHED LOCATION
        FOR MORE:
        https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms
    */

    this->resourceFullPath = ""; // location root + the resourcePath
    this->resourceType = NONE; // see ./defines/defines.hpp for other possible types
    this->resouceLength = 0; // THE RESOURCE SIZE
    std::vector<std::string> allowed_methods;
    allowed_methods.push_back("GET");
    allowed_methods.push_back("POST"); // THIS IS JUST AN EXAMPLE
    this->kwargs["allowed_methods"] = allowed_methods;
}

void    Response::generate()
{
    /*
        this function will serve the client request (e.g POST, DELETE, GENERATING BODY CONTENT)
        needs request.method, resouceType, resouceLength, resouceFullPath.
    */
    // if (this->method == GET)
    //     this->handleGet();
    // else if (this->method == POST)
    //     this->handlePost();
    // else if (this->method == DELETE)
    //     this->handleDelete();
    // else
    //     this->status = std::make_pair("405", "Method Not Allowed");
    // /* 
    //     we will set isCompleted to true for now,
    //     but its will stay false unless current length >= resourceLength.
    //     usefull for chuncked responses and used for closing/disconnecting connection or not.
    // */
    // this->isCompleted = true;
    // this->send();
}

// void    Response::send()
// {
//     /*
//         This function will convert the response attributes to an http response string
//         and send it, 
//     */
//     std::string responseMessage;
//     std::map<std::string, std::string>::iterator it;
//     responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second + "\r\n";
//     // adding headers
//     for (it = this->headers.begin(); it != this->headers.end(); it++)
//         responseMessage += it->first + ": " + it->second + "\r\n";
//     // adding generated body content
//     responseMessage += "\r\n" + this->body;
//     // this needs to be optimized
//     if (write(this->_curr_event.ident, responseMessage.c_str(), responseMessage.size()) < 0)
//         std::cout << "write error" << std::endl;

//     // we will decide at this point to close / disconnect the client if his request was 100% served
//     if (this->isCompleted)
//     {
//         this->_webserv.delete_event(this->_curr_event.ident, EVFILT_WRITE);
//         this->_webserv.disconnect_client(this->_curr_event.ident, this->_webserv.get_clients());
//         this->_webserv.delete_client(this->_curr_event.ident);
//     }
// }
