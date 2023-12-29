#ifndef __NETWORK_H__
#define __NETWORK_H__

struct node_info
{
  uint64_t talking_pipe;    // Pipe used to talk to parent node
  uint64_t listening_pipe;  // Pipe used to listen to parent node
  uint8_t parent_node;      // Address of parent node
};


typedef struct payload_t
{
  uint8_t src_node;
  uint8_t dest_node;
  uint16_t pkt_id;
  bool type;
  unsigned long time;
  
  
  uint16_t data;
  
  payload_t(void) {}
  payload_t(uint8_t _from, uint8_t _to,  uint16_t _id, bool _type,  const unsigned long& _time): src_node(_from), dest_node(_to), pkt_id(_id), type(_type), time(_time) {}
} payload_t;

#endif //

