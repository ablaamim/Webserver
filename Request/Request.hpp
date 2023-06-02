#ifndef REQUEST_HPP
# define REQUEST_HPP

# include "../MainInc/main.hpp"

typedef std::map<std::string , std::string> param;
typedef std::map<std::string , std::string>::iterator it_param;

class Request
{
    public:
        param               params;
        bool                headers_done;
        bool                first_line;
        bool                is_chuncked;
        int                 fd_accept;
        int                 fd_server;

        std::string         method;
        std::string         path;
        std::string         version;
        size_t              content_length;
        std::string         body;
        std::string         body_name;

        std::string get_content();
        int         parse_request(char *str);
        int         get_headers(std::string str);
        int         get_chuncked_msg(std::string str);
        void        get_firstline(std::string line);
        void        get_other_lines(std::string line);
        int         check_readed_bytes(void);
        void        print_params();
        void        reset_request();
        Request();
        Request(Request const & ob);
        Request & operator=(Request const &ob);
        ~Request();
};

std::ostream & operator<<(std::ostream & o, Request const & ref);

#endif