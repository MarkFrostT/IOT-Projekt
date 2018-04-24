void Matrix() {
  int pass = 0;
  int x    = matrix.width();
  uint16_t colors[] = {
  matrix.Color(255,0,0),matrix.Color(0,255,0),matrix.Color(0,0,255) };
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  String OutString = message;
  matrix.print(OutString);
  int Value = OutString.length();
  Value = Value * -1 ;
  //Serial.println(x);
  if(--x < (-20+Value*6)) {
    x = matrix.width();
    
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(100);
}
