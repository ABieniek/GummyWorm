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
    char *end = "bye\n";
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        return(1);
    }
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(5);
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
    int flag = 0; // 0 when reading filesize, 1 when reading data
    long filesize = sizeof(long); // default
    char *eptr;
    FILE *writefp = fopen("wm.webm", "wb");
    if (writefp == NULL) {
        printf("Error to open file\n");
    }
    
    char buffer[1024];
    while (strcmp(end, buffer) !=0)
    {
        bzero(buffer, 1024);
        if (flag == 0) {
            n = read(clisockfd, buffer, sizeof(long));
            if (n < 0) {
                perror("ERROR reading from socket");
                return(1);
            }
            filesize = strtol(buffer, &eptr, 10);
//            printf("filesize: %ld\n", filesize);
        } else {
            n = read(clisockfd, buffer, filesize);
            if (n < 0) {
                perror("ERROR reading from socket");
                return(1);
            }

            if (fwrite(buffer, 1, filesize, writefp) != filesize) {
                printf("Error writing fo file\n");
            
            }
//            printf("tried to write %s\n", buffer);
        }
        flag = (flag+1)%2;
/*
        bzero(buffer,256);
        // If connection is established then start communicating 
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
*/
    }
    fclose(writefp);
    close(sockfd);
    return 0;
}


