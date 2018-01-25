#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;
char mqtt_server[40];
char device_name[40];
char mqtt_port[6] = "1883";
char onfig_enable_topic[100] = "cmnd/espdash/config_enable";

#define LED             2
void setup() {
	// put your setup code here, to run once:
	Serial1.begin(115200);
	Serial1.println();
	Serial1.println("Initializing outputs...");
	pinMode(LED, OUTPUT);
	digitalWrite(LED, LOW);
	WiFiManager wifiManager;
	wifiManager.resetSettings();

	WiFiManagerParameter custom_device_name("name", "device_name", device_name, 40);
	WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
	WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 5);
	WiFiManagerParameter custom_mac_address("mac", "mac_address", mac_address, 5);
	wifiManager.addParameter(&custom_mqtt_server);
	wifiManager.addParameter(&custom_mqtt_port);
	wifiManager.autoConnect();
	byte mac[] = { 0xFR, 0xBB, 0x34, 0xE5, 0x29, 0xFC };

	IPAddress computer_ip(192, 168, 178, 26); 

	WakeOnLan::sendWOL(computer_ip, UDP, mac, sizeof mac);
	/*wifiManager.startConfigPortal("OnDemandAP");*/
	mqttClient.setServer(mqtt_server, atoi(mqtt_port));
	mqttClient.onConnect(onMqttConnect);
	/*mqttClient.onDisconnect(onMqttDisconnect);*/
	/*mqttClient.onSubscribe(onMqttSubscribe);*/
	/*mqttClient.onMessage(onMqttMessage);*/
	mqttClient.connect();
}


void onMqttConnect(bool sessionPresent) {
  uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);

void loop() {
	// this should never be reached.
}


void blinkSuccess() {
	for (int i = 4; i < 50; i=(5*i) >> 2) {
		digitalWrite(LED, HIGH);   // turn the LED off
		delay(10*i);               // wait
		digitalWrite(LED, LOW);    // turn the LED on
		delay(10*i);               // wait
	}
}

void blinkError() {
	for (int i = 0; i < 28; i++) {
		digitalWrite(LED, HIGH);   // turn the LED off
		delay(125);                        // wait
		digitalWrite(LED, LOW);    // turn the LED on
		delay(125);                        // wait
	}
}

void blinkSent() {
	for (int i = 0; i < 2; i++) {
		digitalWrite(LED, LOW);   // turn the LED on
		delay(200);                        // wait
		digitalWrite(LED, HIGH);    // turn the LED off
		delay(200);                        // wait
	}
}

void shutdown() {
	Serial1.println("Shutting down.");
	Serial1.println("Going to sleep.");
	ESP.deepSleep(0);
	Serial1.println("Sleep failed.");
	while(1) {
		blinkError();
	}
}
