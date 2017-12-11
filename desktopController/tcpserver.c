#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include "tcpserver.h"

void* runServer(void* arg) {
    serverArg* sa = (serverArg*) arg;
    int portno = sa->portnumber;
    char* outputname = sa->videoFileName;
    printf("Wating for connection...\n");
    int sockfd, clisockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(5);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 2410;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);


    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd,5);
    clilen = (socklen_t) sizeof(cli_addr);

    clisockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    if (clisockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    }
    printf("Made connection\n");
    bool flag = 0; // 0 when reading filesize, 1 when reading data
    long filesize = 0; // default
    char *eptr;


    char buffer[1024];
    ssize_t sizeleft = 0;
    while (1) {
        bzero(buffer, 1024);
        if (flag == false) {
            // read in 8 bytes, interpret them as a (long) filesize
            n = read(clisockfd, buffer, sizeof(long));
            if (n == 0) {
                close(sockfd);
                exit(0);
            }
            if (n < 0) {
                perror("ERROR reading from socket");
                exit(1);
            }
            memcpy(&filesize, buffer, sizeof(long));
            sizeleft = filesize;
        } else {
            FILE *writefp = fopen(outputname, "w");
            if (writefp == NULL) {
                printf("Error to open file\n");
            }
            while (sizeleft > 0) {
                int readsize = 1024; if (sizeleft < 1024) readsize = sizeleft;
                n = read_all_from_socket(clisockfd, buffer, readsize);
		if (n == 0) {
                    close(sockfd);
                    exit(0);
                }
                if (n < 0 || n != readsize){
                    perror("");

                } else if (n == 0) {
                    close(sockfd);
                    return 0;
                }

                fwrite(buffer, 1, readsize, writefp);
                sizeleft -= n;
            }
            if (sizeleft != 0) {
                fprintf(stderr, "wrote too many bytes to file, sizeleft: %ld\n", sizeleft);
            }
            fclose(writefp);
       }
       if (flag == false) flag = true;
       else flag = false;

    }
    //close(sockfd);
    return 0;
}

// attemps to read all count bytes from socket into buffer
// returns number of bytes read, 0 if socket disconnected or -1 on failure
ssize_t read_all_from_socket(int socket, char *buffer, long count) {
    // Your Code Here
    ssize_t bytes_read = 0;
    ssize_t bytes_left = (ssize_t) count;

    while (bytes_left > 0) {
        ssize_t curr_read  = read(socket, buffer, bytes_left);
        if (curr_read == 0) {
            // connection closed, return total bytes read
            return 0;
            //return bytes_read;
        } else if (curr_read == -1 && errno == EINTR) {
            // read interrupted, try again
        } else if (curr_read == -1 && errno != EINTR) {
            // read failed, don't try again
            return -1;
        } else {
            // continue reading
            buffer += curr_read;
            bytes_left -= curr_read;
            bytes_read += curr_read;
        }
    }
    return bytes_read;
}


// attempts to write all count bytes from buffer to socket
// returns number of bytes written, 0 if socket disconnected or -1 on failure
ssize_t write_all_to_socket(int socket, char *buffer, long count) {
    // Your Code Here
    ssize_t bytes_write = 0; // total bytes written
    ssize_t bytes_left = (ssize_t) count;

    while (bytes_left > 0) {
        ssize_t curr_write  = write(socket, buffer, bytes_left);
        if (curr_write == 0) {
            // connection closed, return total bytes read
            return bytes_write;
        } else if (curr_write == -1 && errno == EINTR) {
            // write interrupted, try again
        } else if (curr_write == -1 && errno != EINTR) {
            // write failed, don't try again
            return -1;
        } else {
            // continue reading
            buffer += curr_write;
            bytes_left -= curr_write;
            bytes_write += curr_write;
        }
    }
    return bytes_write;
}
