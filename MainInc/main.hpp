#ifndef MAIN_HPP
# define MAIN_HPP

#include <iostream>
#include <iomanip>
#include <fstream>
#include <fcntl.h>
#include <sstream>
#include <exception>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>


# include "../parsing/debug.hpp"
# include "../parsing/libcpp.hpp"
# include "../parsing/ConfigurationParser.hpp"
# include "../parsing/debug.hpp"
# include "../defines/defines.hpp"
# include "../Servers/Servers.hpp"
# include "../global_interface/Webserv.hpp"
# include "../defines/defines.hpp"
# include "../tools/tools.hpp"

void check_param_nbr(int argc);

#endif