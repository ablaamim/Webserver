#ifndef MAIN_HPP
# define MAIN_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>


# include "../parsing/ConfigurationParser.hpp"
# include "../parsing/debug.hpp"
# include "../defines/defines.hpp"
# include "../Servers/Servers.hpp"
# include "../global_interface/Webserv.hpp"

void check_param_nbr(int argc);

#endif