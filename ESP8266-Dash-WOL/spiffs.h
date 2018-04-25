/* Custom struct for the eeprom data */
char mqtt_server[40];
char mqtt_port[6] = "1883";
char mqtt_user[34] = "default";
char mqtt_pass[34] = "default";
char mqtt_topic[120];
char mqtt_payload[40];
char device_name[40] = "woldash";
char mac_address[13];

//Stores if the configuration has been changed
bool shouldSaveConfig = false;

//Callback if the configuration has been changed
void saveConfigCallback () {
  shouldSaveConfig = true;
}

//Load the configuration data from the internal SPIFFS
void loadConfig() {

  //Try to open the SPIFFS 
  if (SPIFFS.begin()) {
    Serial.println("Successfully mounted SPIFFS.");
    //Check if config file is present
    if (SPIFFS.exists("/config.json")) {
      //If it is, try to read it
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
    
        configFile.readBytes(buf.get(), size);
        //Create JSON Buffer & Object to store the data
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());

        //If the JSON has been parsed correctly
        if (json.success()) {
          Serial.println("Successfully parsed the JSON data.");

          //Copy data from JSON object to variables
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(mqtt_user, json["mqtt_user"]);
          strcpy(mqtt_pass, json["mqtt_pass"]);
          strcpy(mqtt_topic, json["mqtt_topic"]);
          strcpy(mqtt_payload, json["mqtt_payload"]);
          strcpy(device_name, json["device_name"]);
          strcpy(mac_address, json["mac_address"]);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  //If the mounting of the SPIFFS failed format it, reset WifiManager and reboot.
  } else {
    Serial.println("failed to mount FS");
    wifiManager.resetSettings();
    SPIFFS.format();
    ESP.restart();
  }
}

//Save config to SPIFFS
void saveConfig() {
  Serial.println("Saving the config data to SPIFFS");

  //Create JSON Buffer & Object to store the data
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  //Copy data from variables to JSON object
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_user"] = mqtt_user;
  json["mqtt_pass"] = mqtt_pass;
  json["mqtt_topic"] = mqtt_topic;
  json["mqtt_payload"] = mqtt_payload;
  json["device_name"] = device_name;
  json["mac_address"] = mac_address;

  //Open config file for writing
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  //Print data to the config file
  json.printTo(configFile);
  //close it
  configFile.close();
  
}
