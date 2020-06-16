void Relais_00(int x) {
  if (relais_status != 0) {
    delay(500);
    digitalWrite(R1, HIGH);
    digitalWrite(R2, HIGH);
    Serial.print(timestamper_console());
    Serial.println(F("R1: AUS R2: AUS"));
    daten += "AUS";
    daten += ";";
    daten += "AUS";
    daten += ";";
    if (relais_debug) {
      daten += x;
      daten += ";";
    }
    lcd_relais("00");
    relais_status = 0;
  }
  return 1;
}


void Relais_01(int x) {
  if (relais_status != 1) {
    delay(500);
    digitalWrite(R1, HIGH);
    digitalWrite(R2, LOW);
    Serial.print(timestamper_console());
    Serial.println(F("R1: AUS R2: AN"));
    daten += "AUS";
    daten += ";";
    daten += "AN";
    daten += ";";
    if (relais_debug) {
      daten += x;
      daten += ";";
    }
    lcd_relais("01");
    relais_status = 1;
  }
  return;
}

void Relais_11(int x) {
  if (relais_status != 11) {
    delay(500);
    digitalWrite(R1, LOW);
    digitalWrite(R2, LOW);
    Serial.print(timestamper_console());
    Serial.println(F("R1: AN R2: AN"));
    daten += "AN";
    daten += ";";
    daten += "AN";
    daten += ";";
    if (relais_debug) {
      daten += x;
      daten += ";";
    }
    lcd_relais("11");
    relais_status = 11;
  }
  return;
}

void Relais_10(int x) {
  if (relais_status != 10) {
    delay(500);
    digitalWrite(R1, LOW);
    digitalWrite(R2, HIGH);
    Serial.print(timestamper_console());
    Serial.println(F("R1: AN R2: AUS"));
    daten += "AN";
    daten += ";";
    daten += "AUS";
    daten += ";";
    if (relais_debug) {
      daten += x;
      daten += ";";
    }
    lcd_relais("10");
    relais_status = 10;
  }
  return;
}
