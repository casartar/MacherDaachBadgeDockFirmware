/*
 * Badge tries to connect on last configured WiFi. 
 * If not succesfull it will open an access point.
 * Acces 192.168.4.1 to configure WiFi.
*/

#include <ESP8266WiFi.h>                        //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "src/WiFiManager/WiFiManager.h"        //https://github.com/tzapu/WiFiManager
#include "src/pubsubclient/src/PubSubClient.h"  //https://github.com/knolleary/pubsubclient

const char* mqtt_server = "nerdparty.holzmolz.de";

WiFiClient espClient;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient client(mqtt_server, 8002, callback, espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point named ESP + ChipID
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect();

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    if (client.connect("arduinoClient", "holymoly", "thisisjustatest")) {
      client.publish("outTopic","hello world");
      client.subscribe("inTopic");
    }
}

void loop() {

  client.loop();

}
