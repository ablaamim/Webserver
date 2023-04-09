# ifndef COMMON_H
# define COMMON_H

#include <sys/socket.h> // BASIC SOCKET DEFINITIONS
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> // FOR VARIADIC FUNCTIONS
#include <unistd.h> // UNIX STANDARD FUNCTION DEFINITIONS
#include <arpa/inet.h> // IP ADDRESS CONVERSION FUNCTION DEFINITIONS
#include <netinet/in.h> // SOCKET ADDRESS STRUCTURES
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/uio.h>

/* Preprocessor macros for readability */

// Standard HTTP port
#define SERVER_PORT 18000
// BUFFER LENGTH WHERE DATA IS STORED
#define MAXLINE 4096
// SOCKET ADDRESS STRUCTURE
#define SA struct sockaddr

void err_n_die(const char *fmt, ...);
char *bin2hex(const unsigned char *input, size_t len);

# endif