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

    // begin of request attributes:

    std::string     method;
    std::string     client_ip;
    std::string     client_port;
    std::string     resoucePath; // like /directory/index.html

    // end of request attributes


    bool            isCompleted; // whether the reponse has served 100% of resourceLength
    int             resouceLength; // e.g. requested resouce size in Bytes
    int             currentLength; // usefull for chuncking reponses
    int             resourceType; // MACRO, like DIRECTORY, FILE, CGI_SCRIPT
    
    std::string     resourceFullPath;
    std::string     httpVersion;
    std::string     body;

    // response status code + message (e.g. 200, OK)
    std::pair   <std::string, std::string>  status;

    // response headers
    std::map    <std::string, std::string>  headers;

    /*
        kwargs or keywords arguments.
        could contain dynamic args depending on the location.
        for example it could have ["allowed_methods":"GET, POST, DELETE"]
        OR/AND "cgi-bin":".py, /bin/python"
    */
    std::map    <std::string, std::vector<std::string> >  kwargs;
    
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