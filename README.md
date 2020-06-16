# basement-dryer

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
  - Sensoren via I2C
  - Schwellwerte einstellbar per Drehtaster
