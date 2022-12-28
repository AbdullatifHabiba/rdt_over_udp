#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>
#include <pthread.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    if (argc != 2)
    {
        printf("usage: udpcli <IPaddress>");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9877);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    dg_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    exit(0);
}