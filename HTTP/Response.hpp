#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "../MainInc/main.hpp"

class Response
{

    public :
        Response()
        {
            std::cout << "Response default constructor" << std::endl;
        };
        ~Response()
        {
            std::cout <<  "Response destructor" << std::endl;
        };
        
        configurationSA::location env;

};

#endif