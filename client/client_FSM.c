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
#include <math.h>
#include "client_FSM.h"

#define maxbuffer 500

int packet_numbers = 0;

Packet recv_packet(int packet_num,int sockfd,struct sockaddr *pservaddr)
{
    Packet packet;
    int n;
    socklen_t len;
    len = sizeof(*pservaddr);
    n = recvfrom(sockfd, &packet, sizeof(packet), MSG_WAITALL, pservaddr, &len);
    if (n < 0||n != sizeof(Packet)){
        perror("ERROR in recvfrom");
        return packet;
    }
    
     printf("packet %d received.\n", packet.seq_num);

    return packet;
}

Ack_packet recv_ack_packet(int sockfd, struct sockaddr *pservaddr)
{
    Ack_packet ack_packet;
    int n;
    socklen_t len;
    len = sizeof(*pservaddr);
    n = recvfrom(sockfd, &ack_packet, sizeof(ack_packet), MSG_WAITALL, pservaddr, &len);
    if (n < 0)
        perror("ERROR in recvfrom");
    return ack_packet;
}

void send_packet(Packet packet, int sockfd,  struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, &packet, sizeof(packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) perror("ERROR in sendto packet");
}

void send_ack_packet( Ack_packet ack_packet, int sockfd,  struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, (void*)&ack_packet, sizeof(ack_packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) perror("ERROR in sendto ack packet");
    
}

void recv_file(FILE *fp, int sockfd, struct sockaddr *pservaddr)
{
    Packet packet;
    int n;
    while (1)
    {
        packet = recv_packet(packet_numbers, sockfd, pservaddr);
        if (packet.length == 0)
            break;
        if (packet.seq_num != packet_numbers)
        {
            Ack_packet p;
            p.ack_num = packet_numbers;
            send_ack_packet(p, sockfd, pservaddr);
        }
        else
        { 
            fwrite(packet.data, 1, packet.length, fp);
            Ack_packet p;
            p.ack_num = packet_numbers;
            send_ack_packet(p, sockfd, pservaddr);
            packet_numbers++;
        }
    }
}