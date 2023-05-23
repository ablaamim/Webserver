#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <iostream>
#include <map>
#include <cstring>

class Request
{
    public:
        std::map<std::string , std::string> params;
        bool                                headers;
        bool                                first_line;
        int                                 content_lem;

        int     get_headers(std::string str);
        void    get_firstline(std::string line);
        void    get_other_lines(std::string line);
        int     check_readed_bytes(void);
        void    print_params();
        Request();
        ~Request();

};

#endif