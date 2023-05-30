#include "../MainInc/main.hpp"

void Request::print_body()
{
    std::cout << COLOR_BLUE << "Body : " << COLOR_RESET << std::endl;
    for (size_t i = 0; i < this->body.size(); i++)
         std::cout << this->body[i];
    std::cout << std::endl;
    std::cout << COLOR_BLUE << "Body size : " << COLOR_RESET << this->body.size() << std::endl;
}

std::vector<char>   Request::get_body()
{
    return this->body;
}
std::string Request::get_content()
{
    return this->params[_CONTENT_];
}

void    Request::print_params()
{
    it_param it = this->params.begin();
    std::cout << COLOR_RED << "   Display Request params " << COLOR_RESET << std::endl;
    while (it != this->params.end())
    {
        if (it->first != _CONTENT_)
            std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
        it++;
    }
}

size_t Request::get_body_size()
{
    //std::cout << "Body size : " << this->params[_CONTENT_].size() << std::endl;
    return (this->params[_CONTENT_].length());
}

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

    //std::cout << "Parsing First line " << std::endl;
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
    size_t  indx;

    //std::cout << COLOR_GREEN << "Parsing Other lines '" << line << "'" << COLOR_RESET << std::endl;
    if (this->params.find("Content-Type") != this->params.end())
    {
        this->params[_CONTENT_].append(line);
        return ;
    }
    indx = line.find(": ");
    if (indx != std::string::npos )
        this->params[line.substr(0, indx)] = line.substr(indx + 2);
}

int Request::get_headers(std::string str)
{
    size_t line;

    if (!str.size())
        return 1;
    //std::cout << "Parsing headers " << std::endl;
    while((line = str.find("\r\n")) != std::string::npos)
    {
        if (!this->first_line)
            this->get_firstline(str.substr(0, line));
        else
            this->get_other_lines(str.substr(0, line));
        str = str.substr(line + 2);
    }
    if (str.size())
        this->get_other_lines(str);
    this->headers_done = true;
    return (check_readed_bytes());
}

int Request::get_chuncked_msg(std::string str)
{
    size_t line;

    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    // std::cout << COLOR_GREEN << this->params[_CONTENT_] << std::endl << std::endl;
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    while((line = str.find("0\r\n\r\n")) != std::string::npos)
    {
        this->params[_CONTENT_].append(str.substr(0, line));
       // this->body.insert(this->body.end(), line, line + std::strlen(line));
        str = str.substr(line + 5);
    }
    this->params[_CONTENT_].append(str);
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    // std::cout << COLOR_BLUE << this->params[_CONTENT_] << std::endl << std::endl;
    // std::cout << COLOR_YELLOW << std::strlen(line) << "   " << std::strlen(str) << std::endl << std::endl;
    // std::cout << COLOR_RED << "----------------------------------" << std::endl;
    //return (check_readed_bytes());
    return (0);
}

int Request::parse_request(char * str)
{
    if (!str)
        return _ERR_PARSE_REQUEST;
    else if (!this->headers_done)
        return (this->get_headers(std::string(str)));
    else if (this->is_chuncked)
        return(this->get_chuncked_msg(str));
    return 1;
}