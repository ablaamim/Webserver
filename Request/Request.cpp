#include "../MainInc/main.hpp"

Request::Request()
{
    this->headers_done = false;
    this->first_line = false;
    this->is_chuncked = false;
    this->params[_CONTENT_] = "";
}
Request::~Request(){};

std::ostream & operator<<(std::ostream & o, Request const & ref)
{
    param para =  ref.params;
    std::cout << std::endl << COLOR_GREEN << "Request :" << COLOR_RESET << std::endl;
    std::cout << "Server fd :" << ref.fd_server <<  "  Client fd : " << ref.fd_accept << std::endl;
    for(it_param it = para.begin(); it != para.end(); it++ )
        std::cout << it->first << " : '" << it->second << "'" << std::endl;
    return o;
}

void    Request::reset_request()
{
    this->headers_done = false;
    this->first_line = false;
    this->is_chuncked = false;
    this->params.clear();
    this->params[_CONTENT_] = "";
}

int Request::check_readed_bytes()
{
    if (this->params.find("Content-Length") != this->params.end())
    {
        // std::cout << "check Content length: " << this->params["Content-Length"] << std::endl;
        // std::cout << "_CONTENT_: " << this->params[_CONTENT_].size() << std::endl;
        this->is_chuncked = true;
        return(std::stoi(this->params["Content-Length"]) != \
        static_cast<int>(this->params[_CONTENT_].size()));
    }
    return _PARSE_REQUEST_DONE;
}

void Request::get_firstline(std::string line)
{
    std::string         str;
    std::stringstream   file(line);
    int                 i = 0;

    // std::cout << "Parsing First line " << std::endl;
    while (std::getline(file, str, ' '))
    {
        switch (i)
        {
            case 0 : this->params["Method"] = str;
                     break;
            case 1 : this->params["Url"] = str;
                     break;
            case 2 : this->params["Protocol Version"] = str;
                     break;
        }
        i++;
    }
    this->first_line = true;
}

void Request::get_other_lines(std::string line)
{
    std::string         str1, str2;
    std::stringstream   file(line);

    // std::cout << "Parsing Other lines " << line << std::endl;
    if (std::getline(file, str1, ':'))
    {
        if (std::getline(file, str2, ':'))
            this->params[str1] = str2.substr(1);
        else
            this->params[_CONTENT_].append(str1);
    }
}

int Request::get_headers(char * str)
{
    char *line;

    // std::cout << "Parsing headers " << std::endl;
    line = std::strtok(str, "\r\n");
    if (!line)
        return _ERR_PARSE_REQUEST;
    while(line)
    {
        if (!this->first_line)
            this->get_firstline(std::string(line));
        else
            this->get_other_lines(std::string(line));
        line = std::strtok(NULL, "\r\n");
    }
    this->headers_done = true;
    return (check_readed_bytes());
}

int Request::parse_request(char * str)
{
    if (!str)
        return _ERR_PARSE_REQUEST;
    else if (!this->headers_done)
        return (this->get_headers(str));
    // else if (this->is_chuncked)
    //     return(this->get_chuncked_msg(str))
    return _PARSE_REQUEST_DONE;
}