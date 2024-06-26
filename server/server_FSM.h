#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

#include <signal.h>
#include <pthread.h>
#include <errno.h>

# define maxbuffer 500

  struct pack ;
  struct ack_pack;
  
  typedef struct pack {
    uint16_t seq_num;
    uint16_t checksum;
    uint32_t length;
    char data[500];
} Packet;

 typedef struct ack_pack {
    uint16_t ack_num;
    uint16_t checksum;
    uint32_t length;
}Ack_packet;

struct map;
typedef struct map {
    Packet packet;
    int index;
    clock_t time;
}windowMap;

struct ackmap;
typedef struct ackmap {
    Ack_packet ack_packet;
    int index;
}ackMap;
int number_of_Packets;
double loss_prob;
int seed_num;
int time_out;

void send_packet( Packet pkt,int sockfd,  struct sockaddr *servaddr);
void send_ack_packet( Ack_packet ack,int sockfd, struct sockaddr *servaddr);
Packet recv_packet(int packet_num,int sockfd, struct sockaddr *servaddr );
Ack_packet recv_ack_packet(int sockfd, struct sockaddr *servaddr, int time_out, int* status);
void get_loss_packet(double prob_of_loss, int seednumber, int lost_packets_array[]);
void send_file(FILE *filename, int sockfd, struct sockaddr *servaddr);
void recv_file(FILE *filename, int sockfd, struct sockaddr *servaddr);
void send_file_by_window(FILE *fp, int sockfd,  struct sockaddr *pservaddr);
int get_size(FILE* file);
int check_packet_in_lost_packets(int lost_packets_array[], int lost_packets, int packet_num);
int check_timeout(clock_t t1);
int max(int x, int y);
int min(int x, int y);


