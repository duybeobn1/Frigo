#include <SPI.h>
#include <RF24.h>
#include "packet.h"
#include <dht.h>
#include <Multichannel_Gas_GMXXX.h>
#include <Wire.h>
GAS_GMXXX<TwoWire> gas;
//led
int RED = 2;
int GREEN = 3;
//relay
int pinRelay = 5; //pin 5
// dht11
dht DHT;
#define DHT11_PIN 7
// o2
const float VRefer = 3.3;       // voltage of adc reference
const int pinAdc   = A0;

// rf
RF24 radio(9, 10);
RF24 radio2(7, 8);
uint8_t address[6] = {0x5C, 0xCE, 0xCC, 0xCE, 0xCC}; // Adresse du pipe
int val = 1; // Valeur à envoyer
payload_t payload;
payloadLimit_t payloadLimit;

void setup() {
  Serial.begin(115200);    // Initialiser la communication série
  pinMode(RED, OUTPUT);
  digitalWrite(RED, HIGH);
  pinMode(GREEN, OUTPUT);
  digitalWrite(GREEN, HIGH);
  pinMode(pinRelay, OUTPUT);
  digitalWrite(pinRelay, LOW);
  Serial.println (F("Starting my first test")) ;
  // tx to rx
  radio.begin();
  radio.setChannel(0x5c);
  radio.setDataRate(RF24_2MBPS);
  //rx to tx
  radio2.begin();
  radio2.setChannel(100);
  radio2.setDataRate(RF24_2MBPS);
  //tx to rx
  radio.openWritingPipe(address);    // Ouvrir le Pipe en écriture
  radio.stopListening();
  //rx to tx
  radio2.openReadingPipe(0, address);
  radio2.startListening();

  // capteur CO
  gas.begin(Wire, 0x08); // use the hardware I2C
}

void loop(void) {
  int chk = DHT.read11(DHT11_PIN);
  // temp + humid
  Serial.print("temp=");
  Serial.println(DHT.temperature);
  Serial.print("humid=");
  Serial.println(DHT.humidity);
  //O2
  float Vout = 0;
  Vout = readO2Vout();
  Serial.print("conO2=");
  Serial.println(readConcentration());

  //CO2
  int co = gas.getGM702B();
  Serial.print("conCO2=");
  Serial.println(co);


  payload.o2 = readConcentration();
  payload.humid = DHT.humidity;
  payload.temp = DHT.temperature;
  payload.co2 = co;


  //temperature
  if (payload.temp < payloadLimit.limTemp) {
    turnOff(pinRelay);
  }
  else {
     turnOn(pinRelay);
  }
//  //humidity
//  if (payload.humid < payloadLimit.limHumid) {
//    turnOn(pinRelay);
//  }
//  else {
//    turnOff(pinRelay);
//  }
//  //o2
//  if (payload.o2 < payloadLimit.limO2) {
//    turnOff(pinRelay);
//  }
//  else {
//    turnOn(pinRelay);
//  }
//  //co2
//  if (payload.co2 < payloadLimit.limCO2) {
//    turnOn(pinRelay);
//  }
//  else {
//    turnOff(pinRelay);
//  }

  while (radio2.available())
  {
    radio2.read(&payloadLimit, sizeof(payloadLimit));
    Serial.print("Received PresetTemperature=");
    Serial.println(payloadLimit.limTemp);
    Serial.print("Received PresetHumidity=");
    Serial.println(payloadLimit.limHumid);
    Serial.print("Received PresetO2=");
    Serial.println(payloadLimit.limO2);
    Serial.print("Received PresetCO2=");
    Serial.println(payloadLimit.limCO2);
  }


  //sending to Rx
  val++;
  Serial.print(F("\n Now sending Packet "));
  Serial.print(val);

  if (radio.write(&payload, sizeof(payload)))
  {
    Serial.print(F(" ... Ok ...\n"));
  }
  else
  {
    Serial.print(F(" ... failed ...\n"));
  }
  delay(5000);
}

float readO2Vout()
{
  long sum = 0;
  for (int i = 0; i < 32; i++)
  {
    sum += analogRead(pinAdc);
  }

  sum >>= 5;

  float MeasuredVout = sum * (VRefer / 1023.0);
  return MeasuredVout;
}

float readConcentration()
{
  // Vout samples are with reference to 3.3V
  float MeasuredVout = readO2Vout();

  //float Concentration = FmultiMap(MeasuredVout, VoutArray,O2ConArray, 6);
  //when its output voltage is 2.0V,
  float Concentration = MeasuredVout * 0.21 / 2.0;
  float Concentration_Percentage = Concentration * 100;
  return Concentration_Percentage;
}
void turnOn (int pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
}
void turnOff (int pin) {
  digitalWrite(pin, LOW);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
}
