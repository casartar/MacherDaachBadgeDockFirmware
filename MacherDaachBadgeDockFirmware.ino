/**
*/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "src/WiFiManager/WiFiManager.h"                  // https://github.com/tzapu/WiFiManager
#include "src/Timezone/src/Timezone.h"                    // https://github.com/JChristensen/Timezone

unsigned int localPort = 2390;                            // local port to listen for UDP packets

IPAddress timeServerIP;
const char* ntpServerName = "de.pool.ntp.org";

const int NTP_PACKET_SIZE = 48;                           // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE];                      //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

//Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};    //Central European Summer Time
TimeChangeRule CET = {"CET", Last, Sun, Oct, 3, 60};       //Central European Standard Time
Timezone CE(CEST, CET);


void setup()
{
  Serial.begin(9600);
  //delay(500);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();
  
  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point named ESP + ChipID
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect();

  //Serial.println("Starting UDP");
  udp.begin(localPort);
  //Serial.print("Local port: ");
  //Serial.println(udp.localPort());
  Serial.swap();
}

void loop()
{
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);

  int cb = udp.parsePacket();
  if (!cb) {
   // Serial.println("no packet yet");
  }
  else {
   // Serial.print("packet received, length=");
   // Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //   Serial.print("Seconds since Jan 1 1900 = " );
    // Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    //Serial.print("Unix time = ");
    //Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    //subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    //print Unix time:
    //Serial.println(epoch);

    TimeChangeRule *tcr;
    time_t utc;
    utc = epoch;

    //printTime(utc, "UTC", "Universal Coordinated Time");
    printTime(CE.toLocal(utc, &tcr), tcr -> abbrev, "Landau");
    delay(5000);
  }
  // wait ten seconds before asking for the time again
  
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
//  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

//Function to print time with time zone
void printTime(time_t t, char *tz, char *loc) {
  sPrintI00(hour(t));
  sPrintDigits(minute(t));
  //  sPrintDigits(second(t));
  //  Serial.print(' ');
  //  Serial.print(dayShortStr(weekday(t)));
  //  Serial.print(' ');
  //  sPrintI00(day(t));
  //  Serial.print(' ');
  //  Serial.print(monthShortStr(month(t)));
  //  Serial.print(' ');
  //  Serial.print(year(t));
  //  Serial.print(' ');
  //  Serial.print(tz);
  //  Serial.print(' ');
  //  Serial.print(loc);
  Serial.print("\r");
}

//Print an integer in "00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintI00(int val)
{
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
  return;
}

//Print an integer in ":00" format (with leading zero).
//Input value assumed to be between 0 and 99.
void sPrintDigits(int val)
{
  Serial.print(':');
  if (val < 10) Serial.print('0');
  Serial.print(val, DEC);
}
