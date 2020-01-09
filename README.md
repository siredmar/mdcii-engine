MDCII Game Engine
=================

Ziel des MDCII-Projekts ist eine unabhängige Neuimplementierung der Engine des PC-Spiels **ANNO 1602** unter freier Lizenz.

Gegenwärtig enthält das Projekt die Programme [`mdcii-bshdump`](doc/mdcii-bshdump.md), [`mdcii-bshpacker`](doc/mdcii-bshpacker.md), `mdcii-codcat`, `mdcii-inselbmp`, [`mdcii-sdltest`](doc/mdcii-sdltest.md) und `mdcii-weltbmp`.

Das komplexeste dieser Programme ist [`mdcii-sdltest`](doc/mdcii-sdltest.md), das Spielstände von ANNO 1602 animiert und navigierbar darstellen kann.


Installation
------------

Die Programme erwarten in ihrem Ordner bestimmte Ordner und Dateien aus der ANNO-1602-Installation beziehungsweise vom ANNO-1602-Datenträger.

Die erforderlichen Ordner sind:

- Grafikordner
  - `sgfx`: Grafiken der kleinsten Vergrößerungsstufe
  - `mgfx`: Grafiken der mittleren Vergrößerungsstufe
  - `gfx`: Grafiken der höchsten Vergrößerungsstufe
  - `toolgfx`: Schriftarten und Menügrafiken (letztere ungenutzt)
- Inselordner
  - `nord`: Nördliche Inseln
  - `sued`: Südliche Inseln

Stammen die Dateien von der unerweiterten Originalversion oder der Demoversion, muss `grafiken_1602.txt`, umbennant zu `grafiken.txt`, als Index benutzt werden.
Die üblicherweise in den Ausgabeordner kopierte `grafiken.txt` indiziert die Grafikdateien der NINA-Erweiterung.


Lizenz
------

GPL Version 2 oder neuer, siehe [COPYING](COPYING).