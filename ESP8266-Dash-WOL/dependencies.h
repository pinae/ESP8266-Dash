#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <AsyncMqttClient.h>      //MQTT Client library
#include <WiFiUdp.h>              //UDP library for WOL
#include <ArduinoOTA.h>           //OTA function
#include <ArduinoJson.h>          //JSON library for config storage
#include <Ticker.h>               //Ticker for automatic suspend

// Basic objects
WiFiManager wifiManager;    //WifiManager for Wifi and settings
WiFiUDP udp;                //UDP for Wake-on-LAN
AsyncMqttClient mqttClient; //MQTT client
Ticker sleepTicker;         //sleepTicker to trigger deep sleep

//Control if the esp should go to sleep
bool enableSleep = true;

//include other parts if the code
#include "spiffs.h"
#include "wol.h"
#include "mqtt.h"

//Send the ESP to Deep Sleep if it's enabled
void shutdown() {
  if (enableSleep && taskComplete) {
    Serial.println("Going to sleep.");
    ESP.deepSleep(0);
  } else if (!enableSleep) {
    Serial.println("Sleep disabled");
  }
}
