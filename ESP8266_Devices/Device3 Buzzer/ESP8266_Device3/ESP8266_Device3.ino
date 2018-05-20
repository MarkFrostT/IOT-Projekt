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
WiFiClient espClient3; // ###
// Opsætning af MQTT client
PubSubClient client(espClient3); // ###
// Opsætning Http client
HTTPClient httpClient;


// Output
const int ledPin = 5 ; // ###
int ledState = LOW;    // ###

// Opsætning MQTT Topics
const char* outTopic = "Device3out"; // ###
const char* outTopicAlive = "Device3Alive";// ###
const char* inTopic = "Device3in";// ###


// Global Variable
// Lav en long til at gemme tiden
unsigned long next_refresh = 0;
// Mqtt CallBack
String message;
// JSON Data 
String Device = "Device3"; // ###
String Type;
String Unit;
String Data;

void setup() { 
  // Disable AP Mode 
  WiFi.mode(WIFI_STA);
  pinMode(ledPin, OUTPUT); // ###
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
  //Create_Data_Packet(); // ###
  //Publish_Output(Device,Type,Unit,Data); // ###
  checkForUpdates();
  next_refresh = millis() + 10000;
  }
  
}









