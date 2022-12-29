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
# define maxbuffer 500

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

Ack_packet recv_ack_packet(int sockfd, struct sockaddr *pservaddr, int time_out, int* status)
{
    clock_t start = clock();
    Ack_packet ack_packet;
    while (clock() - start < time_out * CLOCKS_PER_SEC)
    {
        int n;
        socklen_t len;
        len = sizeof(*pservaddr);
        n = recvfrom(sockfd, &ack_packet, sizeof(ack_packet), MSG_WAITALL, pservaddr, &len);
        if (n > 0)
        {
            *status = 1;
            return ack_packet;
        }
        if (n < 0)
            perror("ERROR in recvfrom");
    }
    *status = 10;
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

void get_loss_packet(double prob_of_loss, int seednumber, int lost_packets_array[], int lost_packets){
    srand(seednumber);
    int n_packets = get_number_of_packets();
    int i;
    lost_packets = ceil(n_packets * prob_of_loss);
    lost_packets_array[lost_packets];
    for(i = 0;i < lost_packets;i++){
        double random = (double)(rand()%n_packets);
        if(lost_packets_array[i] == random){
            i--;
        }
        else{
            lost_packets_array[i] = random;
        }
    }
}

void send_file(FILE *fp, int sockfd,  struct sockaddr *pservaddr)
{
    Packet packet;
    int packet_num = 0;
    int ack_num = 0;
    int n;
    int status = 0;
    int time_out = 100;
    while (1)
    {
        if (packet.length == 0)
            break;
        packet.seq_num = packet_num;
        packet.length = fread(packet.data, 1, maxbuffer, fp);
        if(check_packet_in_lost_packets(lost_packets_array, packet_num) == 1){
            packet_num++;
            continue;
        }
        Ack_packet ack_p;
        while(status != 1)
        {
            send_packet(packet, sockfd, pservaddr);
            
            ack_p = recv_ack_packet(sockfd, pservaddr, time_out, &status);
        }
        status = 0;
        if (ack_p.ack_num != packet_num)
            perror("ERROR: ack out of order");
        packet_num++;
    }
    packet.seq_num = packet_num;
    packet.length = 0;
    send_packet(packet, sockfd, pservaddr);
};
int check_packet_in_lost_packets(int lost_packets_array[], int lost_packets, int packet_num){
    int i;
    for(i = 0;i < lost_packets;i++){
        if(lost_packets_array[i] == packet_num){
            return 1;
        }
    }
    return 0;
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
        p.ack_num=packet.seq_num;
        
        send_ack_packet(p, sockfd, pservaddr);
       // packet_numbers++;
    }
}

int get_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}