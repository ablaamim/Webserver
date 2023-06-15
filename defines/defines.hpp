#ifndef DEFINES_HPP

#define DEFINES_HPP

# define BUFFER_SIZE 2048
# define EVENT_LIST 10000
# define UNLIMITED_PARAMS 0
# define SIZEOF(arr) sizeof(arr) / sizeof(*arr)
# define DEFAULT_LISTEN_INTERFACE "127.0.0.1"
# define DEFAULT_LISTEN_PORT "8080"
# define PORT_MAX_VALUE 65535
# define MAX_BODY_SIZE 1000000000
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
# define TIMEOUT 120
# define CGI_DOTDOT_ERR "Cgi, value should not contain ../"
# define ROOT_DOTDOT_ERR "Root, value should not contain ../"
# define ROOT_SIZE_ERR "Root, value should be at least 2 characters long."
# define BODYSIZE_DIGIT_ERR "BodySize, value should be a digit."
# define BODYSIZE_OVFL_ERR "Body size overflow, max value : "
# define SERVER_PARAM_ERR "Server : Doesn't take parameters"
# define SERVER_BRACKET_ERR "Server : Server context should be followed by a '{'"
# define SOCKET_CREATE_ERR "Failed creating socket"
# define SOCKET_BINDING_ERR "Failed binding socket"
# define SOCKET_OPTION_ERR "Failed setting socket options"
# define SOCKET_LISTEN_ERR "Failed listening on socket"
# define _ERR_PARSE_REQUEST 1
# define _PARSE_REQUEST_DONE 0
# define _CHUNCKED_REQUEST 2
# define _CONTENT_ "Content_body"
# define _TMP_FILE_ "tmp/"

/*  Response defines */

# define NONE -1
# define FILE 0
# define DIRECTORY 1
# define CGI 2
# define REDIRECT 3
# define NOT_FOUND 4
# define FORBIDDEN 5
# define FULL 6
# define HEADERS_ONLY 7
# define CHUNCK_SIZE 1024

# define GET "GET"
# define POST "POST"
# define DELETE "DELETE"

# define DEFAULT_ROOT "/Users/afaris/Desktop/Webserver/www/html"

/* end of Response defines */

// Server Code Status 
# define _CS_200 "200"
# define _CS_201 "201"
# define _CS_301 "301"
# define _CS_302 "302"
# define _CS_303 "303"
# define _CS_307 "307"
# define _CS_400 "400"
# define _CS_403 "403"
# define _CS_404 "404"
# define _CS_405 "405"
# define _CS_409 "409"
# define _CS_411 "411"
# define _CS_413 "413"
# define _CS_500 "500"
# define _CS_501 "501"
# define _CS_505 "505"
# define _CS_520 "520"

// Server Code Status Messages
# define _CS_200_m "OK"
# define _CS_201_m "Created"
# define _CS_301_m "Moved Permanently"
# define _CS_400_m "Bad Request"
# define _CS_403_m "Forbidden"
# define _CS_404_m "Not Found"
# define _CS_405_m "Method not allowed"
# define _CS_409_m "Conflict"
# define _CS_411_m "Length Required"
# define _CS_413_m "Payload Too Large"
# define _CS_500_m "Internal Server Error"
# define _CS_501_m "Not Implemented"
# define _CS_505_m "HTTP Version Not Supporte"
# define _CS_520_m "Web Server Returned an Unknown Error"

#endif