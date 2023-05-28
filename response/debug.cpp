#include "Response.hpp"

void Response::print_request()
{
    std::cout << COLOR_GREEN << "Request :" << COLOR_RESET << std::endl;
    this->_req.print_params();
}

void Response::print_methods()
{
    std::map<std::string, void(Response::*)()>::iterator it = this->_methods.begin();
    while (it != this->_methods.end())
    {
        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET << it->second << std::endl;
        it++;
    }
}

void Response::print_kwargs()
{
    std::map<std::string, std::vector<std::string> >::iterator it = this->kwargs.begin();
    while (it != this->kwargs.end())
    {
        std::cout << COLOR_BLUE << it->first << " : " << COLOR_RESET;
        for (size_t i = 0; i < it->second.size(); i++)
        {
            std::cout << COLOR_YELLOW << it->second[i] << " " << COLOR_RESET;
            std::cout << std::endl;
            it++;
        }
    }
}
