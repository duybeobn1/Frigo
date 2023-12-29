    #include <dht.h>

// dht11
dht DHT;
#define DHT11_PIN 7
// o2
const float VRefer = 3.3;       // voltage of adc reference
const int pinAdc   = A0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("temp=");
  Serial.println(DHT.temperature);
  Serial.print("humid=");
  Serial.println(DHT.humidity);

  float Vout = 0;
  Vout = readO2Vout();
  Serial.print("con=");
  Serial.println(readConcentration());
  delay(10000);
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
