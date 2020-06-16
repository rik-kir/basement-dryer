void ntp_start() {
  Serial.println(F("Auf NTP sync warten..."));
  setSyncInterval(86400);
  setSyncProvider(getNtpTime);

  return;
}


String time_stamp() {
  String temp = "";
  if (hour() < 10) {
    temp += "0";
  }
  temp += hour();
  temp += ":";
  if (minute() < 10) {
    temp += 0;
  }
  temp += minute();
  temp += ":";
  if (second() < 10) {
    temp += 0;
  }
  temp += second();
  return temp;
}

String date_stamp() {
  String temp = "";
  if (day() < 10) {
    temp += 0;
  }
  temp += day();
  temp += ".";
  if (month() < 10) {
    temp += 0;
  }
  temp += month();
  temp += ".";
  temp += year();

  return temp;
}

String timestamper() {
  String temp = date_stamp();
  temp += ";";
  temp += time_stamp();
  temp += ";";

  return temp;
}

String timestamper_console() {
  String temp = date_stamp();
  temp += "-";
  temp += time_stamp();
  temp += "\t";

  return temp;
}

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println(F("NTP Transmit Request"));
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < serverdelay) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println(F("NTP Receive Response"));
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println(F("NTP No Response :-("));
  sd_write("NTP No Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void update_start_timestamp() {

  start[1] = year();
  start[2] = month();
  start[3] = day();
  if (start[0] == 0) {
    start[4] = (hour() * 100) + minute();
  }
  if (start[4] > 2350) {
    start[4] = 2350;
    Serial.println(F("Startzeit auf 2350 gesetzt um Schwierigkeiten zu vermeiden"));
  }

  return;
}
