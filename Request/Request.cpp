#include "../MainInc/main.hpp"

Request::Request()
{
    this->headers = false;
    this->first_line = false;
    this->params[_CONTENT_] = "";
}
Request::~Request(){};

void Request::print_params()
{
    std::cout << std::endl << COLOR_GREEN << "Request :" << COLOR_RESET << std::endl;
    for(std::map<std::string , std::string>::iterator it = this->params.begin(); it != this->params.end(); it++ )
        std::cout << it->first << " : '" << it->second << "'" << std::endl;
}

int Request::check_readed_bytes()
{
    if (this->params.find("Content-Length") != this->params.end())
    {
        std::cout << "check Content length: " << this->params["Content-Length"] << std::endl;
        std::cout << "_CONTENT_: " << this->params[_CONTENT_].size() << std::endl;
        return(std::stoi(this->params["Content-Length"]) != (int)this->params[_CONTENT_].size());
    }
    return 0;
}

void Request::get_firstline(std::string line)
{
    std::string         str;
    std::stringstream   file(line);
    int                 i = 0;

    std::cout << "          Parsing First line " << std::endl;
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
    this->headers = true;
}

void Request::get_other_lines(std::string line)
{
    std::string         str1, str2;
    std::stringstream   file(line);

    std::cout << "Parsing Other lines " << std::endl;
    std::getline(file, str1);
    if (std::getline(file, str1, ':'))
    {
        if (std::getline(file, str2, ' '))
        {
            str2 = str2.substr(0);
            this->params[str1] = str2;
        }
        else
            this->params["_CONTENT_"].append(str1);
    }
}

int Request::get_headers(std::string str)
{
    std::string line;
    std::stringstream file(str);

    // std::cout << "Parsing headers " << std::endl;
    // if (this->headers)
    //     this->get_chuncked_msg(str);
    if (!std::getline(file, line, '\n'))
        return _ERR_PARSE_REQUEST;
    if (!this->first_line)
        this->get_firstline(line.size() - 1);
    while(std::getline(file, line, '\n'))
        this->get_other_lines(line.substr(0, line.size() - 1));
    // if (check_readed_bytes())
    //     return 1;
    return 0;
}