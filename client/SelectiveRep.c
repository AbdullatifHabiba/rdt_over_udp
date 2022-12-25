
#include"SelectiveRep.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int packet_numbers = 0;

Packet packets [packet_numbers];
void recv_file(int sockfd, struct sockaddr *pservaddr, int packet_numbers)
{
    int i;
    for (i = 0; i < packet_numbers; i++)
    {
        packets[i] = recv_packet(i, sockfd, pservaddr);
    }
    for (i = 0; i < packet_numbers; i++)
    {
        write_file(packets[i].data, packets[i].length);
    }
    
}
