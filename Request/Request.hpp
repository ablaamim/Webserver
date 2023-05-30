#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <cstring>

typedef std::map<std::string , std::string> param;
typedef std::map<std::string , std::string>::iterator it_param;

class Request
{
    public:
        param   params;
        bool    headers_done;
        bool    first_line;
        bool    is_chuncked;
        int     fd_accept;
        int     fd_server;

        std::string         method;
        std::string         path;
        std::string         version;
        std::vector<char>   body;

        int                 parse_request(char *str);
        int                 get_headers(std::string str);
        int                 get_chuncked_msg(std::string str);
        void                get_firstline(std::string line);
        void                get_other_lines(std::string line);
        int                 check_readed_bytes(void);
        size_t              get_body_size();
        void                print_params();
        void                print_body();
        void                reset_request();
        std::vector<char>   get_body();
        std::string         get_content();
        Request();
        ~Request();
};

std::ostream & operator<<(std::ostream & o, Request const & ref);

#endif