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
#include <WiFiUdp.h>
#include <ArduinoJson.h>

WiFiUDP Udp;
DynamicJsonBuffer jsonBuffer;

void InfinityClass::begin(String hostname, Debug_t d)
{
    EEPROM.begin(4096);
    _debug = d;
    if (_debug)
    {
        Serial.printf("\nSet hostname ... %s\n", WiFi.hostname(hostname) ? "[ OK ]" : "[ FAILED ]");
        Serial.printf("Wi-Fi mode set to WIFI_STA ... %s\n", WiFi.mode(WIFI_STA) ? "[ OK ]" : "[ FAILED ]");
    }
    else
    {
        WiFi.hostname(hostname);
        WiFi.mode(WIFI_STA);
    }
    WiFi.setAutoConnect(true);
    if (_identifyMethod != 0 && !empty((char *)WiFi.SSID().c_str()) && !EEPROM.read(0x0F + _0x0F[0]))
        WiFi.disconnect(true);
    if (empty((char *)WiFi.SSID().c_str()) && !EEPROM.read(0x0F + _0x0F[0]))
    {
        if (_debug)
            Serial.printf("Begin SmartConfig ... %s\n", WiFi.beginSmartConfig() ? "[ OK ]" : "[ FAILED ]");
        else
            WiFi.beginSmartConfig();
    }
}

void InfinityClass::erase()
{
    if (_debug)
        Serial.print("Erase configuration ... ");
    if (ESP.eraseConfig())
    {
        EEPROM.write(0x0F + _0x0F[0], 0);
        EEPROM.commit();
        // More ...
        if (_debug)
        {
            Serial.println("[ OK ]");
            Serial.println("*** Please restart your board now! ***");
            _stop = true;
        }
    }
    else
    {
        if (_debug)
            Serial.println("[ FAILED ]");
    }
}

void InfinityClass::id(String id)
{
}

void InfinityClass::mode(CommunicationMethod_t m)
{
    switch (m)
    {
    case 0:
        // More ...
        break;
    case 1:
        // More ...
        break;
    case 2:
        // More ...
        break;
    }
}

void InfinityClass::identify(IdentifyMethod_t m, bool status)
{
    if (m == 0 && !EEPROM.read(0x0F + _0x0F[0]))
        _identifyMethod = m;
    else
    {
        if (!status && !EEPROM.read(0x0F + _0x0F[0]))
            _identifyMethod = m;
        else if (status && !empty((char *)_tempSSID.c_str()))
        {
            if (_debug)
                Serial.printf("%sManually identifying ... ", (_connected ? "" : "\n"));
            if (!EEPROM.read(0x0F + _0x0F[0]))
            {
                WiFi.stopSmartConfig();
                WiFi.begin((char *)_tempSSID.c_str(), (char *)_tempPsk.c_str());
                EEPROM.write(0x0F + _0x0F[0], 1);
                EEPROM.commit();
                _tempSSID = _tempPsk = "";
                if (_debug)
                    Serial.println("[ OK ]");
            }
            else
            {
                if (_debug)
                    Serial.println("[ FAILED ]");
            }
            if (_debug && !_connected && !EEPROM.read(0x0F + _0x0F[0]))
                Serial.print("Waiting for connect to AP ... ");
            else if (_debug && !_connected && EEPROM.read(0x0F + _0x0F[0]))
                Serial.printf("Waiting for connect to %s ... ", empty(WiFi.SSID().c_str()) ? "AP" : WiFi.SSID().c_str());
        }
    }
}

void InfinityClass::identify(IdentifyMethod_t m, const char *key)
{
    if (m == 1 && !EEPROM.read(0x0F + _0x0F[0]))
    {
        _identifyMethod = m;
        _identifyKey = key;
    }
}

void InfinityClass::cloud(String url, int port)
{
}

void InfinityClass::auth(String key)
{
}

void InfinityClass::send(String id, String msg)
{
}

void InfinityClass::send(SendDestination_t d, String msg)
{
}

void InfinityClass::receive()
{
}

void InfinityClass::property()
{
}

void InfinityClass::update()
{
    if (!_stop)
    {
        checkConnetion();
        if (_identifyMethod != 0 && !EEPROM.read(0x0F + _0x0F[0]))
            checkIdentification();
        UDPReceiver();
    }
}

void InfinityClass::checkConnetion(void)
{
    if (WiFi.isConnected() && !_connected)
    {
        if (_debug)
        {
            Serial.println("[ OK ]");
            Serial.printf("Connected to %s", WiFi.SSID().c_str());
            if (Udp.begin(_localPort))
                Serial.printf(", Now listening at: %s:%d [ UDP ]\n", WiFi.localIP().toString().c_str(), _localPort);
            else
                Serial.println(", Running service ... [ FAILED ]");
        }
        else
            Udp.begin(_localPort);
        _connected = true;
        _waitConnection = _identificationCheck = _waitIdentification = false;
        _process[0] = millis();
    }
    else if (WiFi.status() != WL_CONNECTED)
    {
        _connected = false;
        if (_debug && !_waitConnection)
        {
            Serial.printf("Waiting for connect to %s ... ", empty(WiFi.SSID().c_str()) ? "AP" : WiFi.SSID().c_str());
            _waitConnection = true;
        }
    }
}

void InfinityClass::checkIdentification(void)
{
    if (_connected && !_identificationCheck)
    {
        if (_debug && !_waitIdentification && _identifyMethod == 1)
        {
            Serial.print("Waiting for identification ... ");
            _waitIdentification = true;
        }
        if (millis() - _process[0] > _timeout || _tryIdentification > 3)
        {
            WiFi.disconnect(true);
            WiFi.stopSmartConfig();
            delay(100);
            if (_debug)
            {
                if (_identifyMethod == 1)
                    Serial.println("[ FAILED ]");
                Serial.printf("Begin SmartConfig ... %s\n", WiFi.beginSmartConfig() ? "[ OK ]" : "[ FAILED ]");
            }
            else
                WiFi.beginSmartConfig();
            _identificationCheck = true;
            _tryIdentification = 0;
        }
    }
}

void InfinityClass::serializer(String data, String packetRemoteIP, uint16_t packetRemotePort)
{
    JsonObject &json = jsonBuffer.parseObject(data);
    if (json.success())
    {
        _remoteIP.fromString(packetRemoteIP);
        _remotePort = packetRemotePort;
        int id = json["id"];
        int req = json["req"];
        switch (req)
        {
        case 1:
        {
            JsonObject &data = jsonBuffer.createObject();
            data["hostname"] = WiFi.hostname();
            data["identify"] = _identifyMethod;
            UDPack(id, req, 1, data);
            break;
        }
        case 2:
            if (!EEPROM.read(0x0F + _0x0F[0]))
            {
                if (_identifyMethod == 1)
                {
                    String key = json["key"];
                    if (key == _identifyKey)
                    {
                        EEPROM.write(0x0F + _0x0F[0], 1);
                        EEPROM.commit();
                        JsonObject &data = jsonBuffer.createObject();
                        UDPack(id, req, 1, data);
                        if (_debug)
                            Serial.println("[ OK ]");
                    }
                    else
                    {
                        JsonObject &data = jsonBuffer.createObject();
                        data["error"] = "Wrong key";
                        UDPack(id, req, 0, data);
                        _tryIdentification++;
                    }
                }
                else if (_identifyMethod == 2 && !_waitIdentification)
                {
                    if (_debug)
                        Serial.print("Begin manually identifying ... ");
                    if (!empty((char *)WiFi.SSID().c_str()))
                    {
                        _tempSSID = WiFi.SSID();
                        _tempPsk = WiFi.psk();
                        JsonObject &data = jsonBuffer.createObject();
                        UDPack(id, req, 1, data);
                        _waitIdentification = true;
                        if (_debug)
                            Serial.println("[ OK ]");
                    }
                    else
                    {
                        if (_debug)
                            Serial.println("[ FAILED ]");
                    }
                }
            }
            break;
        }
    }
}

void InfinityClass::UDPReceiver(void)
{
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        int len = Udp.read(incomingPacket, 255);
        if (len > 0)
        {
            incomingPacket[len] = 0;
        }
        serializer(incomingPacket, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    }
}

void InfinityClass::UDPsender(String data, String remoteIP, uint16_t remotePort)
{
    Udp.beginPacket((char *)remoteIP.c_str(), remotePort);
    Udp.write((char *)data.c_str());
    Udp.endPacket();
}

void InfinityClass::UDPack(int id, int req, int status, JsonObject &data)
{
    JsonObject &json = jsonBuffer.createObject();
    json["id"] = id;
    json["req"] = req;
    json["status"] = status;
    jsonMerge(json, data);
    String output;
    json.printTo(output);
    UDPsender(output, _remoteIP.toString().c_str(), _remotePort);
}

bool InfinityClass::empty(const char *data)
{
    if (strlen(data) == 0)
        return true;
    else
        return false;
}

void InfinityClass::jsonMerge(JsonObject &dest, JsonObject &src)
{
    for (auto kvp : src)
    {
        dest[kvp.key] = kvp.value;
    }
}

void InfinityClass::EEP_Write(int part, String val)
{
    int n = part * 32;
    for (int i = 0; i < n; i++)
    {
        if (sizeof(val) == i)
            break;
        EEPROM.write(0x0F + i, val[i]);
    }
    EEPROM.commit();
}

String InfinityClass::EEP_Read(int part)
{
    String data;
    int n = part * 32;
    for (int i = 0; i < n; i++)
    {
        if (char(EEPROM.read(0x0F + i)) == 0)
            break;
        data = data + char(EEPROM.read(0x0F + i));
    }
    return data;
}

InfinityClass Infinity;
