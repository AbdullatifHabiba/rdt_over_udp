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
#include "SelectiveRep.h"

int main(void){
    printf("you are in main\n");
   FILE *fp = fopen("client.in", "r");
      if(fp == NULL) {
          perror("Unable to open file!");
         exit(1);
    }
    printf("file opened \n");

    char *line = NULL;
    size_t len = 0;
    int port;
    char* address = NULL;
    char* file = NULL;
 
    getline(&address, &len, fp);
    getline(&line, &len, fp);
    sscanf(line, "%d", &port);
    getline(&file, &len, fp);

    fclose(fp);
    free(line);     

    printf("port = %d ",port);
    printf("address = %s ",address);
    printf("file = %s \n ",file);

    int number_of_packets;
    int socket_client;
    // Create socket:
    socket_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_client < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(address);

    //connect to server
    if (connect(socket_client, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        printf("connection error\n");
        exit(1);
    }
    
    int type;
    printf("choose type of communication 1:stopandwait 2:selectiveRepeat = ");
    scanf("%d",&type);
    if(type==1){
        Packet packet;
        packet.checksum = 0;
        packet.length = 0;
        packet.seq_num = 0;
        strcpy(packet.data, file);
        send_packet(packet,socket_client,(struct sockaddr *)&server_addr);
        Ack_packet ack_p = recv_ack_packet(socket_client,(struct sockaddr *)& server_addr);

        number_of_packets= ack_p.ack_num;

        printf(" number of packets= %d\n",number_of_packets);
        FILE*out= fopen(file,"wb");
        recv_file(out,socket_client,(struct sockaddr *)& server_addr);
    }
    else if(type ==2)
    {
        
    }
    else{
        printf("choose correct type");
        exit(1);
    }
    
    
    close(socket_client);
    
    return 0;
}
