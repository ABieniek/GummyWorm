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
    printf("Wating for connection...\n");
    int sockfd, clisockfd, portno;
    char * end = "bye\n";
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        return(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 2410;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);


    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        return(1);
    }

    listen(sockfd,5);
    clilen = (socklen_t) sizeof(cli_addr);

    clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    if (clisockfd < 0) 
    {
        perror("ERROR on accept");
        return(1);
    }

    while (strcmp(end, buffer) !=0)
    {
        bzero(buffer,256);
        /* If connection is established then start communicating */
        n = read( clisockfd,buffer,256);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            return(1);
        }
        printf("command: %s\n",buffer);
        n = write(clisockfd,"Received\n",10);
        if (n < 0) {
            perror("ERROR writing to socket");
            return(1);
        }
    }
    close(sockfd);
    return 0;
}


