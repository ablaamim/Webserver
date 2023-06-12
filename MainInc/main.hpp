#ifndef MAIN_HPP
# define MAIN_HPP

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <dirent.h>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <netinet/in.h>
#include <set>
#include <signal.h>
#include <sstream>
#include <stdlib.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <stdlib.h>

# include "../defines/defines.hpp"
# include "../tools/tools.hpp"
# include "../parsing/debug.hpp"
# include "../parsing/libcpp.hpp"
# include "../parsing/ConfigurationParser.hpp"
# include "../parsing/debug.hpp"
# include "../Servers/Servers.hpp"
# include "../Request/Request.hpp"
# include "../response/Response.hpp"
# include "../global_interface/Webserv.hpp"

#endif