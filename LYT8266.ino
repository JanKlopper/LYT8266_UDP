/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.double clicked upload
 */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * *`
 Code by AUTHOMETION S.r.l.
 Version: 1.00
 Date: 24.10.2015
 
 Changes by Jan Klopper
 Version: 1.01
 Date: 15.11.2016
 
 Add udp discovery packet, removed self hosted AP mode.
 Add automatic lights on mode, as this is what is most usefull for regular 
 light bulbs.
 Remove OTA / mdns / webserver, as these are not needed in my case.
 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************
 *                  INPORTANT NOTICE                     *
 *********************************************************                  
 * This sketch require stagging stable 2.0.0             *
 * for a correct compiling and an ESP module with 1 MB   *
 * or more memory flash. Set SPIFFS to 64 kB.            *
 * Use Packet Sender program to send UDP packet to bulb  *
 * on 8899 port                                          *
 *********************************************************/

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <LYT8266Led.h>

// Configure these to match your network
const char* cSSID="";
const char* cPhrase="";

// how bright should the white lights turn on when power is applied?
static unsigned char ui8StartWhiteValue = 127; // 0 - 255

//OTA_Setup(8080)
uint16_t ui16LocalPort=8899;      // local port to listen for UDP packets
uint16_t ui16DiscoveryPort=8898;      // local port to listen for UDP packets
uint16_t ui16BytesReceived;
uint8_t ui8Counter, ui8RedValue, ui8GreenValue, ui8BlueValue, ui8WhiteValue;
char cPacketBuffer[512];         //buffer to hold incoming and outgoing packets
String sReceivedCommand;
uint32_t ui32Time;
LYT8266Led myLYT8266Led;
WiFiUDP recvUDP;
WiFiUDP sendUDP;
IPAddress broadcastIP;

static const unsigned long DISCOVERY_INTERVAL = 1000; // ms
static unsigned long lastDiscoveryTime = 0;

void sendDiscovery(bool direct){
  // sends a discovery packet to the network
  
  if(direct || millis() - lastDiscoveryTime >= DISCOVERY_INTERVAL){
    lastDiscoveryTime += DISCOVERY_INTERVAL;
    sendUDP.beginPacketMulticast(broadcastIP, ui16DiscoveryPort, WiFi.localIP());
    sendUDP.write("lyt8266 ip: ");
    sendUDP.write(WiFi.localIP());
    sendUDP.write(" port: ");
    sendUDP.write(ui16LocalPort);
    sendUDP.endPacket();
  }  
}
  
void setup(){
  // Start the leds, set color leds to off, and white to preset value
  myLYT8266Led.vfESP8266HWInit();
  myLYT8266Led.vfSetRGBValues(0, 0, 0);
  myLYT8266Led.vfSetWhiteValue(ui8StartWhiteValue);
  myLYT8266Led.vfLYT8266LedTask();

  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin(cSSID, cPhrase);
  Serial.begin(115200);
  Serial.print("WiFi Connection ");
  
  ui32Time = millis();
  while((WiFi.status() != WL_CONNECTED) && (millis() - ui32Time < 10000)) {
    Serial.print(".");
    delay(50);
  }

  if(WiFi.status() != WL_CONNECTED){
    WiFi.softAP("LYT8266");
    delay(500);
  }
  //OTA_Init("LYT8266-OTA");

  recvUDP.begin(ui16LocalPort);
  //delay(1000);
  //Serial.println("Setup done");

  Serial.print("\nLocal IP address: ");
  Serial.println(WiFi.localIP());
  // Get the broadcast IP for the current subnet and IP
  broadcastIP = ~WiFi.subnetMask() | WiFi.gatewayIP();
  sendDiscovery(true);
}

void loop(){
  sendDiscovery(false);

  myLYT8266Led.vfLYT8266LedTask();
  ui16BytesReceived = recvUDP.parsePacket();
  if (ui16BytesReceived > 0){
    sReceivedCommand = "";
    memset(cPacketBuffer, '\0', 512);
    recvUDP.read(cPacketBuffer, ui16BytesReceived);
    sReceivedCommand = String(cPacketBuffer);

    // +1,RED,GREEN,BLUE<CR><LF> set PWM RGB leds
    if ((sReceivedCommand.indexOf("+1,") != -1) && 
        (sReceivedCommand.indexOf("\r\n") != -1)){
      sReceivedCommand = sReceivedCommand.substring(sReceivedCommand.indexOf(",") + 1);
      ui8RedValue = sReceivedCommand.toInt();
      sReceivedCommand = sReceivedCommand.substring(sReceivedCommand.indexOf(",") + 1);
      ui8GreenValue = sReceivedCommand.toInt();
      sReceivedCommand = sReceivedCommand.substring(sReceivedCommand.indexOf(",") + 1);
      ui8BlueValue = sReceivedCommand.toInt();
      myLYT8266Led.vfSetWhiteValue(0);
      myLYT8266Led.vfSetRGBValues(ui8RedValue, ui8GreenValue, ui8BlueValue);
    
    // +4,WHITE<CR><LF> set PWM white leds    
    } else if ((sReceivedCommand.indexOf("+4,") != -1) && 
         (sReceivedCommand.indexOf("\r\n") != -1)){
        sReceivedCommand = sReceivedCommand.substring(sReceivedCommand.indexOf(",") + 1);
        ui8WhiteValue = sReceivedCommand.toInt();
        myLYT8266Led.vfSetRGBValues(0, 0, 0);
        myLYT8266Led.vfSetWhiteValue(ui8WhiteValue);
    }
  }
}
