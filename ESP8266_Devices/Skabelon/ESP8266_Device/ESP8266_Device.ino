/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h> // ###
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>


// Opsætning af netværk
const char* ssid = "MarkFrostPi";
const char* password = "Skoleprojekt";
// Opsætning af MQTT Broker
const char* mqtt_server = "192.168.5.1";
const int mqtt_port = 1883;

// OTA fra HTTP Server
const int FW_VERSION = 3;
const char* fwUrlBase = "http://192.168.5.1/Firmware/";

// Opsætning af Wifi client
WiFiClient espClient4; // ###
// Opsætning af MQTT client
PubSubClient client(espClient4); // ###
// Opsætning Http client
HTTPClient httpClient;

// Definer HC-SR04 samt ben nr (Trig,Echo) 
Ultrasonic ultrasonic(4, 5); 
// Output
const int ledPin = 14 ;
int ledState = LOW;  

// Opsætning MQTT Topics
const char* outTopic = "Device4out";
const char* outTopicAlive = "Device4Alive";
const char* inTopic = "Device4in";


// Global Variable
// Lav en long til at gemme tiden
unsigned long next_refresh = 0;
// Mqtt CallBack
String message;
// JSON Data 
String Device = "Device4";
String Type;
String Unit;
String Data;

void setup() { 
  // Disable AP Mode 
  WiFi.mode(WIFI_STA);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  setup_wifi();                   // Connect to wifi 
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if( (long)(millis() - next_refresh) >= 0)
  {
  Create_Data_Packet();
  Publish_Output(Device,Type,Unit,Data);
  checkForUpdates();
  next_refresh = millis() + 10000;
  }
  
}









