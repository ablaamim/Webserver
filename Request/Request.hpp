#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <cstring>

typedef std::map<std::string , std::string> param;

class Request
{
    public:
        std::map<std::string , std::string> params;
        bool                                headers_done;
        bool                                first_line;
        bool                                is_chuncked;
        int                                 fd_accept;

        int     parse_request(char * str);
        int     get_headers(char * str);
        void    get_firstline(std::string line);
        void    get_other_lines(std::string line);
        int     check_readed_bytes(void);
        void    print_params();
        Request();
        ~Request();
};

#endif