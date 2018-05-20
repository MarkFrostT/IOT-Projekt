// Se om der er MQTT besked 
void callback(char* topic, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  message = (char*)payload;
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  // Output Code
  if (message == "1") {
      ledState = HIGH;
      Serial.print("Led high");
  } else {
      ledState = LOW;
      Serial.print("Led low");
  }
  digitalWrite(ledPin, ledState);
  Serial.println();
}
