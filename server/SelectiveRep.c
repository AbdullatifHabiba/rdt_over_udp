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

void send_file (char *name, int sock) {
    FILE* file = fopen (name, "rb");

    int size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    printf("size = %d\n", size);    
    send(sock, &size, sizeof(int), 0);
    
    char* data = (char*) malloc(sizeof(char) * size);
    fread(data, 1, size, file);

    int num_blocks = size / BLOCK_SIZE;
    printf("%d\n", size);
    printf("%d\n", BLOCK_SIZE);
    printf("%d\n", num_blocks);
    int lastBlockSize = size % BLOCK_SIZE;
    int offset = 0;

    while (num_blocks > 0)
    {
        send(sock, data + (offset * BLOCK_SIZE), BLOCK_SIZE, 0);
        offset++;
        num_blocks --;
    }

    send(sock, data + offset * BLOCK_SIZE, lastBlockSize, 0);
    fclose(file);
    free(data);
    printf("finish sending\n");
}