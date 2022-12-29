
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
    int flag;
    clock_t time;
}Map;

