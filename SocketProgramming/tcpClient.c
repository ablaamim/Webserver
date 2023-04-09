#include "common.h"

// Socket Programming in C / IPC

// THIS IS THE WEB CLIENT PART

int main(int argc, char **argv)
{
    int                sockfd, n;
    int                sendbytes;
    struct sockaddr_in servaddr;
    char              sendline[MAXLINE];
    char              recvline[MAXLINE];

    // check if the user has provided the server address
    if (argc != 2)
        err_n_die("Usage: %s <Server address>", argv[0]);
    // create a socket, 0 = default protocol (TCP)

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_n_die("socket error");
    printf("-> Socket created successfully\n\n");
    // Zero out the server address structure
    // AF_INET = IPv4 Internet protocols
    // SOCK_STREAM = TCP
    // PORT = 80
    // htons = host to network short
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT); // port
    
    // translate address :
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
        err_n_die("inet_pton error for %s", argv[1]);
    printf("Address translated successfully\n\n");
    // Connect to the server

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
    {
        //printf("Connection failed\n\n");
        err_n_die("connect error"); 
    }
    printf("Connected to server successfully\n\n");
    // Send a GET request to the server
    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    // Send the request and write the response to the socket
    if (write(sockfd, sendline, sendbytes) != sendbytes)
        err_n_die("write error");
    printf ("Request sent successfully\n\n");
    
    // RECEIVE THE RESPONSE
    memset(recvline, 0, MAXLINE);
    // Now read server response
    
    while ((n = read(sockfd, recvline, MAXLINE - 1)) > 0)
        printf("%s", recvline);
    
    printf("Response received successfully\n\n");

    if (n < 0)
        err_n_die("read error");
    exit(EXIT_SUCCESS);
}