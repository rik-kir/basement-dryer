byte ftp_start() {
  fh = SD.open(filename, FILE_READ);
  while (fh.available() == 0);
  Serial.print(timestamper_console());
  Serial.println(F("Datei geöffnet"));

  if (doFTP()) {
    Serial.print(timestamper_console());
    Serial.println(F("FTP OK"));
    return 1;
  } else {
    Serial.print(timestamper_console());
    Serial.println(F("FTP FAIL"));
    return 0;
  }
}

byte doFTP() {
  if (client.connect(ftpserver, 21)) {
    Serial.print(timestamper_console());
    Serial.println(F("Verbindung zu FTP-Server hergestellt"));
  }
  else {
    Serial.print(timestamper_console());
    Serial.println(F("Die Verbindung zum FTP-Server konnte nicht hergestellt werden :( "));
    fh.close();
    return 0;
  }

  if (!eRcv()) return 0;

  client.print(F("USER "));
  client.println(ftp_username);

  if (!eRcv()) return 0;

  client.print(F("PASS "));
  client.println(ftp_passwort);

  if (!eRcv()) return 0;

  client.println(F("SYST"));

  if (!eRcv()) return 0;

  client.println(F("Type I"));

  if (!eRcv()) return 0;

  client.println(F("PASV"));

  if (!eRcv()) return 0;

  char *tStr = strtok(outBuf, "(,");
  int array_pasv[6];
  for ( int i = 0; i < 6; i++) {
    tStr = strtok(NULL, "(,");
    array_pasv[i] = atoi(tStr);
    if (tStr == NULL) {
      Serial.print(timestamper_console());
      Serial.println(F("Bad PASV Answer"));
    }
  }

  unsigned int hiPort, loPort;

  hiPort = array_pasv[4] << 8;
  loPort = array_pasv[5] & 255;
  Serial.print(timestamper_console());
  Serial.print(F("Data port: "));
  hiPort = hiPort | loPort;
  Serial.println(hiPort);

  if (dclient.connect(ftpserver, hiPort)) {
    Serial.print(timestamper_console());
    Serial.println(F("Data connected"));
  }
  else {
    Serial.print(timestamper_console());
    Serial.println(F("Data connection failed"));
    client.stop();
    fh.close();
    return 0;
  }

  client.print(F("STOR "));
  client.println(filename);

  if (!eRcv()) {
    dclient.stop();
    return 0;
  }
  Serial.print(timestamper_console());
  Serial.println(F("Writing"));

  byte clientBuf[64];
  int clientCount = 0;

  while (fh.available()) {
    clientBuf[clientCount] = fh.read();
    clientCount++;

    if (clientCount > 63) {
      dclient.write(clientBuf, 64);
      clientCount = 0;
    }
  }

  if (clientCount > 0) dclient.write(clientBuf, clientCount);

  dclient.stop();
  Serial.print(timestamper_console());
  Serial.println(F("Data disconnected"));

  if (!eRcv()) return 0;

  client.println(F("QUIT"));

  if (!eRcv()) return 0;

  client.stop();
  Serial.print(timestamper_console());
  Serial.println(F("Command disconnected"));

  fh.close();
  Serial.print(timestamper_console());
  Serial.println(F("Datei geschlossen"));
  return 1;
}

byte eRcv() {
  byte respCode;
  byte thisByte;

  while (!client.available()) delay(1);

  respCode = client.peek();

  outCount = 0;

  while (client.available()) {
    thisByte = client.read();
    Serial.write(thisByte);

    if (outCount < 127) {
      outBuf[outCount] = thisByte;
      outCount++;
      outBuf[outCount] = 0;
    }
  }

  if (respCode >= '4') {
    efail();
    return 0;
  }

  return 1;
}


void efail() {
  byte thisByte = 0;

  client.println(F("QUIT"));
  while (!client.available()) delay(1);
  while (client.available()) {
    thisByte = client.read();
    Serial.write(thisByte);
  }

  client.stop();
  Serial.print(timestamper_console());
  Serial.println(F("Command disconnected"));
  fh.close();
  Serial.print(timestamper_console());
  Serial.println(F("Datei geschlossen"));
}

void readSD() {
  fh = SD.open(filename, FILE_READ);
  delay(2000);

  if (!fh) {
    Serial.print(timestamper_console());
    Serial.println(F("Öffnen der Datei fehlgeschlagen"));
    return;
  }

  while (fh.available()) {
    Serial.print(timestamper_console());
    Serial.write(fh.read());
  }

  fh.close();
}
