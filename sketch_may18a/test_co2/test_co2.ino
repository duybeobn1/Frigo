#include <Multichannel_Gas_GMXXX.h>
#include <Wire.h>
GAS_GMXXX<TwoWire> gas;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); 
  gas.begin(Wire, 0x08); // use the hardware I2C
}
 
void loop() {
  // put your main code here, to run repeatedly:
  int val;

  // GM102B NO2 sensor
  val = gas.getGM102B();
  if (val > 999) val = 999;
  // GM302B C2H5CH sensor
  Serial.println("C2H5CH");
  Serial.println(val);
  val = gas.getGM302B();
  if (val > 999) val = 999;
  // GM502B VOC sensor
  val = gas.getGM502B();
  if (val > 999) val = 999;
  // GM702B CO sensor
  val = gas.getGM702B();
  if (val > 999) val = 999;
 
  delay(2000);
 
}
