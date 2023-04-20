#ifndef DEFINES_HPP

#define DEFINES_HPP

# define UNLIMITED_PARAMS 0
# define SIZEOF(arr) sizeof(arr) / sizeof(*arr)
# define DEFAULT_LISTEN_INTERFACE "0.0.0.0"
# define DEFAULT_LISTEN_PORT "8080"
# define PORT_MAX_VALUE 65535
# define MAX_BODY_SIZE 10000000
# define COLOR_RED "\033[1;31m"
# define COLOR_GREEN "\033[1;32m"
# define COLOR_RESET "\033[0m"
# define COLOR_YELLOW "\033[1;33m"
# define COLOR_BLUE "\033[1;34m"
# define COLOR_BLACK "\033[1;30m"
# define COLOR_WHITE "\033[1;37m"
# define INVALID_ARGS "Invalid number of arguments : Usage ./Parsing <configuration file>"
# define HELP "Help : configuration examples are to be found in ../configs folder."
# define PORT_ERR_DIGIT "Port must be a digit."
# define ERR_MAX_PORT "Port must be between 0 and 65535."
# define LISTEN_IP_ERR "Listen, up needs to be 4 octets separated by a dot."
# define LISTEN_META_ADDR "Listen, ip cannot be a meta address."
# define LISTEN_DOTS_ERR "Listen, ip split by dot needs to be a digit."
# define LISTEN_INTERVAL_ERR "Listen, ip split by dot needs to be between 0 and 255."
# define CGI_DOT_ERR "Cgi, key should start with a dot."
# define CGI_SIZE_ERR "Cgi, key should be at least 2 characters long."
# define CGI_SLASH_ERR "Cgi, key should not end with a slash."
# define TIMEOUT 30

#endif