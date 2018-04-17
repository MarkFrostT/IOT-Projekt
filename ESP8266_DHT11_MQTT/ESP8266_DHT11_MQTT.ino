/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>


#define DHTTYPE DHT11
#define DHTPIN  5

DHT dht(DHTPIN, DHTTYPE);

// Opsætning af netværk
const char* ssid = "MarkFrostPi";
const char* password = "Skoleprojekt";
const char* mqtt_server = "192.168.5.1";


// Opsætning af client
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Opsætning MQTT
const char* outTopic = "Device1int";
const char* outTopicTest = "Device1";
const char* inTopic = "Sonoff1in";


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
    if (client.connect("ESP8266Client")) {
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
  dht.begin();
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
  JsonData["Device"] = "Device1";
  JsonData["Type"] = "Temperatur";
  JsonData["Units"] = "C";
  JsonData["Data"] = dht.readTemperature();
  JsonData.printTo(Payload);
  Serial.println();
  JsonData.prettyPrintTo(Serial);
  client.publish(outTopic, (char*) Payload.c_str());

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


