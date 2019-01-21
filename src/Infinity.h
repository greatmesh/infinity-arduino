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

#ifndef Infinity_h
#define Infinity_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

typedef enum CommunicationMethod 
{
    AUTO = 0, WIFI = 1, WIFI_NRF = 2
} CommunicationMethod_t;

typedef enum IdentifyMethod 
{
    OPEN = 0, KEY = 1, MANUAL = 2
} IdentifyMethod_t;

typedef enum SendDestination 
{
    CLOUD = 0, PHONE = 1
} SendDestination_t;

typedef enum Debug 
{
    NONE = 0, DEBUG = 1
} Debug_t;

class InfinityClass {
    private:
        bool empty(const char* data);
        void EEP_Write(int part, String val);
        String EEP_Read(int part);
        void checkConnetion(void);
        void setLocalIP(void);
        void UDPReceiver(void);
        void UDPsender(String data, String remoteIP, uint16_t remotePort);
        void UDPack(int id, int status);
        void serializer(String data, String packetRemoteIP, uint16_t packetRemotePort);
    public:
        void begin(String hostname, Debug_t d = NONE);
        void erase();
        void id(String id);
        void mode(CommunicationMethod_t m);
        void identify(IdentifyMethod_t m, bool status = false);
        void cloud(String url, int port = 3085);
        void auth(String key);
        void send(String id, String msg);
        void send(SendDestination_t d, String msg);
        void receive();
        void property();
        void update();
};

extern InfinityClass Infinity;
#endif