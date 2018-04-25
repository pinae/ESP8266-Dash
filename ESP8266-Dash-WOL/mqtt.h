bool taskComplete = false;      //this variable makes sure, that the WOL and MQTT tasks have been completed.

//Function that processes received MQTT messages.
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {

  //Ensure correct lenght of the payload
  char payload_content[len + 1];
  //Copy payload pointer value to char[]
  strlcpy(payload_content, payload, len + 1);
  
  Serial.print("MQTT message received: '");
  Serial.print(payload_content);
  Serial.println("'");

  //Determine what to do depending on the payload
  if (strcmp(payload_content, "reset") == 0) {
    Serial.println("Received Reset command");
    wifiManager.resetSettings();
    ESP.restart();
  } else if (strcmp(payload_content, "delay") == 0) {
    Serial.println("Received Delay command. Flipping sleep switch.");
    enableSleep = !enableSleep;
  }
  
}

//This function is called after a successful MQTT-connection has been established
void onMqttConnect(bool sessionPresent) {
  Serial.println("MQTT connected");

  //Build topic string...
  char cmnd_topic[255] = "cmnd/";
  strcat(cmnd_topic, device_name);
  strcat(cmnd_topic, "/cmnd");

  //... and print it.
  Serial.println("Send commands to the following topic:");
  Serial.println(cmnd_topic);

  //Subscribe to the topic
  mqttClient.subscribe(cmnd_topic, 2);

  //Publish the configured payload on the configured topic
  mqttClient.publish(mqtt_topic, 2, false, mqtt_payload);

  //Call the WOL process
  wakePC();

  //Consider the work done and allow Deep Sleep
  taskComplete = true;
}
