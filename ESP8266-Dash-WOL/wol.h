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
  } else {
    Serial.println("Incorrect MAC format.");
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
