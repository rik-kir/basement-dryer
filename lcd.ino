void lcd_start() {
  lcd.init();
  lcd.backlight();

  return;
}

void lcd_struktur() {
  lcd.setCursor(19, 0);
  lcd.print(F(":"));
  lcd.setCursor(0, 0);
  lcd.print(F("Innen"));
  lcd.setCursor(9, 0);
  lcd.print(F("Aussen"));
  lcd.setCursor(0, 1);
  lcd.print(F("T:"));
  lcd.setCursor(9, 1);
  lcd.print(F("T:"));
  lcd.setCursor(0, 2);
  lcd.print(F("TP:"));
  lcd.setCursor(9, 2);
  lcd.print(F("TP:"));
  lcd.setCursor(0, 3);
  lcd.print(F("F:"));
  lcd.setCursor(18, 1);
  lcd.print(F("F"));
  lcd.setCursor(18, 2);
  lcd.print(F("S"));
  lcd.setCursor(16, 3);
  lcd.print(F("R:"));

  return;
}

void lcd_ergebnisse() {
  lcd.setCursor(11, 1);
  lcd.print(fTempAussen, 1);
  lcd.setCursor(12, 2);
  lcd.print(fTPktAussen, 1);
  lcd.setCursor(2, 1);
  lcd.print(fTempInnen, 1);
  lcd.setCursor(3, 2);
  lcd.print(fTPktInnen, 1);
  lcd.setCursor(3, 3);
  lcd.print(fFeuchteInnen, 1);
  
  return;
}

void lcd_lebensanzeige() {
  lcd.setCursor(19, 0);
  if (durchlauf % 2 == 0) {
    lcd.print(F("."));
  } else {
    lcd.print(F(":"));
  }
  return;
}

void lcd_relais(String x) {
  lcd.setCursor(18, 3);
  lcd.print(x);

  return;
}

void lcd_ftperror() {
  lcd.setCursor(19, 1);
  lcd.print(F("E"));

  return;
}

void lcd_ftpnoerror() {
  lcd.setCursor(19, 1);
  lcd.print(F("1"));

  return;
}

void lcd_sderror() {
  lcd.setCursor(19, 2);
  lcd.print(F("E"));

  return;
}

void lcd_sdnoerror() {
  lcd.setCursor(19, 2);
  lcd.print(F("1"));

  return;
}

void lcd_sdiniterror() {
  lcd.setCursor(0, 0);
  lcd.print(F("SD ERROR"));
  lcd.setCursor(0, 1);
  lcd.print(F("HALTED!"));

  return;
}

void lcd_variniterror() {
  lcd.setCursor(0, 0);
  lcd.print(F("VAR ERROR"));
  lcd.setCursor(0, 1);
  lcd.print(F("HALTED!"));

  return;
}
