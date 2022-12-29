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
#include <time.h>
#include <sys/time.h>
#include "SelectiveRep.h";


int packet_numbers = 0;

struct Packet packets [packet_numbers];
Map recieved_packets[packet_numbers];
int recieved_packets_size = 0;
int recieved_packets_index = 0;

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

int get_number_of_packets() {
    return packet_numbers;
}

recieve_selective_rep(int sockfd, struct sockaddr *pservaddr, int time_out, int* status)
{
    clock_t start = clock();
    Packet packet;
    while (clock() - start < time_out * CLOCKS_PER_SEC)
    {
        int n;
        socklen_t len;
        len = sizeof(*pservaddr);
        n = recvfrom(sockfd, &packet, sizeof(packet), MSG_WAITALL, pservaddr, &len);
        if (n > 0)
        {
            *status = 1;
            return packet;
        }
        if (n < 0)
            perror("ERROR in recvfrom");
    }
    *status = 10;
    return packet;
}
create_ack_packet(int seq_num)
{
    Ack_packet ack_packet;
    ack_packet.ack_num = seq_num;
    ack_packet.length = 0;
    ack_packet.checksum = 0;
    return ack_packet;
}
recieve_window_packets(int sockfd, struct sockaddr *pservaddr,char* file)
{
    Packet packet;
    packet = recv_packet(recieved_packets_index+1, sockfd, pservaddr);
    
        add_packet_to_map(packet);
        send_ack_packet(create_ack_packet(packet.seq_num), sockfd, pservaddr);
        recieve_window_packets(sockfd, pservaddr, file);
    

     if(recieved_packets_size == packet_numbers){
        write_packets_to_file(file);
        return;
    }
    
}
add_packet_to_map(Packet packet)
{
    recieved_packets[recieved_packets_index].packet = packet;
    recieved_packets[recieved_packets_index].seq_num = packet.seq_num;
    recieved_packets_index++;
    recieved_packets_size++;
}

recive_lost_packets(){
    for (int i = 0; i < recieved_packets_size; i++)
    {
        if (recieved_packets[i].packet.seq_num != i+1)
        {
            recieved_packets[i].packet = recv_packet(i+1, sockfd, pservaddr);
            recieved_packets[i].seq_num = recieved_packets[i].packet.seq_num;
        }
    }
}
write_packets_by_seq_num(char* file)
{
    FILE *fp;
    fp = fopen(file, "wb");
    for (int i = 0; i < recieved_packets_size; i++)
    {
        fwrite(recieved_packets[i].packet.data, sizeof(char), recieved_packets[i].packet.length, fp);
    }
    fclose(fp);
    
}
    


write_packets_to_file( char*file)
{
    FILE *fp;
    fp = fopen(file, "wb");
    for (int i = 0; i < recieved_packets_size; i++)
    {
        fwrite(recieved_packets[i].packet.data, sizeof(char), recieved_packets[i].packet.length, fp);
    }
    fclose(fp);
    
}


