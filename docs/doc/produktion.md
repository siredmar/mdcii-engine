Ablauf der Produktion
=====================

Notizen zu Änderungen in den Prodlist-Daten zwischen unmittelbar nacheinander gespeicherten Spielständen

    0e 05 29
aktives Gebäude ohne Rohstoffe: warteschritt 3 wahrscheinlich um 4 auf 10 reduziert => ani von 3 auf 4 erhöht und auslastung_nenner von 179 auf 190

    0e 28 10
auslastung_nenner = 231  =>  auslastung_nenner = 121  (= (231+11)/2 )
    0e 0b 18
229 => 120



Vermutung für ani: Anzahl der Nulldurchgänge von warteschritt ohne Produktion




Mit Produktion

    0e 0c 25
    0e 13 16
    0e 13 1e



Vermuteter Ablauf

warteschritt wird sekündlich um 1 verringert.
Bei Nulldurchgang wird
- ein vom Gebäudetyp abhängiger Modulo zu warteschritt addiert.
- der Modulo zu auslastung_nenner addiert
- wenn das Gebäude gerade produziert (d.h. wenn genug Rohstoffe da sind)
  - ein anderer typabhängiger Wert zu auslastung_zaehler addiert
  - ani auf 0 gesetzt
  - ein typabhängiger Wert von rohstoff (und rohstoff2) abgezogen
  - ein typabhängiger Wert zu produkt addiert
- sonst
  - ani um 1 erhöht (nicht über 0x0f)

Übersteigt auslastung_nenner einen bestimmten Wert, werden auslastung_nenner und auslastung_zaehler halbiert.
