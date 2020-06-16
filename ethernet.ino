void ethernet_start() {
  if (Ethernet.begin(mac) == 0) {
    while (1) {
      Serial.println(F("Ethernet Konfiguration via DHCP fehlgeschlagen!"));
      lcd.setCursor(0, 0);
      lcd.print(F("KEIN DHCP"));
      lcd.setCursor(0, 1);
      lcd.print("ANGEHALTEN!");
      while (true);
    }
  }
  Serial.print(F("Durch DHCP zugewiesene IP: "));
  Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
}
