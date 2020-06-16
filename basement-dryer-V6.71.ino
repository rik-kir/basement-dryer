/*
  Automatische Lüftung

  Programmbeschreibung
  Das Programm erlaubt das automatische Entlüften von Räumen anhand eines günstigen
  Taupunktverhältnisses zwischen innen und aussen mittels eines Arduino Mega und
  den Luftfeuchte/Temperatursensoren des Model DHT22.
  Zudem holt es sich per NTP die aktuelle Uhrzeit, speichert die Messwerte in eine log Datei
  und lädt diese nach Zeitplan auf einen FTP-Server hoch.
  Fehlermeldungen werden auf der Konsole und auf einem 20x4 LCD via I2C ausgegeben.

  Alle Schwellen, Serveradressen, Logindaten etc. können ca. zwischen Zeilen
  112-136 des Programmcodes (und nur dort) angepasst werden.

  Das 1. Relais ist für die reine Lüftung
  Das 2. Relais ist für einen Luftentfeuchter

  Nach Möglichkeit wird die Lüftung des Betriebs des Entfeuchters vorgezogen


  Idee und Codebasis von:
  Thomas Gemander (DJ9ZZZ) tom@dj9zzz.de und Jan Holdstein holdstein1976@googlemail.com
  vom 11 Feb 2017

  Zudem verwendet:
  - Beispielcode aus der TimeLib
  - Beispielcode aus FTP / SD Libary

  modifiziert von rikkir (riktir@mailbox.org) in 2018-2020

  Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
  der GNU General Public License, wie von der Free Software Foundation,
  Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
  veröffentlichten Version, weiterverbreiten und/oder modifizieren.

  Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
  OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
  Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
  Siehe die GNU General Public License für weitere Details.

  Todo (wenn ich mal Lust habe):
  - WinterZeit/SommerZeit Problem lösen

*/

/**
   Grundsätzliches
*/
#include "Wire.h"
#include "math.h"

/**
   Temperatursensoren
*/
#include <DHT.h>
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321

/**
   Display: I2C
*/
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 20, 4);

/**
   NTP und Netzwerk
*/
#include <TimeLib.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
byte mac[] = { 0xDE, 0xAC, 0xBE, 0xEF, 0xFE, 0xED };  // MAC Adresse der NIC
EthernetUDP Udp;
const unsigned int localPort = 8888;                  // lokaler UDP Port

/**
   SD
*/
#include <SD.h>
#include <SPI.h>
const byte chipSelect = 4;
String daten = "";
const String suffix = ".csv";
String filename = "";
String filename_old = "";

/**
   NTP Kram
*/
const unsigned int serverdelay = 3000;
const byte timeZone = 2;               // Zeitversatz Zentraleuropa
const byte NTP_PACKET_SIZE = 48;       // NTP time ist in den ersten 48 bytes enthalten
byte packetBuffer[NTP_PACKET_SIZE];    // Buffer

/**
   FTP Kram
*/
EthernetClient client;
EthernetClient dclient;
char outBuf[128];
char outCount;
File fh;

/**

  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

   Hier könnten Anpassungen nötig sein !!!

  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
*/
const byte DHTPIN1  = A4;   // Pin für den Aussensensor
const byte DHTPIN2  = A5;   // Pin für den Innensensor
const byte R1 = 30;         // Pin für das Relais 1 (Lüfter)
const byte R2 = 31;         // Pin für das Relais 2 (Entfeuchter)

const float fTauPSchw       =   5.0;     //   2.3 Taupunktschwelle, minimale Differenz zwischen TPunkt innen und aussen
const float fTempMin        =   8.0;     //   8.6 Innen-Temperatur welche nicht unterschritten werden darf
const float fFeuchteMin     =  50.0;    //  50.0 Innen-Feuchte die nicht unterschritten werden darf (nur sinnvoll in Weinkeller o.ä.)
                                        //       auf 0 setzen, wenn es nicht eingreifen soll

// Hier wichtig: Die Werte dürfen NICHT so gewählt werden das min+med >= max ist!!!
const float fFeuchteSchwMin = 58.0;     //  60.0 Innen-Feuchte auf die heruntergesenkt werden soll
const float fFeuchteSchwMed =  2.5;     //   5.0 Wert um Schwellwerte so zu ändern, das ein SchaltBEREICH entsteht
const float fFeuchteSchwMax = 65.0;     //  80.0 Innen-Feuchte, ab der das zweite Relais immer scharfgeschaltet wird

const unsigned long intervall = 500; // Durchläufe bis Werte geschrieben/ausgewertet werden 1000 ca. 10min

IPAddress ftpserver( 192, 168, 0, 22 );
IPAddress timeServer(192, 168, 0, 3);
const String ftp_passwort = "Logger";
const String ftp_username = "arduino";

// Wenn true, wird ftp Aktivität in die logdatei geschrieben werden soll
const bool ftp_debug = false;

// Wenn true, wird die csv um eine weiter Spalte erweitert, die angibt WO die Umschaltung der Relais stattfand
const bool relais_debug = true;

// Timestampspeicher für Logik FTP-Upload nach Zeitplan
// Nur den 5ten Wert ändern, wenn man die Upload UHRZEIT vorgeben möchte, wenn nicht = 0
// Der Rest muss genau so bleiben
int start[5] = {1, 0, 0, 0, 2000};

/** XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    Ab hier nichts mehr verändern!!!

    Laufzeit / Durchlauf Zähler / Statuskram / Benötigte Vars
*/
byte relais_status      = 2;   // Status der Relais 2=Start
unsigned long durchlauf = 0;   // Durchlaufzähler
byte schwellen_status   = 0;   // Kommen wir von min = 1, kommen wir von max = 2; sonst 0
float fTPktAussen       = 0, fTPktInnen     = 0; // Speicherung Taupunkte
float fTempAussen       = 0, fTempInnen     = 0; // Speicherung der Temperaturen
float fFeuchteAussen    = 0, fFeuchteInnen  = 0; // Variablen zur Speicherung der Feuchtewerte
DHT sen1(DHTPIN1, DHTTYPE);
DHT sen2(DHTPIN2, DHTTYPE);


void setup() {

  // Serielle Konsole starten
  konsole_start();

  // LCD insten
  lcd_start();

  // Checken, ob die Schwellwerte richtig sind
  if (fFeuchteSchwMin + fFeuchteSchwMed >= fFeuchteSchwMax) {
    lcd_variniterror();
    Serial.println(F("Schwellen Variablen fehlerhaft!"));
    Serial.println(F("System angehalten!"));
    while (1);
  }

  // SD insten
  sd_init();

  // Netzwerk starten
  ethernet_start();

  // NTP starten
  ntp_start();

  // Logdateinamen auf Datum setzen
  filename = String(year());
  if (month() < 10) {
    filename += "0";
  }
  filename += String(month());
  filename += suffix;
  filename_old = filename;

  // Dateikopf ausgeben
  sd_write(daten);
  sd_write(sd_csv_kopfdaten());
  sd_write(sd_csv_kopf());

  // Starttimestamp speichern
  update_start_timestamp();

  // Variablenwerte auf der Konsole ausgeben
  Serial.print(timestamper_console());
  Serial.println(F("Voreinstellungen:"));
  Serial.print(timestamper_console());
  Serial.print(F("Innen-Temperatur min:"));
  Serial.print(F("\t"));
  Serial.println(fTempMin);
  Serial.print(timestamper_console());
  Serial.print(F("Taupunktschwelle:"));
  Serial.print(F("\t"));
  Serial.println(fTauPSchw);
  Serial.print(timestamper_console());
  Serial.print(F("Feuchte Schwelle min:"));
  Serial.print(F("\t"));
  Serial.println(fFeuchteSchwMin);
  Serial.print(timestamper_console());
  Serial.print(F("Feuchte Schwelle max:"));
  Serial.print(F("\t"));
  Serial.println(fFeuchteSchwMax);
  Serial.print(timestamper_console());
  Serial.print(F("Feuchte Schwelle med:"));
  Serial.print(F("\t"));
  Serial.println(fFeuchteSchwMed);

  // Grundinfos anzeigen
  lcd_struktur();

  // Initialisierung der Sensoren
  sensoren_init();

  // Relaispins auf Ausgang setzen
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);

  // die Relaisplatine funktioniert mit AKTIV LOW d.h. HIGH == LüfterAUS
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);
}


void loop() {

  // Lebensanzeige auf LCD starten
  lcd_lebensanzeige();

  // Sensoren auslesen
  sensoren_lesen();

  // LCD befüllen
  lcd_ergebnisse();

  // nur bei jedem xten Intervall die Daten auf die Konsole und ins Log schreiben
  if (durchlauf == intervall || start[0] == 1) { // oder durchlauf == 0

    // Konsole befüllen
    Serial.print(timestamper_console());
    Serial.print(F("TempI: "));
    Serial.print(fTempInnen, 1);
    Serial.print(F(" \t"));
    Serial.print(F("FeuchtI: "));
    Serial.print(fFeuchteInnen, 1);
    Serial.print(F(" \t"));
    Serial.print(F("TauPktI: "));
    Serial.print(fTPktInnen, 1);
    Serial.print(F(" \t"));
    Serial.print(F("TempA: "));
    Serial.print(fTempAussen, 1);
    Serial.print(F(" \t"));
    Serial.print(F("FeuchtA: "));
    Serial.print(fFeuchteAussen, 1);
    Serial.print(F(" \t"));
    Serial.print(F("TauPktA: "));
    Serial.println(fTPktAussen, 1);

    // Ergebnisse der Sensoren bewerten und gewünschte Logik schalten
    sensoren_auswerten();

    // Logdatei befüllen
    sd_write(daten);

    // durchlauf nullen
    durchlauf = 0;
  }

  // per ftp datei hochladen wenn ein Tag um ist oder erster Durchlauf
  if ((((year() > start[1] || month() > start[2] || day() > start[3] ) && ((hour() * 100) + minute() >= start[4]))) || (start[0] == 1)) {

    if (ftp_start()) {
      if (ftp_debug) {
        daten = timestamper();
        daten += "FTP Transfer erfolgreich!";
      }
      lcd_ftpnoerror();
    } else
    {
      if (ftp_debug) {
        daten = timestamper();
        daten += "FTP Transfer fehlgeschlagen!";
      }
      lcd_ftperror();
    }

    // Zeitspeicher akualisieren
    update_start_timestamp();

    // Entfernen der Start-Kennzeichnung beim ersten Durchlauf nach dem Start
    if (start[0] == 1) {
      start[0] = 0;
    }


  }

  // Logrotate - hält den filenamen immer aktuell
  filename = String(year());
  if (month() < 10) {
    filename += "0";
  }
  filename += String(month());
  filename += suffix;

  // csv kopf ausgeben, wenn neue Datei gestartet wird
  if (filename != filename_old) {
    sd_write(" ");
    sd_write(sd_csv_kopfdaten());
    sd_write(sd_csv_kopf());

    // Zeile mit den letzten Werten ausgeben
    daten = timestamper();
    String temp = String(fTempInnen, 2);
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
    temp.replace(".", ",");
    daten += temp;

    if (relais_status == 0) {
      daten += "AUS";
      daten += ";";
      daten += "AUS";
      daten += ";";
    }
    else if (relais_status == 1) {
      daten += "AUS";
      daten += ";";
      daten += "AN";
      daten += ";";
    }

    else if (relais_status == 11) {
      daten += "AN";
      daten += ";";
      daten += "AN";
      daten += ";";
    }

    else if (relais_status == 10) {
      daten += "AN";
      daten += ";";
      daten += "AUS";
      daten += ";";
    }
    else {
      Serial.println(F("Hier ist was schiefgelaufen mit dem Status der Relais!"));
      Serial.println(F("System angehalten"));
      lcd.setCursor(0, 0);
      lcd.print(F("RELAIS MEMORY ERROR"));
      lcd.setCursor(0, 1);
      lcd.print(F("HALTED!"));
      while (true);
    }
    sd_write(daten);
    filename_old = filename;
  }

  // Durchlauf zählen
  durchlauf++;

}
