String operation ;
int RelayPin = 5;

void setup() {
  Serial.begin(115200);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, LOW);

}
void loop() {
  //Read from Serial if Data is Available
  if (Serial.available() > 0)
  {
    operation = Serial.readString();
    operation.replace("\n", "");
    Serial.println(operation);
    if (operation.toDouble() > 20) {
      digitalWrite(RelayPin, LOW);
      Serial.println("open");
    }
    //LED On
    else {
      digitalWrite(RelayPin, HIGH);
      Serial.println("close");
    }
  }
  //LED Off


}
