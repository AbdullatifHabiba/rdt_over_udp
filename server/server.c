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
#include "StopAndWait.h"
#include <math.h>
#include "SelectiveRep.h"


int main(void){
    FILE *fp = fopen("server.in", "r");
      if(fp == NULL) {
          perror("Unable to open file!");
         exit(1);
    }
    char *line = NULL;
    size_t len = 0;
    int port;
    int seed;
    double prob;
 
    getline(&line, &len, fp);
    sscanf(line, "%d", &port);
    getline(&line, &len, fp);
    sscanf(line, "%d", &seed);
    getline(&line, &len, fp);
    sscanf(line, "%lf", &prob);
    fclose(fp);
    free(line); 
    printf("port = %d ",port);
    printf("seed = %d ",seed);
    printf("port = %lf ",prob);
    
    int socket_server;
    // Create UDP socket:
    socket_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if(socket_server < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");
    
    int opt = 1;
    if (setsockopt(socket_server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt Error");
        exit(1);
    }
 
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if(bind(socket_server, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");
        
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    int type;
    loss_prob=prob;
    seed_num=seed;
    printf("choose type of communication 1:stopandwait 2:selectiveRepeat = ");

    //  scanf("%d",&type);

    // if(type==1){
    //     Packet pak=   recv_packet(0,socket_server,(struct sockaddr*)&clientAddress); 
    //     printf("%s\n",pak.data);
    //     Ack_packet ackk;
    //     FILE*fp=fopen (pak.data, "rb");
    //     int s= (int)ceil (1.0*get_size(fp)/500);
    //     ackk.ack_num= s;
    //     ackk.checksum=0;
    //     ackk.length=0;
    //     number_of_Packets=s;
    //     send_ack_packet(ackk,socket_server,(struct sockaddr*)&clientAddress);
    //     send_file(fp,socket_server,(struct sockaddr*)&clientAddress);
    //     }
    //     else if(type ==2)
    //     {
        Packet pak=   recv_packet(0,socket_server,(struct sockaddr*)&clientAddress); 
        printf("%s\n",pak.data);
        Ack_packet ackk;
        FILE*fp1=fopen (pak.data, "rb");
        int s= (int)ceil (1.0*get_size(fp1)/500);
        ackk.ack_num= s;
        ackk.checksum=0;
        ackk.length=0;
        number_of_Packets=s;
        send_ack_packet(ackk,socket_server,(struct sockaddr*)&clientAddress);
        send_file_by_window(fp1,socket_server,(struct sockaddr*)&clientAddress);
        
    //     }
    //     else{
    //         printf("choose correct type");
    //         exit(1);
    //     }
    // /* while (1)
   /* {
        memset(&clientAddress, 0, sizeof(clientAddress));
        pid_t pid = fork();

        if (pid == -1)
        {
            close(socket_server);
            continue;
        }
        if (pid == 0)
        {
            int type;
            scanf("choose type of communication 1:stopandwait 2:selectiveRepeat \n %d",&type);
            if(type==1){
                Packet pak=   recv_packet(0,socket_server,(struct sockaddr*)&clientAddress)   ; 
                printf("%s",pak.data);
                Ack_packet ackk;
                ackk.ack_num=1;
                ackk.checksum=0;
                ackk.length=0;
            
                send_ack_packet(ackk,socket_server,(struct sockaddr*)&clientAddress);
                //stop and wait
                // create packet to send to client
                //send packet to client
                //recieve ack packet from client    
            }
            else if(type ==2)
            {
                //selectiveRepeat
                // create packet to send to client
                //send packet to client
                //recieve ack packet from client   
            }
            else{
                printf("choose correct type");
                exit(1);
            }         
        }
        else
        {
            close(socket_server);
        }
    }*/
    
    return 0;
}