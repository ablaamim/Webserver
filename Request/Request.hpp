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
        int     _fd;

        int     parse_request(char * str);
        int     get_headers(char * str);
        void    get_firstline(std::string line);
        void    get_other_lines(std::string line);
        int     check_readed_bytes(void);
        
        void    print_params()
        {
            it_param it = this->params.begin();
            while (it != this->params.end())
            {
                std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
                it++;
            }
        }
        
        void    reset_request();
        Request(int fd = -1);
        ~Request();
};

std::ostream & operator<<(std::ostream & o, Request const & ref);

#endif