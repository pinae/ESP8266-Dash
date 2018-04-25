//include dependencies, create objects and variables
#include "dependencies.h"

//show WifiManager debug output
bool debugOutput = true;

void setup() {

  Serial.begin(115200);

  //load config from SPIFFS
  loadConfig();

  //Set WiFi hostname to the configured device name
  WiFi.hostname(device_name);

  //Enable WifiManager Debug Output
  wifiManager.setDebugOutput(debugOutput);
  
  //Set custom WifiManager paramters
  WiFiManagerParameter custom_device_name("name", "Device Name", device_name, 40);
  WiFiManagerParameter custom_mqtt_server("server", "MQTT Server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", " Port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("username", "MQTT Username", mqtt_user, 34);
  WiFiManagerParameter custom_mqtt_pass("passwort", "MQTT Password", mqtt_pass, 34);
  WiFiManagerParameter custom_mqtt_topic("topic", "MQTT Topic", mqtt_topic, 120);
  WiFiManagerParameter custom_mqtt_payload("payload", "MQTT Payload", mqtt_payload, 40);
  WiFiManagerParameter custom_mac_address("mac", "PCs MAC-Address", mac_address, 18);

  //Set callback to save the WifiManager config to SPIFFS
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //Add the Parameters to the webinterface
  wifiManager.addParameter(&custom_device_name);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_mqtt_topic);
  wifiManager.addParameter(&custom_mqtt_payload);
  wifiManager.addParameter(&custom_mac_address);

  //Connect to WiFi and
  if (! wifiManager.autoConnect()) {
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }
  
  //Attach the shutdown function every 5 seconds, so the battery isn't depleted when the ESP hangs for some reason.
  sleepTicker.attach(5, shutdown);

  //Copy WifiManager values to the corresponding variables
  strcpy(device_name, custom_device_name.getValue());
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_pass, custom_mqtt_pass.getValue());
  strcpy(mqtt_topic, custom_mqtt_topic.getValue());
  strcpy(mqtt_payload, custom_mqtt_payload.getValue());
  strcpy(mac_address, custom_mac_address.getValue());

  //Save the config data to SPIFFS if neccessary
  if (shouldSaveConfig) {
    saveConfig();
  }

  //Configure the MQTT-Client
  mqttClient.setServer(mqtt_server, atoi(mqtt_port));
  mqttClient.setCredentials(mqtt_user, mqtt_pass);
  mqttClient.setClientId(device_name);
  //Add callback to call if the client is connected
  mqttClient.onConnect(onMqttConnect);
  //Add callback to call if the client receives a message
  mqttClient.onMessage(onMqttMessage);

  //Try to connect to MQTT server
  mqttClient.connect();

  //Enable Arduino OTA for the device. Copied verbatim from the example
  ArduinoOTA.setHostname(device_name);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else {
      type = "filesystem";
      SPIFFS.end();
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

}

void loop() {
  // Handle OTA requests
  ArduinoOTA.handle();
}
