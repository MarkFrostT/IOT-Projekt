
// Indsæt Sensor data 
void Create_Data_Packet () {
   Type = "Temperatur";
   Unit = "C";
   Data = (String)dht.readTemperature();
  
}

void Publish_Output(String device ,String type ,String unit,String data ) {
  String Payload;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& JsonData = jsonBuffer.createObject();
  JsonData["Device"] = device;
  JsonData["Type"] = type;
  JsonData["Units"] = unit;
  JsonData["Data"] = data;
  JsonData.printTo(Payload);
  Serial.print(Payload);
  client.publish(outTopic, (char*) Payload.c_str());
}
/*
JSON TELEGRAM
{
"Device" : "Device3",
"Type" : "Afstand",
"Units" : "mm",
"Data" : 678
}
*/
