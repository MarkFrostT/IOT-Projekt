/*

*/
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>

#define DHTTYPE DHT11
#define DHTPIN  5

DHT dht(DHTPIN, DHTTYPE);

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
WiFiClient espClient1;
// Opsætning af MQTT client
PubSubClient client(espClient1);
// Opsætning Http client
HTTPClient httpClient;

// Opsætning MQTT Topics
const char* outTopic = "Device1out";
const char* outTopicAlive = "Device1Alive";
const char* inTopic = "Device1in";


// Global Variable
// Lav en long til at gemme tiden
unsigned long next_refresh = 0;
// Mqtt CallBack
String message;
// JSON Data 
String Device = "Device1";
String Type;
String Unit;
String Data;

void setup() { 
  // Disable AP Mode 
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  setup_wifi();  // Connect to wifi 
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









