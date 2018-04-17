/*

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <ArduinoJson.h>


// Opsætning af netværk
const char* ssid = "MarkFrostPi";
const char* password = "Skoleprojekt";
const char* mqtt_server = "192.168.5.1";

const int FW_VERSION = 1244;
const char* fwUrlBase = "http://192.168.5.1/Firmware/";

// Opsætning af client
WiFiClient espClient2;
PubSubClient client(espClient2);
long lastMsg = 0;
char msg[50];
int value = 0;

// Opsætning MQTT
const char* outTopic = "Device3int";
const char* outTopicTest = "Device3";
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
  JsonData["Units"] = "cm";
  JsonData["Data"] = ultrasonic.distanceRead();
  JsonData.printTo(Payload);
  Serial.println();
  JsonData.prettyPrintTo(Serial);
  client.publish(outTopicTest, (char*) Payload.c_str());
  checkForUpdates();
  next_refresh = millis() + 10000;
  }
  
}
void checkForUpdates() {
  String mac = getMAC();
  String fwURL = String( fwUrlBase );
  fwURL.concat( mac );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( ".version" );

  Serial.println( "Checking for firmware updates." );
  Serial.print( "MAC address: " );
  Serial.println( mac );
  Serial.print( "Firmware version URL: " );
  Serial.println( fwVersionURL );

  HTTPClient httpClient;
  httpClient.begin( fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware version3 " );
    Serial.println( FW_VERSION );
    Serial.print( "Available firmware version3: " );
    Serial.println( newFWVersion );

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      Serial.println( "Preparing to update" );

      String fwImageURL = fwURL;
      fwImageURL.concat( ".bin" );
      t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
      }
    }
    else {
      Serial.println( "Already on latest version" );
    }
  }
  else {
    Serial.print( "Firmware version check failed, got HTTP response code " );
    Serial.println( httpCode );
  }
  httpClient.end();
}

String getMAC()
{
  uint8_t mac[6];
  char result[14];

 snprintf( result, sizeof( result ), "%02x%02x%02x%02x%02x%02x", mac[ 0 ], mac[ 1 ], mac[ 2 ], mac[ 3 ], mac[ 4 ], mac[ 5 ] );

  return String( result );
}


