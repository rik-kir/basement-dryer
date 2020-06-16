void sd_init() {

  Serial.print(F("Initialisierung der SD card..."));

  if (!SD.begin(chipSelect)) {
    Serial.println(F("fehlgeschlagen!"));
    Serial.println(F("System angehalten!"));
    lcd_sdiniterror();
    while (1);
  }
  Serial.println(F("erfolgreich."));

  return;
}

String sd_csv_kopf() {

  return "Datum;Uhrzeit;TempInnen;FeuchteInnen;TpktInnen;TempAussen;FeuchteAussen;TpktAussen;R1;R2;";

}

String sd_csv_kopfdaten() {

  String d = "";
  d = "TempInnen min";
  d += ";";
  d += String(fTempMin, 2);
  d += ";";
  d += "TauPSchw";
  d += ";";
  d += String(fTauPSchw, 2);
  d += ";";
  d += "FeuchteSchwMin";
  d += ";";
  d += String(fFeuchteSchwMin, 2);
  d += ";";
  d += "FeuchteSchwMax";
  d += ";";
  d += String(fFeuchteSchwMax, 2);
  d += ";";
  d += "FeuchteSchwMed";
  d += ";";
  d += String(fFeuchteSchwMed, 2);
  d += ";";

  // Bei den doofen floats den "." durch "," ersetzten, damit die csv Datei richtig gelesen werden kann
  d.replace(".", ",");

  return d;
}

void sd_write(String data) {
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(filename, FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(data);
    dataFile.close();
    // print to the serial port too:
    Serial.print(timestamper_console());
    Serial.println(F("Datensatz in Datei geschrieben"));
    lcd_sdnoerror();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.print(timestamper_console());
    Serial.print(F("Datensatz konnte nicht in Datei geschrieben werden!"));
    Serial.println(filename);
    lcd_sderror();
  }

  return;
}
