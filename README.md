# ESP8266-Dash
Build your own Amazon-free Dash-Button with a ESP-01-Module (ESP8266)

Modify the `.ino`-file to match your WiFi SSID and password. Adjust the server URL and the SHA1-Hash to match your server's certificate. Flash the customized firmware with the ESP8266 module of the Arduino-IDE.

### Security considerations
Please note that the ESP-firmware only supports TLS 1.1 with RSA Key-Exchange. Setting your server up to accept this type of connection may make it vulnerable to downgrading attacks. If in doubt use a proxy like a Raspberry Pi to receive the weakly encrypted messages from your buttons and establish a highly secure connection to your server.
