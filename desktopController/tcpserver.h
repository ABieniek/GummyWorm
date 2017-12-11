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

ssize_t write_all_to_socket(int socket,  char *buffer, long count);
ssize_t read_all_from_socket(int socket,  char *buffer, long count);

typedef struct{
	int portnumber;
	char* videoFileName;
} serverArg;

void* runServer(void* arg);
