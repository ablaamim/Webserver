#include "utils.hpp"

void    statusListInitialization(std::map<int, std::string> statusList)
{
    statusList[200] = "OK";
    statusList[201] = "Created";
    statusList[204] = "No Content";
    statusList[301] = "Moved Permanently";
    statusList[400] = "Bad Request";
    statusList[401] = "Unauthorized";
    statusList[403] = "Forbidden";
    statusList[404] = "Not Found";
    statusList[405] = "Method Not Allowed";
    statusList[409] = "Conflict";
    statusList[413] = "Request Entity Too Large";
    statusList[414] = "Request-url Too Long";
    statusList[500] = "Internal Server Error";
    statusList[501] = "Not Implemented";
    statusList[502] = "Bad Gateway";
    statusList[503] = "Service Unavailable";
    statusList[504] = "Gateway Timeout";
}

