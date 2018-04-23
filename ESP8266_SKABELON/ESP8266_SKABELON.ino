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
// Opsætning af MQTT Broker
const char* mqtt_server = "192.168.5.1";
const int mqtt_port = 1883;

// OTA fra HTTP Server
const int FW_VERSION = 3;
const char* fwUrlBase = "http://192.168.5.1/Firmware/";

// Opsætning af Wifi client
WiFiClient espClient3;
// Opsætning af MQTT client
PubSubClient client(espClient3);
// Opsætning Http client
HTTPClient httpClient;
// Definer HC-SR04 samt ben nr (Trig,Echo)
Ultrasonic ultrasonic(16, 5);

// Global Variable
// Lav en long til at gemme tiden
unsigned long next_refresh = 0;
// Mqtt CallBack
String message;
// JSON Data 
String Device = "Device3";
String Type;
String Unit;
String Data;

// Output
const int ledPin = 4 ;
int ledState = LOW;  




// Opsætning MQTT Topics
const char* outTopic = "Device3out";
const char* outTopicAlive = "Device3Alive";
const char* inTopic = "Device3in";

// Connecting to a WiFi network
void setup_wifi() {

  delay(3000);
  
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

// Hvis MQTT bliver smidt af reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client3")) {
      Serial.println("connected");

      
      // Once connected, publish an announcement...
      client.publish(outTopicAlive, "{\"Device3\":\"CONNECTED\"}");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      //for(int i = 0; i<5000; i++){
      delay(5000);
      //}
    }
  }
}

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

// Indsæt Sensor data 
void Create_Data_Packet () {
   Type = "Ultralydsmåler";
   Unit = "mm";
   Data = (String)ultrasonic.distanceRead();
  
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

  httpClient.begin( fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware version " );
    Serial.println( FW_VERSION );
    Serial.print( "Available firmware version: " );
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



