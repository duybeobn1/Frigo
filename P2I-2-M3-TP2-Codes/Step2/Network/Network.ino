#include <SPI.h>
#include "RF24.h"
#include "printf.h"
#include "Network.h"

const node_info topology[] =
{
  { 0x0000000000LL, 0x0000000000LL,-1 }, // Node 0 (Base)
  { 0xF0F0F0F0E1LL, 0x3A3A3A3AE1LL, 0 }, // Node 1 (Relay)
  { 0xF0F0F0F0D2LL, 0x3A3A3A3AD2LL, 0 }, // Node 2 (Relay)
  { 0xF0F0F0F0C3LL, 0x3A3A3A3AC3LL, 1 }, // Node 3 (Leaf)
  { 0xF0F0F0F0B4LL, 0x3A3A3A3AB4LL, 1 }, // Node 4 (Leaf)
  { 0xF0F0F0F0A5LL, 0x3A3A3A3AA5LL, 2 }, // Node 5 (Leaf)
};

RF24 radio(9,10);
uint8_t node_address = 0;                    // Adresse logique du noeud (index dans le tableau topology )
uint8_t ping_src = 3;
uint8_t ping_dest = 0;
uint16_t pck_id = 1;

const short num_nodes = sizeof(topology)/sizeof(node_info);

uint8_t next_hop( uint8_t current_node, uint8_t target_node )
{
  uint8_t out_node = target_node;
  bool found_target = false;
  while ( ! found_target )
  {
    if ( topology[out_node].parent_node == current_node )
    {     found_target = true; }
    else
    { out_node = topology[out_node].parent_node;
      if ( out_node == 0 || out_node == -1 )
      {
        out_node = topology[current_node].parent_node;
        found_target = true;
      }
    }
  }
  return out_node;
}

void payload_printf(unsigned long t,const char* name, const payload_t& pl)
{  
  printf("%lu %s Payload: %s %u (from %u to %u) \n",t,name,pl.type?"Pong":"Ping",pl.pkt_id,pl.src_node,pl.dest_node);  
}

typedef enum { role_invalid = 0, role_base, role_relay, role_leaf } role_e;
const char*  role_friendly_name[] = { "invalid", "Base", "Relay", "Leaf" };

role_e role;
static unsigned long last_ping_sent_at;
static bool waiting_for_pong = false;
const unsigned long ping_interval = 5000; // ms
const unsigned long pong_timeout = 1000;  // ms

void setup(void)
{  
  // Déterminer le role du noeud 
   if ( node_address == 0 )
            role = role_base;
    else
    {
      role = role_leaf;
      int i = num_nodes;
      while (i--)
      {
        if ( topology[i].parent_node == node_address )
        { role = role_relay;
          break;
        }
      }
    }
  
  Serial.begin(115200);
  printf_begin();
  printf("Node address: %i \r Role: %s\n",node_address, role_friendly_name[role]);

  radio.begin();
  radio.setChannel(0xZc);
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(15,15);
  radio.setPALevel(RF24_PA_MAX) ;

  // Ouvrir le pipe d'écoute avec le pere
  if ( role == role_leaf )
      radio.openReadingPipe(1,topology[node_address].listening_pipe); 
  if ( role == role_relay ) 
      radio.openReadingPipe(0,topology[node_address].listening_pipe);

  // Ouvrir les pipe d'écoute avec les fils
  if ( role == role_base || role == role_relay )
  {
    uint8_t current_pipe = 1;
    int i = num_nodes;
    while (i--)
    {
      if ( topology[i].parent_node == node_address )
          radio.openReadingPipe(current_pipe++,topology[i].talking_pipe);
    }
  }
  
  radio.startListening();
}

void loop(void)
{
  if (node_address == ping_src ) // Vous pouvez changer la condition (ex: role==role_leaf)
  {
    ping_if_ready();
    check_pong_timeout();
    while ( radio.available() ) // Réception de la réponse (pong)
    {
        payload_t payload;
        radio.read( &payload, sizeof(payload_t) );
        handle_packet(payload);  
    }
  }
  else 
  {
    uint8_t pipe_num;
    while ( radio.available(&pipe_num) )
    {
        payload_t payload;
        radio.read( &payload, sizeof(payload_t) ); 
        
        if ( payload.dest_node == node_address )        // Je suis le destinataire final
                 // A complter ********************
        else   
                 // A complter ********************
    }
  }
}

void ping_if_ready(void)
{
  unsigned long now = millis();
  if ( now - last_ping_sent_at >= ping_interval )
  {
    last_ping_sent_at = now;
    waiting_for_pong = true;

    payload_t ping(node_address,ping_dest, pck_id++,0, millis());
    uint8_t out_node = next_hop(node_address,ping.dest_node);  // next_hop
          
    radio.stopListening();
    uint64_t out_pipe;
    if ( topology[out_node].parent_node == node_address )  // Le prochain saut est un fils
        out_pipe = topology[out_node].listening_pipe;
    else
        out_pipe = topology[node_address].talking_pipe;    // Le prochain saut est un parent
    
    radio.openWritingPipe(out_pipe);
     
    payload_printf(millis(),"> Sending a PING \t",ping);
    
    radio.write( &ping, sizeof(payload_t) );  
    radio.startListening();
  }
}

void check_pong_timeout(void)
{
  if ( waiting_for_pong && ( millis() - last_ping_sent_at > pong_timeout ) )
  {
     waiting_for_pong = false;
     printf(" \t\t\t TIMED OUT at %lu \n\n",millis());
  }
}

void forward_packet(const payload_t& payload)
{
          uint8_t out_node = next_hop(node_address,payload.dest_node);  // next_hop
          
          radio.stopListening();
          uint64_t out_pipe;
          if ( topology[out_node].parent_node == node_address )  // Le prochain saut est un fils
              out_pipe = topology[out_node].listening_pipe;
          else
              out_pipe = topology[node_address].talking_pipe;    // Le prochain saut est un parent
          
          radio.openWritingPipe(out_pipe);
          bool ok = radio.write( &payload, sizeof(payload_t) );
          payload_printf(millis(), "> Packet to forward ",payload);
          printf("%lu > Forwarding packet %u to node %u : %s\n\n",millis(),payload.pkt_id, out_node, ok?"ok":"failed");
          radio.startListening();
}

void handle_packet(const payload_t& payload)
{
  if (payload.type==1)  // Reception de la réponse (Pong)
  {
        waiting_for_pong = false;
        payload_printf(millis(), "> PONG received \t",payload);
        printf(" \t\t\t Round-trip delay : %lu \n\n",millis()-payload.time);
  }
  else                  // Reception de la requete (Ping)
  {
        payload_printf(millis(), "> PING received \t",payload);
        payload_t pong(node_address,payload.src_node,payload.pkt_id,1,payload.time);
        forward_packet(pong);                
  }
}
