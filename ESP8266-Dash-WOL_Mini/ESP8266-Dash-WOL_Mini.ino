#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <WiFiUdp.h>              //UDP library for WOL

#define WIFI_SSID "My_Wi-Fi"
#define WIFI_PASSWORD "my-awesome-password"

#define MQTT_SECURE true
#define MQTT_SERVER_FINGERPRINT {0x7e, 0x36, 0x22, 0x01, 0xf9, 0x7e, 0x99, 0x2f, 0xc5, 0xdb, 0x3d, 0xbe, 0xac, 0x48, 0x67, 0x5b, 0x5d, 0x47, 0x94, 0xd2}
#define mqtt_server "mother"
#define mqtt_port "1883"
#define mqtt_user "default"
#define mqtt_pass "default"
#define mqtt_topic "cmnd/plug-console/cmnd"
#define mqtt_payload "ON"
#define mac_address "deadbeefdead"

WiFiUDP udp;                //UDP for Wake-on-LAN
AsyncMqttClient mqttClient;
WiFiEventHandler wifiConnectHandler;
Ticker sleepTicker;

/*
  Very simple converter from a String representation of a MAC address to
  6 bytes. Does not handle errors or delimiters, but requires very little
  code space and no libraries.
*/

byte valFromChar(char c) {
  if (c >= 'a' && c <= 'f') return ((byte) (c - 'a') + 10) & 0x0F;
  if (c >= 'A' && c <= 'F') return ((byte) (c - 'A') + 10) & 0x0F;
  if (c >= '0' && c <= '9') return ((byte) (c - '0')) & 0x0F;
  Serial.println(c);
  return 0;
}

//Convert the given String to Bytes
void macStringToBytes(const String mac, byte *bytes) {
  if (mac.length() >= 12) {
    for (int i = 0; i < 6; i++) {
      bytes[i] = (valFromChar(mac.charAt(i * 2)) << 4) | valFromChar(mac.charAt(i * 2 + 1));
    }
  }
}

/*
  Send a Wake-On-LAN packet for the given MAC address, to the given IP
  address. Often the IP address will be the local broadcast.
*/
void sendWOL(const IPAddress ip, const byte mac[]) {
  digitalWrite(LED_BUILTIN, HIGH);
  byte preamble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  udp.beginPacket(ip, 9);
  udp.write(preamble, 6);
  for (uint8 i = 0; i < 16; i++) {
    udp.write(mac, 6);
  }
  udp.endPacket();
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}

//Set up variables and call functions to wake the PC
void wakePC() {
  byte target_mac[6];
  macStringToBytes(mac_address, target_mac);

  IPAddress target_ip;
  target_ip = WiFi.localIP();
  target_ip[3] = 255;

  sendWOL(target_ip, target_mac);
}

void connectToWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  connectToMqtt();
}

void connectToMqtt() {
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  mqttClient.publish(mqtt_topic, 2, false, mqtt_payload);
  wakePC();
}

void sleep() {
  ESP.deepSleep(0);
}

void setup() {
  Serial.begin(115200);
  sleepTicker.attach(5, sleep);

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.setServer(mqtt_server, atoi(mqtt_port));
  mqttClient.setCredentials(mqtt_user, mqtt_pass);

  connectToWifi();
}

void loop() {
}
