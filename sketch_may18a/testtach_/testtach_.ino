String x = "12,13|14,15", chuoi1, chuoi2, so1, so2, so3, so4;
float val1, val2, val3, val4;
byte moc, moc1, moc2;
void setup() {
  Serial.begin(115200);

  for ( int i = 0; i < x.length(); i++) {
    if ( x.charAt(i) == '|') {
      moc = i;
    }
  }
  chuoi1 = x;
  chuoi2 = x;
  chuoi1.remove(moc);
  chuoi2.remove(0, moc + 1);


  for (int i = 0; i < chuoi1.length(); i++) {
    if ( chuoi1.charAt(i) == ',') {
      moc1 = i;
    }
  }
  so1 = chuoi1;
  so2 = chuoi1;
  so1.remove(moc1);
  so2.remove(0, moc1 + 1);
  for (int i = 0; i < chuoi2.length(); i++) {
    if ( chuoi1.charAt(i) == ',') {
      moc2 = i;
    }
  }
  so3 = chuoi2;
  so4 = chuoi2;
  so3.remove(moc2);
  so4.remove(0, moc2 + 1);
  Serial.println(so1);
//  Serial.println(so2);
//  Serial.println(so3);
//  Serial.println(so4);


}

void loop() {


}
