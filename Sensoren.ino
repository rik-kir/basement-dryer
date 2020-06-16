void sensoren_lesen() {

  String temp = "";

  // solange lesen bis sinnvolle Werte aussen
  do {
    fFeuchteAussen = sen1.readHumidity();
    fTempAussen = sen1.readTemperature();
    delay(200);
  } while (isnan(fFeuchteAussen) || isnan(fTempAussen));

  // Berechnung der Taupunktes Aussen
  fTPktAussen = berechnungTaupunkt(fFeuchteAussen, fTempAussen);

  // solange lesen bis sinnvolle Werte innen
  do {
    fFeuchteInnen = sen2.readHumidity();
    fTempInnen = sen2.readTemperature();
    delay(200);
  } while (isnan(fFeuchteInnen) || isnan(fTempInnen));

  // Berechnung der Taupunktes Innen
  fTPktInnen = berechnungTaupunkt(fFeuchteInnen, fTempInnen);

  //Logstring befüllen
  daten = timestamper();

  temp = String(fTempInnen, 2);
  temp += ";";
  temp += String(fFeuchteInnen, 2);
  temp += ";";
  temp += String(fTPktInnen, 2);
  temp += ";";
  temp += String(fTempAussen, 2);
  temp += ";";
  temp += String(fFeuchteAussen, 2);
  temp += ";";
  temp += String(fTPktAussen, 2);
  temp += ";";
  // Bei den doofen floats den "." durch "," ersetzten, damit die csv Datei richtig gelesen werden kann
  temp.replace(".", ",");
  daten += temp;

  return;
}

float berechnungTaupunkt(float feuchte, float temp) {
  return (243.12 * ((log10(feuchte) - 2.0) / 0.4343 + (17.62 * temp) / (243.12 + temp)) / (17.62 - ((log10(feuchte) - 2.0) / 0.4343 + (17.62 * temp) / (243.12 + temp))));
}

void sensoren_init() {
  sen1.begin();
  sen2.begin();

  return;
}

void sensoren_auswerten() {
  // 1. Relais Lüftung
  // 2. Relais Entfeuchter

  // Feuchte UNTER min
  if (fFeuchteInnen < fFeuchteSchwMin) {
    // wenn TP OK und FeuchteInnen nicht zu niedrig: lüften ansonsten nix machen
    if (TP_OK() && fFeuchteInnen > fFeuchteMin) {
      Relais_10(1);
    } else {
      Relais_00(1);
    }
    schwellen_status = 1;
  }
  // Feuchte ÜBER max
  else if (fFeuchteInnen > fFeuchteSchwMax) {
    if (TP_OK()) {
      Relais_11(2);
    }
    // TP nicht gut undoder TempInnen zu nierig
    else {
      Relais_01(3);
    }
    schwellen_status = 2;
  }
  // Ab hier ist die Feuchte zwischen min und max
  //
  // Neutrale Stellung ODER
  // Feuchte größer min+med und wir kommen von min ODER
  // Feuchte kleiner max-med und wir kommen von max ODER
  // Erster Durchlauf
  else if (schwellen_status == 0 ||
           ((fFeuchteInnen > (fFeuchteSchwMin + fFeuchteSchwMed)) && schwellen_status == 1) ||
           ((fFeuchteInnen < (fFeuchteSchwMax - fFeuchteSchwMed)) && schwellen_status == 2) ||
           (start[0] == 1) ) {
    if (TP_OK()) {
      Relais_10(4);
    }
    // TP nicht gut undoder TempInnen zu nierig
    else {
      Relais_01(5);
    }
    schwellen_status = 0;
  }
  // überprüfen ob TP noch ok
  else {
    if (TP_OK()) {
      if (relais_status == 11) {
        Relais_11(6);
      }
      else if(relais_status == 1 && schwellen_status == 2 && (fFeuchteInnen > (fFeuchteSchwMax - fFeuchteSchwMed))){
        Relais_11(7);
      }
      else {
        Relais_10(8);
      }
    }
    else if (relais_status != 0) {
      Relais_01(9);
    }
  }
  return;
}

// Prüft ob TP Verhältnisse ok sind UND die Innentemperatur nicht zu niedrig ist
bool TP_OK() {
  
  return (((fTPktAussen + fTauPSchw) < fTPktInnen) && (fTempInnen >= fTempMin));
}
