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

void resend_packets(int sockfd, struct sockaddr *pservaddr, int lost_packets_array[], int lost_packets, Map map[]){
    int i;
    for(i = 0;i < lost_packets;i++){
        send_packet(map[lost_packets_array[i]].packet, sockfd, pservaddr);
    }
}

selective_repeat_send_file(){
    
    get_loss_packet(prob_of_loss, seednumber, lost_packets_array, lost_packets);
     map[n_packets];
    read_packets_from_file(fp, map);
    while (start_window<n_packets)
    {
        if (cw < window_size && end_window < n_packets)
        {
            send_packet(map[end_window].packet, sockfd, pservaddr);
            end_window++;
            cw++;
        }
        else
        {
            int status;
            Ack_packet ack_packet = recv_ack_packet(sockfd, pservaddr, 1, &status);
            if (status == 1)
            {
                if (ack_packet.ack_num == add)
                {
                    start_window++;
                    cw--;
                    add++;
                }
                else if (ack_packet.ack_num == remove)
                {
                    resend_packets(sockfd, pservaddr, lost_packets_array, lost_packets, map);
                    remove++;
                }
            }
            else if (status == 10)
            {
                resend_packets(sockfd, pservaddr, lost_packets_array, lost_packets, map);
            }
        }
    }
    
    


}
 int cw = 1;
    int add=1;
    int remove=2;
    int start_window = 0;
    int end_window = 0;
    int window_size = 10;
selective_repeat_recv_ack(){
   
    
    Map map[n_packets];
    read_packets_from_file(fp, map);
    int congestion_window = 1;

    while (start_window<n_packets)
    {
        for(int i=start_window;i<end_window;i++){
           mtx.lock();
           if()


            
        }
        if (cw < window_size && end_window < n_packets)
        {
            send_packet(map[end_window].packet, sockfd, pservaddr);
            end_window++;
            cw++;
        }
        else
        {
            int status;
            Ack_packet ack_packet = recv_ack_packet(sockfd, pservaddr, 1, &status);
            if (status == 1)
            {
                if (ack_packet.ack_num == add)
                {
                    start_window++;
                    cw--;
                    add++;
                }
                else if (ack_packet.ack_num == remove)
                {
                    resend_packets(sockfd, pservaddr, lost_packets_array, lost_packets, map);
                    remove++;
                }
            }
            else if (status == 10)
            {
                resend_packets(sockfd, pservaddr, lost_packets_array, lost_packets, map);
            }
        }
        mtx.unlock();
    }

}
resend_packets(int sockfd, struct sockaddr *pservaddr, int lost_packets_array[], int lost_packets){
    int i;
    for(i = 0;i < lost_packets;i++){
        Packet packet;
        packet.seq_num = lost_packets_array[i];
        packet.length = fread(packet.data, 1, maxbuffer, fp);
        send_packet(packet, sockfd, pservaddr);
    }
}
read_packets_from_file(FILE *fp, Packet packets[]){
    int i = 0;
    Packet packet;
    while (1)
    {
        packet.seq_num = i;
        packet.length = fread(packet.data, 1, maxbuffer, fp);
        packets[i] = packet;
        if (packet.length < maxbuffer)
            break;
        i++;
    }
    packet_numbers = i;
}
send_file(FILE *fp, int sockfd,  struct sockaddr *pservaddr, double prob_of_loss, int seednumber){
    int cw = 1;
    int add=1;
    int remove=2;
    int start_window = 0;
    int end_window = 0;
    int window_size = 10;
    int n_packets = get_number_of_packets();
    int lost_packets =ceil(n_packets * prob_of_loss);
    int lost_packets_array[lost_packets];
    pthread_t thread;
    thread = pthread_create(&thread, NULL, &selective_repeat_send_file, (void *)fp);
    thread = pthread_create(&thread, NULL, &selective_repeat_recv_ack, (void *)fp);
    pthread_join(thread, NULL);
    pthread_join(thread, NULL);

}
send_packets(int sockfd, struct sockaddr *pservaddr, Map map[], int start_window, int end_window,int lost_packets_array[]){
    int i;

    get_loss_packet(prob_of_loss, seednumber, lost_packets_array, lost_packets);

    for(i = start_window;i < end_window;i++){
        if(lost_packets_array[map[i].packet.seq_num] != map[i].packet.seq_num)
        {
            send_packet(map[i].packet, sockfd, pservaddr);
        }
    }
}