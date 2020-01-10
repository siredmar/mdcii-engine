mdcii-bshpacker
===============

Mit `mdcii-bshpacker` steht ein Werkzeug zum Packen der `bsh`-Grafikarchive zur Verfügung.

Bedienung
---------

Das Programm akzeptiert die Folgenden Kommandozeilenparameter:

    Zulässige Optionen:
      -o [ --output ] arg           Zieldatei (*.bsh, *.zei)
      -s [ --signature ] arg (=BSH) Signatur (BSH oder ZEI)
      -f [ --format ] arg (=pnm)    Format (bmp oder pnm)
      -b [ --bpp ] arg (=8)         Bits pro Pixel (8 oder 24)
      -p [ --prefix ] arg (=g_)     Präfix (inklusive Pfad) für die Namen der 
                                    Ausgangsdateien
      -n [ --number ] arg (=1)      Anzahl zu verarbeitenden Bilder (mit Nummern 0 
                                    bis n-1)
      -c [ --color ] arg (=0)       Hintergrundfarbe für transparente Bereiche
      -e [ --extra ] arg (=0)       Extraspalten in den Ausgangsdateien
      -h [ --help ]                 Gibt diesen Hilfetext aus
    
