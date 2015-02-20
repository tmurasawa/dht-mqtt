#include <PubSubClient.h>
#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>


// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

// dtostrf buffer
char s[16];
char t[16];

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 9);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient Ethclient;


/*
  AM2302 Temperature / Humidity Sensor (DHT22)

  Current Code
  Created by Derek Erb 30/01/2013
  Modified 30/01/2013

  Requirement : Arduino + AM2302 sensor connected to pin 2

  DHT Library from:
  https://github.com/adafruit/DHT-sensor-library

  v0.03 Delays and formatting
  v0.02 Delay 3 seconds between readings
*/

// Version number
const float fVerNum = 0.03;

// Data pin connected to AM2302
#define DHTPIN 2

#define DHTTYPE DHT22       // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);   // LED pins

// scalenics
#define DEVICE_TOKEN "YOUR_DEVICETOKEN_HERE"
#define DEVICE_ID "arduino01"
#define MQTT_SERVER "api.scalenics.io"
// MQTT client
PubSubClient client2(MQTT_SERVER, 1883, callback, Ethclient);

String topic;
String PostData;
char mqtt_topic[128];
char mqtt_payload[64];


// No callback
void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
    dht.begin();

  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    Ethernet.begin(mac, ip);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
}


void loop()
{
  // Read DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Read DHT fail
  if (isnan(t) || isnan(h)) {
    Serial.println(F("Failed to read from DHT"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F(" %\t"));
    Serial.print(F("Temperature: "));
    Serial.print(t);
    Serial.println(F(" C"));
  }

  // give the Ethernet shield a second to initialize:
  // delay(1000);
  Serial.println("connecting...");

  // make topic (DEVICE_TOKEN/DEVICE_ID)
  topic = DEVICE_TOKEN;
  topic += "/";
  topic += DEVICE_ID;


  // serialize env_data to HTTP_POST strings

  PostData = "";
  PostData += "v=";
  PostData += dtostrf(t, 4, 1, s);
  PostData += "&v2=";
  PostData += dtostrf(h, 4, 1, s);
  // for debug
  Serial.println("DEVICE_TOKEN=");
  Serial.println(DEVICE_TOKEN);
  Serial.println("DEVICE_ID=");
  Serial.println(DEVICE_ID);
  Serial.println("POST strings=");
  Serial.println(PostData);


  // if you get a connection, report back via serial:
  if (client2.connect(DEVICE_ID)) {
    topic.toCharArray(mqtt_topic, topic.length() + 1);
    PostData.toCharArray(mqtt_payload, PostData.length() + 1);
    Serial.println("mqtt_topic=");
    Serial.println(mqtt_topic);
    Serial.println("mqtt_payload=");
    Serial.println(mqtt_payload);
    Serial.println("Connect to MQTT server..");
    client2.publish(mqtt_topic, mqtt_payload);
    Serial.println("Disconnecting MQTT server..");
    client2.disconnect();
  }

  Serial.println("Waiting for next loop...");
  delay(600000); //just here to slow down the output. You can remove this
  //delay(30000);


}

