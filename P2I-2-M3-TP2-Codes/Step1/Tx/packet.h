
#ifndef __PACKET_H__
#define __PACKET_H__

typedef struct payload_t
{
  unsigned long time;
  float co2;
  float temp;
  float humid;
  float o2;
} payload_t;

typedef struct payloadLimit_t
{
  float limTemp;
  float limHumid;
  float limO2;
  float limCO2;
} payloadLimit_t;

#endif
