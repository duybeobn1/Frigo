#include  <SPI.h>
#include  <RF24.h>
#include  "packet.h"

RF24 radio(9, 10);
RF24 radio2(7, 8);
uint8_t address[6] = {0x5C, 0xCE, 0xCC, 0xCE, 0xCC}; // Adresse du pipe
int rcv_val = 0;
payload_t payload;
payloadLimit_t payloadLimit;

//seperate 4 limits
String stringFromJava, getTemp, getHumid, getO2, getCO2, phrase1, phrase2;
byte index, index1, index2;

void setup() {
  Serial.begin(115200);    // Initialiser la communication série
  Serial.println (F("Starting my first test")) ;
  //tx to rx
  radio.begin();
  radio.setChannel(0x5c);
  radio.setDataRate(RF24_2MBPS);
  //rx to tx
  radio2.begin();
  radio2.setChannel(100);
  radio2.setDataRate(RF24_2MBPS);

  //tx to rx
  radio.openReadingPipe(0, address); // Ouvrir le Pipe en lecture
  radio.startListening();
  //rx to tx
  radio2.openWritingPipe(address);
  radio2.stopListening();

  // init payload limit
  payloadLimit.limTemp = 25;
  payloadLimit.limHumid = 30;
  payloadLimit.limO2 = 14;
  payloadLimit.limCO2 = 350;
}

void loop(void) {
  while (radio.available())
  {
    radio.read(&payload, sizeof(payload));
    Serial.print(F("Received from Tx : \n"));
    Serial.print("temp=");
    Serial.println(payload.temp);
    Serial.print("humid=");
    Serial.println(payload.humid);
    Serial.print("conO2=");
    Serial.println(payload.o2);
    Serial.print("conCO2=");
    Serial.println(payload.co2);
  };
  
  //seperate string from java
  if (Serial.available()) {
    stringFromJava = Serial.readString();
    for (int i = 0; i < stringFromJava.length(); i++) {
      if (stringFromJava.charAt(i) == '|') {
        index = i;
      }
    }
    phrase1 = stringFromJava;
    phrase2 = stringFromJava;
    phrase1.remove(index);
    phrase2.remove(0, index + 1);

    for (int i = 0; i < phrase1.length(); i++) {
      if (phrase1.charAt(i) == ',') {
        index1 = i;
      }
    }
    getTemp = phrase1;
    getHumid = phrase1;
    getTemp.remove(index1);
    getHumid.remove(0, index1 + 1);
    payloadLimit.limTemp = getTemp.toFloat();
    payloadLimit.limHumid = getHumid.toFloat();

    for (int i = 0; i < phrase2.length(); i++) {
      if (phrase2.charAt(i) == ',') {
        index2 = i;
      }
    }
    getO2 = phrase2;
    getCO2 = phrase2;
    getO2.remove(index2);
    getCO2.remove(0, index2 + 1);
    payloadLimit.limO2 = getO2.toFloat();
    payloadLimit.limCO2 = getCO2.toFloat();
  }

//  Serial.print("PresetTemperature=");
//  Serial.println(payloadLimit.limTemp);
//  Serial.print("PresetHumidity=");
//  Serial.println(payloadLimit.limHumid);
//  Serial.print("PresetO2=");
//  Serial.println(payloadLimit.limO2);
//  Serial.print("PresetCO2=");
//  Serial.println(payloadLimit.limCO2);

  if (radio2.write(&payloadLimit, sizeof(payloadLimit)))
  {
    Serial.print(F("Success\n"));
  }
  else
  {
    Serial.print(F(" Failed\n"));
  }
  delay(5000);


}
