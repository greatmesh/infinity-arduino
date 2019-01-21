/*
 MIT License

 Copyright (c) 2019 GreatMesh

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/

#include "Arduino.h"
#include "Infinity.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>

IPAddress localIP;
IPAddress cloudIP;
int localPort;
int cloudPort;
bool debug, connected, waitConnection = false;

//int reg[] = {1};

void InfinityClass::begin(String hostname, Debug_t d) {
    EEPROM.begin(4096);
    if(d) debug = true;
    else debug = false;
    if(!debug) WiFi.hostname(hostname);
    else Serial.printf("\nSet hostname ... %s\n", WiFi.hostname(hostname) ? "[ OK ]" : "[ FAILED ]");
    if(!debug) WiFi.mode(WIFI_STA);
    else Serial.printf("Wi-Fi mode set to WIFI_STA ... %s\n", WiFi.mode(WIFI_STA) ? "[ OK ]" : "[ FAILED ]");
    WiFi.setAutoConnect(true);
    mode(WIFI); 
    identify(OPEN);
    if(empty(WiFi.SSID().c_str()) && empty(WiFi.SSID().c_str())) {
        if(!debug) WiFi.beginSmartConfig();
        else Serial.printf("Begin SmartConfig ... %s\n", WiFi.beginSmartConfig() ? "[ OK ]" : "[ FAILED ]");
    }
}

void InfinityClass::erase() {
    ESP.eraseConfig();
    // More ...
}

void InfinityClass::id(String id) {

}

void InfinityClass::mode(CommunicationMethod_t m) {
    switch(m){
        case 0:
            // Auto mode
            break;
        case 1:
            //test
            break;
        case 2:
            //test
            break;
   }
}

void InfinityClass::identify(IdentifyMethod_t m, bool status) {
    switch(m){
        case 0:
            break;
        case 1:
            //test
            break;
        case 2:
            //test
            break;
   }
}

void InfinityClass::cloud(String url, int port) {
   
}

void InfinityClass::auth(String key) {
   
}

void InfinityClass::send(String id, String msg) {
   
}

void InfinityClass::send(SendDestination_t d, String msg) {
   
}

void InfinityClass::receive() {
   
}

void InfinityClass::property() {
   
}

void InfinityClass::update() {
    checkConnetion();
    setLocalIP();
}

bool InfinityClass::empty(const char* data) {
    if(strlen(data) == 0) return true;
    else return false;
}

void InfinityClass::EEP_Write(int part, String val) {
    int n = part * 32;
    for(int i = 0; i < n; i++) {
        if(sizeof(val) == i) break;
        EEPROM.write(0x0F + i, val[i]);
    }
    EEPROM.commit();
    EEPROM.end();
}

String InfinityClass::EEP_Read(int part) {
    String data;
    int n = part * 32;
    for(int i = 0; i < n; i++) {
        if (char(EEPROM.read(0x0F + i)) == 0) break;
        data = data + char(EEPROM.read(0x0F + i)); 
    }  
    return data; 
}

void InfinityClass::checkConnetion(void) {
    if(WiFi.isConnected() && !connected) {
        if(debug) {
            Serial.println("[ OK ]");
            Serial.printf("Connected to %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
        }
        connected = true;
        waitConnection = false;
    }
    else if(WiFi.status() != WL_CONNECTED) {
        connected = false;
        if (debug && !waitConnection) {
            waitConnection = true;
            Serial.printf("Waiting for connect to %s ... ", empty(WiFi.SSID().c_str()) ? "AP" : WiFi.SSID().c_str());
        }
    } 
}

void InfinityClass::setLocalIP(void) {
    if (WiFi.localIP() != IPAddress()) localIP = WiFi.localIP();
}

InfinityClass Infinity;
