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
#include <poll.h>
#include "server_FSM.h"

int sand_index = 0;

Packet recv_packet(int packet_num, int sockfd, struct sockaddr *pservaddr)
{
    Packet packet;
    int n;
    socklen_t len;
    len = sizeof(*pservaddr);
    n = recvfrom(sockfd, &packet, sizeof(packet), MSG_WAITALL, pservaddr, &len);
    if (n < 0 || n != sizeof(Packet))
    {
        perror("ERROR in recvfrom");
        return packet;
    }
    if (packet.seq_num != packet_num)
        perror("ERROR: packet out of order");
    return packet;
}

Ack_packet recv_ack_packet(int sockfd, struct sockaddr *pservaddr, int time_out, int *status)
{
    int flags = fcntl(sockfd, F_GETFL);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    clock_t start = clock();
    while ((clock() - start) / CLOCKS_PER_SEC < time_out)
    {
        Ack_packet ack_packet;
        int n;
        socklen_t len;
        len = sizeof(*pservaddr);
        n = recvfrom(sockfd, &ack_packet, sizeof(ack_packet), MSG_WAITALL, pservaddr, &len);
        if (n > 0)
        {
            *status = 1;
            return ack_packet;
        }
    }
    *status = 0;
    Ack_packet ack_packet2;
    return ack_packet2;
}

int tmp = 0;

Ack_packet recv_ack_packet_sel(int sockfd, struct sockaddr *pservaddr, int time_out, int *state)
{
    struct pollfd pfd = {.fd = sockfd, .events = POLLIN};
    Ack_packet ack_packet;
    int n;
    socklen_t len;
    len = sizeof(*pservaddr);
    if (poll(&pfd, 1, 1000) != 0)
    {
        n = recvfrom(sockfd, &ack_packet, sizeof(ack_packet), MSG_WAITALL, pservaddr, &len);
        tmp = ack_packet.ack_num;
        return ack_packet;

    }
    else{
      printf("time out\n");
        *state=0;
        ack_packet.ack_num = tmp;
    }
    

    return ack_packet;
}

void send_packet(Packet packet, int sockfd, struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, &packet, sizeof(packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0)
        perror("ERROR in sendto packet");
}

void send_ack_packet(Ack_packet ack_packet, int sockfd, struct sockaddr *pservaddr)
{
    int n;
    n = sendto(sockfd, (void *)&ack_packet, sizeof(ack_packet), MSG_CONFIRM, pservaddr, sizeof(*pservaddr));
    if (n < 0)
        perror("ERROR in sendto ack packet");
}

int get_number_of_packets()
{
    return number_of_Packets;
}

void get_loss_packet(double prob_of_loss, int seednumber, int lost_packets_array[])
{
    srand(seednumber);
    int n_packets = number_of_Packets;
    int i;
    int lost_packets = ceil(n_packets * prob_of_loss);
    lost_packets_array[lost_packets];
    for (i = 0; i < lost_packets; i++)
    {
        double random = (double)(rand() % n_packets);
        if (lost_packets_array[i] == random)
        {
            i--;
        }
        else
        {
            lost_packets_array[i] = random;
        }
    }
}

void send_file(FILE *fp, int sockfd, struct sockaddr *pservaddr)
{
    int lost[(int)ceil(number_of_Packets * loss_prob)];
    get_loss_packet(loss_prob, seed_num, lost);
    int size = sizeof(lost) / sizeof(lost[0]);
    printf("lostsize= %d\n", size);

    Packet packet;
    int packet_num = 0;
    int ack_num = 0;
    int n;
    int status = 0;
    time_out = 1;
    while (1)
    {
        packet.seq_num = packet_num;
        packet.length = fread(packet.data, 1, maxbuffer, fp);
        Ack_packet ack_p;
        if (packet.length == 0)
            break;

        if (status != 1)
        {
            int ch = check_packet_in_lost_packets(lost, size, packet_num);
            if (ch == 1)
            {
                recv_ack_packet(sockfd, pservaddr, time_out, &status);
            }
        }

        send_packet(packet, sockfd, pservaddr);
        ack_p = recv_ack_packet(sockfd, pservaddr, time_out, &status);

        status = 0;
        printf("sand %d rec_ack=%d\n", packet_num, ack_p.ack_num);
        if (ack_p.ack_num != packet_num)
        {
            perror("ERROR: ack out of order");
        }
        packet_num++;
    }
    packet.seq_num = packet_num;
    packet.length = 0;
    send_packet(packet, sockfd, pservaddr);
}

int check_packet_in_lost_packets(int lost_packets_array[], int lost_packets, int packet_num)
{
    int i;
    for (i = 0; i < lost_packets; i++)
    {
        if (lost_packets_array[i] == packet_num)
        {
            lost_packets_array[i] = -1;
            return 1;
        }
    }
    return 0;
}

int get_size(FILE* file)
{
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);
    return size;
}

void read_packets_from_file(FILE *fp, Packet packets[])
{
    int i = 0;
    while (1)
    {
        packets[i].seq_num = i;
        packets[i].length = fread(packets[i].data, 1, maxbuffer, fp);
        if (packets[i].length == 0)
            break;
        i++;
    }
}

int dub_ack = 0;
int cwnd = 1;
int ssthread = 64;
int add = 1;
int dec = 2;
int start_window = 0;
int end_window = 1;
int state = 1;
int congestion = 0;
int window_size = 5;
int sockfd;
FILE* fp;
windowMap window;
ackMap acked;

void send_file_by_window(FILE *fp, int sockfd, struct sockaddr *pservaddr)
{
    Packet allPackets[number_of_Packets];
    read_packets_from_file(fp, allPackets);
    int sizep = sizeof(allPackets) / sizeof(allPackets[0]);
    printf("pac = %d\n", sizep);

    // get lost packets
    int lost[(int) ceil(number_of_Packets * loss_prob)];
    get_loss_packet(loss_prob, seed_num, lost);
    int size = sizeof(lost) / sizeof(lost[0]);
    printf("lostsize = %d\n", size);
    while (start_window <= number_of_Packets)
    {
        printf("start window = %d   end window = %d\n", start_window, end_window);
        for (int index = start_window; index < end_window; index++)
        {
            Packet packet = allPackets[index];
            // los
            int ch = check_packet_in_lost_packets(lost, size, index);
            if (ch == 1)
            {
                printf("wl = %d \n", window.packet.seq_num);
            }
            else
            {
                send_packet(packet, sockfd, pservaddr);
            }
            start_window++;
        }
        
        Ack_packet ac = recv_ack_packet_sel(sockfd, pservaddr, time_out, &state);
        printf("state %d ackk %d\n",state, ac.ack_num);
        start_window = ac.ack_num;
        // check time out
        if (state == 0 )
        {
            state = 1;
            cwnd = 1;
        }
        else
        {
            // check congestion
            if (cwnd < ssthread)
            {
                cwnd *= 2;
            }
            else if (cwnd > ssthread && cwnd < number_of_Packets)
            {
                cwnd++;
            }
            else
            {
                cwnd /= 2;
            }            
        }
        end_window = min(start_window + cwnd, number_of_Packets + 1);
    }    
}

int check_timeout(clock_t t1)
{
    int flags = fcntl(sockfd, F_GETFL);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    return ((clock() - t1) / CLOCKS_PER_SEC > time_out);
}

int min(int x, int y)
{
    if (x > y)
        return y;
    return x;
}

int max(int x, int y)
{
    if (x > y)
        return x;
    return y;
}