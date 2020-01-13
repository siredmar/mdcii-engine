Bitflags
========

Bits im `flags`-Attribut der `Siedler`-Struktur
-----------------------------------------------

Noch einzuordnen sind:
- Gesundheit/Krankheit
- Nahrung
- Alkohol
- Tabakwaren
- Gewürze
- Kleidung
- Kakao
- Schmuck

Unbekannte benutzte Bits sind: 6, 8, 9, 11, 16, 17, 19

| Bit | Bedeutung         |
|----:|:------------------|
|   0 |                   |
|   1 |                   |
|   2 |                   |
|   3 |                   |
|   4 |                   |
|   5 |                   |
|   6 |                   |
|   7 | Stoffe            |
|   8 |                   |
|   9 |                   |
|  10 |                   |
|  11 |                   |
|  12 |                   |
|  13 |                   |
|  14 |                   |
|  15 |                   |
|  16 |                   |
|  17 |                   |
|  18 |                   |
|  19 |                   |
|  20 |                   |
|  21 | Zufrieden         |
|  22 | Markt             |
|  23 | Kirche/Kathedrale |
|  24 | Wirtshaus         |
|  25 | Badehaus          |
|  26 | Theater           |
|  27 |                   |
|  28 | Schule            |
|  29 | Hochschule        |
|  30 | Kapelle           |
|  31 | Arzt              |

Bits im `modus`-Attribut der `Prodlist`-Struktur
------------------------------------------------

- bit 0 = aktiv
- bit 6 = nicht abholen

Bits im `fruchtbarkeit`-Attribut der `Insel5`-Struktur
------------------------------------------------------

Die Bits geben an, was zu 100% statt zu 50% wächst.

| Bit | Bedeutung       |
|----:|:----------------|
|   0 | (immer 1)       |
|   1 | 100% Tabak      |
|   2 | 100% Gewürze    |
|   3 | 100% Zuckerrohr |
|   4 | 100% Baumwolle  |
|   5 | 100% Wein       |
|   6 | 100% Kakao      |
|   7 | (immer 1)       |

Bits im `freigeschaltet`-Attribut der `Player`-Struktur
-------------------------------------------------------

Ein gesetztes Bit bedeutet, dass die entsprechenden Gebäude gebaut werden können.

| Bit | Bedingung¹        | Bedeutung
|----:|:------------------|:------------------
|   0 |                   | (ignoriert)
|   1 |                   | (ignoriert)
|   2 | 100 Siedler       | Schule
|   3 | 50 Siedler        | Wirtshaus
|   4 | 150 Bürger        | Kirche
|   5 | 210 Bürger        | Badehaus
|   6 | 300 Kaufleute     | Theater
|   7 | 250 Kaufleute     | Hochschule
|   8 | 50 Bürger         | Arzt
|   9 | 100 Bürger        | Galgen
|  10 | 1500 Aristokraten | Schloss
|  11 | 2500 Aristokraten | Kathedrale
|  12 |                   | (nichts)
|  13 |                   | (nichts)
|  14 | 30 Pioniere       | Fleischerei, Rinderfarm
|  15 | 15 Siedler        | Pflasterstraße, Steinbrücke, Steinbruch, Steinmetz, Feuerwehr
|  16 | 30 Siedler        | Palisade, Holztor, Kontor II
|  17 | 40 Siedler        | Tabakwarenladen, Rumbrennerei, Weingut, Weinstöcke, Zuckerrohrplantage, Zuckerrohrfeld, Tabakplantage, Tabakfeld, Gewürzplantage, Gewürzfeld
|  18 | 75 Siedler        | Windmühle, Bäckerei, Getreidefarm, Getreidefeld
|  19 | 100 Siedler       | Werkzeugschmiede
|  20 | 120 Siedler       | Erzmine, Werft, Erzschmelze
|  21 | 200 Siedler       | kleine Burg, Steinwall, Steintor, Wachturm, Stadttor, Platz I, Schwertschmiede
|  22 | 100 Bürger        | Kontor III
|  23 | 150 Bürger        | Goldmine
|  24 | 200 Bürger        | Schneiderei, Weberei, Baumwollplantage, Baumwollfeld, Kakaoplantage, Kakaofeld
|  25 |                   | (nichts)
|  26 | 400 Bürger        | Kanonengießerei
|  27 | 450 Bürger        | tiefe Erzmine
|  28 | 250 Kaufleute     | Platz II, Platz III, Zierbaum, Goldschmiede, Kontor IV
|  29 | 400 Kaufleute     | große Burg, Musketenbauer
|  30 | 500 Kaufleute     | große Werft
|  31 | 600 Aristokraten  | Festung

1) Quelle: [AnnoWiki 1602](http://1602.annowiki.de/). Die Bedingung muss nur einmal erfüllt gewesen sein.
