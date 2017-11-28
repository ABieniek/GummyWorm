#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>


int main( int argc, char *argv[] )
{
    printf("Waiting for connection...\n");
    int sockfd, clisockfd, portno;
    char * end = "bye\n";
    socklen_t clilen;
    char buffer[1024]; // prev 256
    struct sockaddr_in serv_addr, cli_addr;
    int  n;
    //int optval;

    // First call to socket() function 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        return(1);
    }

    // Initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 2410;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        return(1);
    }

//    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    // close connection when client sends "bye\n"

    int namelen = sizeof(cli_addr);

    while (strcmp(end, buffer) !=0)
    {
        bzero(buffer,1024); // zeros out buffer

        if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &cli_addr, &namelen) < 0) {
            perror("recvfrom()");
            exit(4);
        }
     
        // INTERPRET AS IMAGE HERE

        printf("command: %s",buffer);

    }
    close(sockfd);
    return 0;
}


