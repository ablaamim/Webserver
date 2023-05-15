#include "Response.hpp"



Response::Response()
{
    statusListInitialization(this->statusList);
}

Response::~Response()
{

}

void    Response::generate()
{
    std::string responseMessage;

    responseMessage += this->httpVersion + " " + this->status.first + " " + this->status.second;
}
