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
        int                 fd_server;
        int                 error;

        std::string         method;
        std::string         path;
        std::string         version;
        std::string         erro_msg;
        size_t              content_length;
        std::string         content_type;
        std::string         file_body_name;
        std::ofstream       *file;

        int     parse_request(std::string str);
        int     get_headers(std::string str);
        void    parse_headers(std::string str);
        void    get_content_extension();
        int     get_chuncked_msg(std::string str);
        void    get_firstline(std::string line);
        void    get_other_lines(std::string line);
        int     check_readed_bytes(void);
        void    print_params();
        void    reset_request();
        int     open_file_for_reponse(std::string str);
        std::string gen_random();
        Request();
        Request(Request const & ob);
        Request & operator=(Request const &ob);
        ~Request();
};

std::ostream & operator<<(std::ostream & o, Request const & ref);

#endif