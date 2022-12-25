struct Packet {
    int seq_num;
    int checksum;
    int length;
    char data[500];
};

struct Ack_packet {
    int ack_num;
    int checksum;
    int length;
};

void send_packet(struct Packet *pkt,int sockfd, struct sockaddr *servaddr);
void send_ack_packet(struct Ack_packet *ack,int sockfd, struct sockaddr *servaddr);
Packet recv_packet(int sockfd, struct sockaddr *servaddr, );
Ack_packet recv_ack_packet(int sockfd, struct sockaddr *servaddr);
void get_loss_packet(double prob_of_loss, int seednumber);
void sand_file(File *filename, int sockfd, struct sockaddr *servaddr, int servlen);
void recv_file(File *filename, int sockfd, struct sockaddr *servaddr, int servlen);

