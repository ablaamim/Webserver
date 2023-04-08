# include "common.h"


char *bin2hex(const unsigned char *input, size_t len)
{
    char *result;
    char *hexits = "0123456789ABCDEF";
    
    if (input == NULL || len <= 0)
        return NULL;
    int resultlen = (len * 3) + 1;
    result = (char *)malloc(resultlen);
    if (result == NULL)
        return NULL;
    bzero(result, resultlen);
    for (int i = 0; i < len; i++)
    {
        result[i * 3] = hexits[input[i] >> 4];
        result[(i * 3) + 1] = hexits[input[i] & 0x0F];
        result[(i * 3) + 2] = ' ';
    }
    return (result);
}

void err_n_die(const char *fmt, ...)
{
    int errno_save;
    va_list ap;

    // any system call that fails will set errno
    errno_save = errno;

    // print the error message
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    fflush(stderr);

    // if errno was set, print its string value
    if (errno_save != 0)
    {
        fprintf(stderr, "(errno = %d) : %s\n", errno_save, strerror(errno_save));
        strerror(errno_save);
        fprintf(stderr, "\n");
        fflush(stderr);
    }
    va_end(ap);
    // exit the program
    exit(EXIT_FAILURE);  
}