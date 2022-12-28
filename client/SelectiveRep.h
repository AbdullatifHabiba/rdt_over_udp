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