#include "../MainInc/main.hpp"

void Request::print_body()
{
    std::cout << COLOR_BLUE << "Body : " << COLOR_RESET << std::endl;
    for (size_t i = 0; i < this->body.size(); i++)
         std::cout << this->body[i];
    std::cout << std::endl;
    std::cout << COLOR_BLUE << "Body size : " << COLOR_RESET << this->body.size() << std::endl;
}

void    Request::print_params()
{
    it_param it = this->params.begin();
    while (it != this->params.end())
    {
        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
        it++;
    }
}

size_t Request::get_body_size()
{
    //std::cout << "Body size : " << this->params[_CONTENT_].size() << std::endl;
    return (this->params[_CONTENT_].length());
}

Request::Request(int fd) : _fd(fd)
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
        if (std::stoi(this->params["Content-Length"]) != \
        static_cast<int>(this->params[_CONTENT_].length()))
        {
            // std::cout << "check Content length: " << this->params["Content-Length"] << std::endl;
            // std::cout << "_CONTENT_: " << this->params[_CONTENT_].length() << std::endl;
            this->is_chuncked = true;
            return _CHUNCKED_REQUEST;
        }
        else
            this->is_chuncked = false;
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
            {
                this->method = str;
                break;
            }
            case 1 : this->params["Url"] = str;
            {
                this->path = str;        
                break;
            }
            case 2 : this->params["Protocol Version"] = str;
            {
                this->version = str;
                     break;
            }
        }
        i++;
    }
    this->first_line = true;
}

void Request::get_other_lines(std::string line)
{
    std::string         str1, str2;
    std::stringstream   file(line);

    //std::cout << "Parsing Other lines '" << line << "'" << std::endl;
    if (std::getline(file, str1, ':'))
    {
        if (std::getline(file, str2, ':'))
            this->params[str1] = str2.substr(1);
        else
        {
            //std::cout << COLOR_YELLOW << "Parsing Other here  '" << str1 << "'" << COLOR_RESET <<std::endl;
            this->params[_CONTENT_].append(str1);
            this->body.insert(this->body.end(), str1.begin(), str1.end());
            //std::cout << params[_CONTENT_] << std::endl;
            //std::cout << "Body size : " << this->body.size() << std::endl;
            //sleep(10);
        }
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

int Request::get_chuncked_msg(char * str)
{
    char *line;

    std::cout << "Parsing chuncked messages " << std::endl;
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    // std::cout << COLOR_GREEN << str << std::endl << std::endl;
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    line = std::strtok(str, "0\r\n\r\n");
    if (!line)
        return _ERR_PARSE_REQUEST;
    this->params[_CONTENT_].append(std::string(line));
    
    this->body.insert(this->body.end(), line, line + std::strlen(line));

    //std::cout << this->body.size() << std::endl;
    //sleep(10);

    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    // std::cout << COLOR_BLUE << this->params[_CONTENT_] << std::endl << std::endl;
    // std::cout << COLOR_YELLOW << std::strlen(line) << "   " << std::strlen(str) << std::endl << std::endl;
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    // return (check_readed_bytes());
    return (0);
}

int Request::parse_request(char * str)
{
    if (!str)
        return _ERR_PARSE_REQUEST;
    else if (!this->headers_done)
        return (this->get_headers(str));
    else if (this->is_chuncked)
        return(this->get_chuncked_msg(str));
    return 1;
}