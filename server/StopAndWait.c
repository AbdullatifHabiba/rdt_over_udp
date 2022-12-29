
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

int sand_index = 0;

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
    
};
Ack_packet recv_ack_packet(int sockfd, struct sockaddr *pservaddr)
{
  
    // n = recvfrom(sockfd, &ack_packet, sizeof(ack_packet), MSG_WAITALL, pservaddr, &len);
    // if (n < 0)
    //     perror("ERROR in recvfrom");
    Ack_packet ack_packet;
    clock_t begin = clock();
    int flags = fcntl(sockfd, F_GETFL);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    while((clock() - begin)/ CLOCKS_PER_SEC < 10){
    int n;
    socklen_t len;
    len = sizeof(*pservaddr);
        int n = recvfrom(sockfd, &ack_packet, sizeof(ack_packet),
                             MSG_WAITALL, pservaddr, &len);
        if(n > 0){
           // status = 1;
            return ack_packet;
        }
    }
   // status = 0;    
    return ack_packet;
};

void send_packet(Packet packet, int sockfd,  struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, &packet, sizeof(packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) perror("ERROR in sendto packet");
};
void send_ack_packet( Ack_packet ack_packet, int sockfd,  struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, (void*)&ack_packet, sizeof(ack_packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0) perror("ERROR in sendto ack packet");
    
};

int []  get_loss_packets(double prob_of_loss, int seednumber){
    srand(seednumber);
    int n_packets=number_of_Packets;
    int i;
    int lost_packets= ceil(n_packets*prob_of_loss);
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
    return lost_packets_array;

}


void send_file(int lost[],FILE *fp, int sockfd,  struct sockaddr *pservaddr)
{
    Packet packet;
    int packet_num = 0;
    int ack_num = 0;
    int n;
    for (int i = 0; i < lost.length; i++)
    {
        printf("i= %d, l= %d",i,lost[i]);
    }
    
    while (1)
    {
        packet.seq_num = packet_num;
        packet.length = fread(packet.data, 1, maxbuffer, fp);
        if (packet.length == 0)
            break;
        send_packet(packet, sockfd, pservaddr);
       Ack_packet ack_p = recv_ack_packet(sockfd,pservaddr);
       printf("ack recived : %d \n",ack_p.ack_num);
        if (ack_p.ack_num != packet_num)
            perror("ERROR: ack out of order");
        packet_num++;
    }
    packet.seq_num = packet_num;
    packet.length = 0;
    send_packet(packet, sockfd, pservaddr);
   
};

void recv_file(FILE *fp, int sockfd, struct sockaddr *pservaddr)
{
    Packet packet;
    int n;
    while (1)
    {
        packet = recv_packet(sand_index, sockfd, pservaddr);
        if (packet.length == 0)
            break;
        fwrite(packet.data, 1, packet.length, fp);
        Ack_packet p;
        p.ack_num=packet.seq_num;
        
        send_ack_packet(p, sockfd, pservaddr);
       // sand_index++;
    }
};




