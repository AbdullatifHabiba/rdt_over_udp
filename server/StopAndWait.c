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
#include "StopAndWait.h"

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
    if (packet.seq_num != packet_num)
        perror("ERROR: packet out of order");
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

void send_packet(Packet packet, int sockfd, struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, &packet, sizeof(packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) perror("ERROR in sendto packet");
}

void send_ack_packet( Ack_packet ack_packet, int sockfd, struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, (void*)&ack_packet, sizeof(ack_packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) perror("ERROR in sendto ack packet");
    
}

int get_number_of_packets() {
    return packet_numbers;
}

void  get_loss_packet(double prob_of_loss, int seednumber){
    srand(seednumber);
    int n_packets = get_number_of_packets();
    int i;
    int lost_packets = ceil(n_packets*prob_of_loss);
     int lost_packets_array[lost_packets];
    for(i=0;i<lost_packets;i++){
        double random = (double)(rand()%n_packets);
        if(lost_packets_array[i]==random){
            i--;
        }
        else{
            lost_packets_array[i]=random;
        }
    }
}

void send_file(FILE *fp, int sockfd, struct sockaddr* pservaddr)
{
    Packet packet;
    int packet_num = 0;
    int ack_num = 0;
    int n;
    while (1)
    {
        packet.seq_num = packet_num;
        packet.length = fread(packet.data, 1, maxbuffer, fp);
        if (packet.length == 0)
            break;
        send_packet(packet, sockfd, pservaddr);
        //wait
        Ack_packet ack_p = recv_ack_packet(sockfd, pservaddr);
        if (ack_p.ack_num != packet_num)
            perror("ERROR: ack out of order");
        packet_num++;
    }
    packet.seq_num = packet_num;
    packet.length = 0;
    send_packet(packet, sockfd, pservaddr);
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
        fwrite(packet.data, 1, packet.length, fp);
        Ack_packet p;
        p.ack_num = packet.seq_num;
        send_ack_packet(p, sockfd, pservaddr);
    }
}

int get_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}