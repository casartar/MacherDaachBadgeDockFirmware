/*
 * Badge tries to connect on last configured WiFi. 
 * If not succesfull it will open an access point.
 * Acces 192.168.4.1 to configure WiFi.
*/

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "src/WiFiManager/WiFiManager.h"         //https://github.com/tzapu/WiFiManager


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
}

void loop() {
    // put your main code here, to run repeatedly:
    
}
