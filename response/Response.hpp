#ifndef RESPONSE_HPP
#define RESPONSE_HPP

# include "utils.hpp"
# include "../parsing/ConfigurationParser.hpp"
# include "../global_interface/Webserv.hpp"
# include "../defines/defines.hpp"
# include "methods/methods.hpp"
# include <map>
# include <string>
# include <iostream>
# include <utility>
# include <vector>
# include <algorithm>
# include <sys/types.h>
# include <sys/event.h>
# include <unistd.h>

//class configurationSA;

class Response : public configurationSA
{
    public :
    
    // default Response constructor as for now, it will have 2 parameters, (request instance, sockedFd)
    
    // this constructor will have no use taking Webserver instance as a parameter!
    Response(/*Webserv& webserv,  const struct kevent &curr_event*/configurationSA &config);
    
    ~Response();
    
    //Webserv&                        _webserv;
    //struct kevent&                  _curr_event;
    
    configurationSA::location       _location;
    configurationSA::Server         _server;

    /*
        resourceType is the requested resource type; that is a MACRO defined in defines folder (e.g. CGI_SCRIPT, FILE, DIRECTORY etc) 
        once the request.uri (e.g. "/directory/index.php") is matched with the desired location from the config
        the fullPath should be filled along with resourceType.
    */
    // Request      requestObj;

    bool            isCompleted; // whether the reponse has served 100% of resourceLength

    int             resouceLength; // e.g. a video file 10mb 
    int             length;
    int             resourceType;
    std::string     method;
    std::string     fullPath;
    std::string     httpVersion;
    std::string     body;


    // set this depending on matched location 
    std::vector<std::string>    allowedMethods;

    // list of status codes and their associated message it should be defined globaly in the main (for later ...)
    std::map<int, std::string>  statusList;

    // response status code + message (e.g. 200, OK), you can get it from reponseStatus object below
    std::pair   <std::string, std::string>  status;

    // response headers
    std::map    <std::string, std::string>  headers;

    // Extra arguments depending on resource type (e.g. in case of DIRECTORY, kwargs should contain autoindex as key and its value as value) 
    
    std::map    <std::string, std::string>  kwargs;
    
    // handle client request
    void    generate();

    // convert data members to a response message and write it to the socket
    void    send();

    // match the request uri with one of the locations
    void    matchResourceWithLocation();

    // handle possible request methods
    void    handleGet();
    void    handlePost();
    void    handleDelete();

    class  Response_err : public std::exception
    {
        public :
            Response_err(std::string msg) : _msg(msg) {}
            virtual ~Response_err() throw() {}
            virtual const char* what() const throw() { return _msg.c_str(); }
        private :
            std::string _msg;
    };
};


#endif