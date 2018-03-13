/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>
#include <ArduinoJson.h>


// Opsætning af netværk
const char* ssid = "ASUSA2_13";
const char* password = "raspberry";
//const char* ssid = "Blichersvej 28";
//const char* password = "bombur8210";
const char* mqtt_server = "Blichersvej28.asuscomm.com";


// Opsætning af client
WiFiClient espClient2;
PubSubClient client(espClient2);
long lastMsg = 0;
char msg[50];
int value = 0;

// Opsætning MQTT
const char* outTopic = "Device";
const char* outTopicTest = "Device2";
const char* inTopic = "Sonoff1in";

// Definer HC-SR04 samt ben nr (Trig,Echo)
Ultrasonic ultrasonic(4, 5);

// Lav en long til at gemme tiden
unsigned long next_refresh = 0;

void setup_wifi() {

  delay(1000);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// Se om der er MQTT besked 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Hvis MQTT bliver smidt af reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client222")) {
      Serial.println("connected");

      
      // Once connected, publish an announcement...
      client.publish(outTopic, "{\"Device1\":\"CONNECTED\"}");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      for(int i = 0; i<5000; i++){
      delay(1);
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if( (long)(millis() - next_refresh) >= 0)
  {
  String Payload;
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& JsonData = jsonBuffer.createObject();
  JsonData["Device"] = "Device2";
  JsonData["Type"] = "Afstand";
  JsonData["Units"] = "mm";
  JsonData["Data"] = ultrasonic.distanceRead();
  JsonData.printTo(Payload);
  Serial.println();
  JsonData.prettyPrintTo(Serial);
  client.publish(outTopicTest, (char*) Payload.c_str());

  next_refresh = millis() + 10000;
  }
  
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


