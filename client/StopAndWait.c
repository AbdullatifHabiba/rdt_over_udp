#include "StopAndWait.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int packet_numbers = 0;

struct Packet recv_packet(int packet_num,int sockfd,struct sockaddr *pservaddr)
{
    struct Packet packet;
    int n;
    socklen_t len;
    len = sizeof(*pservaddr);
    n = recvfrom(sockfd, &packet, sizeof(packet), MSG_WAITALL, pservaddr, &len);
    if (n < 0||n != sizeof(Packet)){
        perror("ERROR in recvfrom");
        return Packet();
    }
    if (packet.seq_num != packet_num)
        error("ERROR: packet out of order");
    return packet;
    
};
struct Ack_packet recv_ack_packet(int sockfd, struct sockaddr *pservaddr)
{
    struct Ack_packet ack_packet;
    int n;
    socklen_t len;
    len = sizeof(*pservaddr);
    n = recvfrom(sockfd, &ack_packet, sizeof(ack_packet), MSG_WAITALL, pservaddr, &len);
    if (n < 0)
        error("ERROR in recvfrom");
    return ack_packet;
};

void send_packet(struct Packet packet, int sockfd, const struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, &packet, sizeof(packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) error("ERROR in sendto packet");
};
void send_ack_packet(struct Ack_packet ack_packet, int sockfd, const struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, (void*)&ack_packet, sizeof(ack_packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) error("ERROR in sendto ack packet");
    
};
void  get_loss_packets(double prob_of_loss, int seednumber){
    srand(seednumber);
    int n_packets=get_number_of_packets();
    int i;
    int lost_packets=Math.ceil(n_packets*prob_of_loss);
     int lost_packets_array[lost_packets];
    for(i=0;i<lost_packets;i++){
        double random = (double)rand()%n_packets;
        if(lost_packets_array[i]==random){
            i--;
        }
        else{
            lost_packets_array[i]=random;
        }
       
    }

}
int get_number_of_packets() {
    return packet_numbers;
}

void send_file(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen)
{
    struct Packet packet;
    int packet_num = 0;
    int ack_num = 0;
    int n;
    while (1)
    {
        packet.seq_num = packet_num;
        packet.length = fread(packet.data, 1, MAXLINE, fp);
        if (packet.length == 0)
            break;
        send_packet(packet, sockfd, pservaddr, servlen);
        ack_num = recv_ack(sockfd, pservaddr);
        if (ack_num != packet_num)
            error("ERROR: ack out of order");
        packet_num++;
    }
    packet.seq_num = packet_num;
    packet.length = 0;
    send_packet(packet, sockfd, pservaddr, servlen);
};

void recv_file(FILE *fp, int sockfd, struct sockaddr *pservaddr, socklen_t *servlen)
{
    struct Packet packet;
    int n;
    while (1)
    {
        packet = recv_packet(packet_num, sockfd, pservaddr);
        if (packet.length == 0)
            break;
        fwrite(packet.data, 1, packet.length, fp);
        send_ack(packet_num, sockfd, pservaddr, *servlen);
        packet_num++;
    }
};



