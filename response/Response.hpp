#ifndef RESPONSE_HPP
#define RESPONSE_HPP

# include "../MainInc/main.hpp"
# include "utils.hpp"

class Response 
{
    // default Response constructor as for now, it will have 2 parameters, (request instance, sockedFd)
    Response();
    ~Response();
    
    /*
        resourceType is the requested resource type; that is a MACRO defined in defines folder (e.g. CGI_SCRIPT, FILE, DIRECTORY etc) 
        once the request.uri (e.g. "/directory/index.php") is matched with the desired location from the config
        the fullPath should be filled along with resourceType.
    */
    // Request      requestObj;
    int             socketFd;
    int             resourceType;
    std::string     fullPath;
    std::string     httpVersion;
    std::string     body;

    // list of status codes and their associated message it should be defined globaly in the main (for later ...)
    std::map<int, std::string>              statusList;

    // response status code + message (e.g. 200, OK), you can get it from reponseStatus object below
    std::pair   <std::string, std::string>  status;

    // response headers
    std::map    <std::string, std::string>  headers;

    // Extra arguments depending on resource type (e.g. in case of DIRECTORY, kwargs should contain autoindex as key and its value as value) 
    std::map    <std::string, std::string>  kwargs;
    
    // convert data members to a response message and write it to the socket
    void    generate();
};


#endif