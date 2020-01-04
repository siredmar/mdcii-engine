
// This file is part of the MDCII Game Engine.
// Copyright (C) 2015  Benedikt Freisen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef STRUKTUREN_HPP
#define STRUKTUREN_HPP

#include <inttypes.h>

extern char inselhaus_kennung[];
extern char wiff_kennung[];

struct Ware // 20 bytes
{
  uint32_t verkaufswert : 10; ///< Verkaufswert in Goldstücken
  uint32_t einkaufswert : 10; ///< Einkaufswert in Goldstücken
  uint32_t aktion : 12;       ///< 0: keine, 1: verkaufen, 2: einkaufen
  uint32_t unbekannt;         // i.d.R. 0, einmal 0x80
  uint16_t leer1;             //
  uint16_t lagerstand_soll;   ///< Soll-Lagerstand in 1/32 t
  uint16_t lagerstand_ist;    ///< Ist-Lagerstand in 1/32 t
  uint16_t leer2;             //
  uint16_t ware; // 0x6109 0x6509 0xe105 0xe705 0xe305 0xe905 0xdf05 0xf501 0x0902 0x0502 0x1102 0x0f02 0x3304 0x0d02 0xdd05 0xe505 0x0702 0xfb01 0xf901 0x1502
                 // 0x0302 0xd107 0x0102
  uint16_t leer3;
} __attribute__((packed));


struct Insel3
{
  uint8_t inselnummer; ///< Identifikationsnummer für diese Insel (pro Spiel)
  uint8_t breite;      ///< Breite der Insel in Feldern
  uint8_t hoehe;       ///< Höhe der Insel in Feldern
  uint8_t a;           //
  uint16_t x_pos;      ///< X-Position der Insel auf der Karte
  uint16_t y_pos;      ///< Y-Position der Insel auf der Karte
  uint16_t b;
  uint16_t c;
  uint8_t bytes1[28];

  static constexpr char kennung[] = "INSEL3";
} __attribute__((packed));

struct Erzvorkommen // 8 bytes
{
  uint8_t typ;        ///< 2: Eisen, 3: Gold
  uint8_t x_pos;      ///< X-Position auf der Insel
  uint8_t y_pos;      ///< Y-Position auf der Insel
  uint8_t erkundet;   ///< 0: Erzvorkommen nicht erkundet, 1: Erzvorkommen erkundet
  uint32_t unbekannt; // (neues) kleines Eisenerzvorkommen: 0x0a000001, großes Eisenerzvorkommen: 0x1e000000, Goldvorkommen: 0x06400001
};

struct Insel5
{
  uint8_t inselnummer;   ///< Identifikationsnummer für diese Insel (pro Spiel)
  uint8_t breite;        ///< Breite der Insel in Feldern
  uint8_t hoehe;         ///< Höhe der Insel in Feldern
  uint8_t a;             //
  uint16_t x_pos;        ///< X-Position der Insel auf der Karte
  uint16_t y_pos;        ///< Y-Position der Insel auf der Karte
  uint16_t b;            //
  uint16_t c;            //
  uint8_t bytes0[14];    //
  uint8_t erzvorkommen;  ///< Anzahl an Erzvorkommen (Eisen, Gold) auf dieser Insel (0, 1 oder 2)
  uint8_t erkundet;      ///< 0: Fruchtbarkeit nicht erkundet, 1: Fruchtbarkeit erkundet
  Erzvorkommen erze[2];  ///< Erzvorkommen auf dieser Insel (Daten, ggf. 0)
  uint8_t bytes1[48];    //
  uint8_t fruchtbarkeit; ///< Zeigt an, was auf dieser Insel wie gut wächst
  uint8_t d;             // immer 0x11?
  uint16_t e;            //
  uint16_t basis;        ///< Nummer der Basisinsel (in ihrer Größenordnung)
  uint16_t f;            //
  uint8_t sued;          ///< 0x00 = nord, 0x01 = süd
  uint8_t diff;          ///< 0x00 = eigenständig, 0x01 = basiert auf "basis"
  uint8_t bytes2[14];

  static constexpr char kennung[] = "INSEL5";
} __attribute__((packed));

struct Hirsch2 // 8 bytes
{
  uint8_t inselnummer; //
  uint8_t x_pos;       ///< X-Position auf der Insel
  uint8_t y_pos;       ///< Y-Position auf der Insel
  uint8_t a;           // 0x00
  uint32_t typ;        // 0x00000561

  static constexpr char kennung[] = "HIRSCH2";
};

struct Siedler // 16 bytes
{
  uint8_t inselnummer; ///< Insel, auf der dieses Wohnhaus steht
  uint8_t x_pos;       ///< X-Position auf der Insel
  uint8_t y_pos;       ///< Y-Position auf der Insel
  uint8_t b;           // 0x00 bis 0x07 ?
  uint8_t c;           // 0x00 bis 0x07 ?
  uint8_t stufe;       ///< Entwicklungsstufe (0 bis 4)
  uint16_t nummer;     // der Grafik? (eher nicht); 0x01 bis 0x06
  uint32_t flags;      ///< Zeigt den Zustand der Bewohner an und welche Bedürfnisse befriedigt werden
  uint32_t leer;

  static constexpr char kennung[] = "SIEDLER";
} __attribute__((packed));

struct Rohwachs // 8 bytes
{
  uint8_t inselnummer; ///< Insel, auf der dieses Feld liegt
  uint8_t x_pos;       ///< X-Position auf der Insel
  uint8_t y_pos;       ///< Y-Position auf der Insel
  uint8_t schritt;     ///< Wachstumsschritt (sichtbar)
  uint32_t ani;        //+Typ des Feldes?   0x01/0x07  0x00 bis 0x04   im szenario immer 0x00000000

  static constexpr char kennung[] = "ROHWACHS2";
} __attribute__((packed));

struct Prodlist // 24 bytes
{
  uint8_t inselnummer;         ///< Insel, auf der dieser Betrieb steht
  uint8_t x_pos;               ///< X-Position auf der Insel
  uint8_t y_pos;               ///< Y-Position auf der Insel
  uint8_t leer1;               ///< immer 0
  uint8_t unbekannt;           ///< 0x07, 0x05, 0x04, 0x03, 0x00 (irgendetwas, blockweise konstant, unterscheidet sich pro Spielstand maximal um 1)
  uint16_t produkt_menge;      ///< Lagerstand des von diesem Betrieb hergestellten Produkts
  uint8_t leer2;               ///< immer 0
  uint16_t warteschritt;       //? verbleibende Sekunden: bei 0 werden Lagerstand und Auslastung aktualisiert. 0x00 bis 0x1b, einmal 0x4e
  uint16_t rohstoff2_menge;    ///< Lagerstand des zweiten Rohstoffs dieses Betriebes
  uint16_t rohstoff1_menge;    ///< Lagerstand des ersten Rohstoffs dieses Betriebes
  uint8_t leer3;               ///< immer 0
  uint8_t arbeitsschritt;      // 0x00, 0x80, seltener 0x56, 0x60, 0x40; arbeitsschritt und ani niemals gleichzeitig != 0
  uint16_t auslastung_zaehler; // Auslastung in Prozent etwa = (200 * zaehler) / (256 * nenner) abgerundet
  uint16_t auslastung_nenner;  //
  uint8_t modus;  // 0x00, 0x01, 0x05, 0x15, 0x2d, 0x35, 0x25, 0x19, 0x03, 0x11, 0x29, 0x21, 0x31, 0x39, 0x28  Flag 0x01 = aktiv, 0x40 = nicht abholen
  uint8_t ani;    // Inaktivität? 0x00 bis 0x0f.  0x0f = Rohstoffmangel oder öffentlich; arbeitsschritt und ani niemals gleichzeitig != 0
  uint16_t leer4; ///< immer 0

  static constexpr char kennung[] = "PRODLIST2";
} __attribute__((packed));

struct Werft // 24 bytes
{
  uint8_t inselnummer; ///< Insel, auf der die Werft steht
  uint8_t x_pos;       ///< X-Position auf der Insel
  uint8_t y_pos;       ///< Y-Position auf der Insel
  uint8_t flags;       ///< Flag 0x08 = aktiv, Flags 0x01, 0x02, 0x04 = unbekannt
  uint16_t leer1;      ///< immer 0
  uint8_t
      auftrag; ///< 0xff = kein Auftrag, 0 = kl. Handelsschiff, 1 = gr. Handelsschiff, 2 = kl. Kr., 3 = gr. Kr., 4 = fliegender Händler(!), 5 = Piratenschiff(!)
  uint8_t leer2;        ///< immer 0
  uint16_t fortschritt; ///< Fortschritt (wahrscheinlich in Lebenspunkten), 0x0060 für ca. 5-10% beim kl. Handelsschiff
  uint16_t holz;        ///< Lagerstand Holz (Bug: Lagerstand Stoffe wird nicht mitgespeichert)
  uint16_t ffff;        ///< immer 0xffff
  uint8_t leer3[10];    ///< immer 0

  static constexpr char kennung[] = "WERFT";
} __attribute__((packed));

struct Einheit // 8 bytes     TODO: Wo ist gespeichert, ob die Einheit schon bewaffnet ist?
{
  uint8_t typ;          ///< Typ und Uniformfarbe. 1-4: Infanterist, 5-8: Kavallerist, 9-12: Musketier, 13-16: Kanonier, (vmtl. 33: Eingeborener)
  uint8_t flags;        ///< Flag 0x10: Einheit kann noch nicht ausgesendet werden
  uint16_t unbekannt1;  ///< im Szenario immer 0
  uint16_t fortschritt; ///< Fortschritt in Lebenspunkten: max. 0x280 (Infanterist), 0x240 (Kavallerist), 0x1e0 (Musketier), 0x180 (Kanonier)
  uint16_t unbekannt2;  ///< im Szenario immer 0
};

struct Militar // 112 bytes     TODO: Wo ist das Nicht-bewaffnen-Flag?
{
  uint8_t inselnummer;  ///< Insel, auf der die Burg/große Burg/Festung steht
  uint8_t x_pos;        ///< X-Position auf der Insel
  uint8_t y_pos;        ///< Y-Position auf der Insel
  uint8_t leer1;        ///< vermutlich immer 0
  uint32_t leer2;       ///< vermutlich immer 0
  uint16_t schwerter;   ///< Lagerstand Schwerter
  uint16_t musketen;    ///< Lagerstand Musketen
  uint16_t kanonen;     ///< Lagerstand Kanonen
  uint16_t leer3;       ///< vermutlich immer 0
  Einheit einheiten[8]; ///< Einheiten in der Burg (max. 3), großen Burg (max. 5) oder Festung (max. 8); 0, falls leer
  uint8_t leer4[32];    ///< vermutlich immer 0

  static constexpr char kennung[] = "MILITAR";
} __attribute__((packed));

struct Stadt // 168 bytes
{
  uint8_t inselnummer;              ///< Insel, auf der die Stadt steht
  uint8_t unbekannt0;               ///< Meist 0, manchmal 1
  uint16_t spieler;                 ///< Spieler, dem die Stadt gehört
  uint8_t baumaterialien_entziehen; ///< 0: Dem Volk Baumaterialien überlassen, 1: Dem Volk keine Baumaterialien überlassen
  uint8_t unbekannt1[87];           //
  uint32_t einwohner[5];            ///< Anzahl der Einwohner mit entsprechender Entwicklungsstufe
  uint8_t unbekannt2[15];           //
  uint8_t steuersatz[5];            ///< Steuersatz für jede Entwicklungsstufe (jeweils zwischen 0x40 und 0xc0)
  uint8_t unbekannt3[3];            //
  char name[33];                    ///< Name der Stadt

  static constexpr char kennung[] = "STADT4";
} __attribute__((packed));

struct Kontor // 1004 bytes
{
  uint8_t inselnummer; ///< Insel, auf der dieses Kontor steht
  uint8_t x_pos;       ///< X-Position auf der Insel
  uint8_t y_pos;       ///< Y-Position auf der Insel
  uint8_t spieler;     //
  Ware waren[50];      // 23 Waren ab index 2

  static constexpr char kennung[] = "KONTOR2";
} __attribute__((packed));

struct Markt_intern // 16 bytes
{
  uint16_t unbekannt1;
  uint16_t unbekannt2; // 0x0000, 0x0001, 0x0002, 0x0004 (im gleichen Markt kombinierbar)
  uint16_t unbekannt3; //
  uint16_t leer1;      ///< immer 0
  uint32_t leer2;      ///< immer 0
  uint16_t ware;       // Ware: 0x3304 0x0d02 0xdd05 0xe505 0x0702 0xfb01 0xf901 0x1502 0x0302 0xd107 0x0102
  uint8_t unbekannt4;  // 0x00 oder 0x80, manchmal 0x66 oder 0x50
  uint8_t leer3;       ///< immer 0
} __attribute__((packed));

struct Markt // 260 bytes
{
  uint8_t id;                 ///< fortlaufende Nummerierung der Marktplätze pro Spieler
  uint8_t spieler;            //
  uint16_t a;                 // 0x0000
  Markt_intern unbekannt[16]; //

  static constexpr char kennung[] = "MARKT2";
} __attribute__((packed));

struct Vertrag // 8 bytes
{
  uint32_t status;    ///< 0: nicht vorhanden, 1: biete anderem Spieler an, 2: wird vom anderen Spieler angeboten, 3: besteht
  uint32_t unbekannt; // immer 0?
};

struct Player // 1072 bytes
{
  int32_t kontostand;           ///< Kontostand dieses Spielers
  uint8_t b;                    //
  uint8_t spieler;              ///< Spielernummer? (0 bis 3)
  uint8_t c;                    //
  uint8_t farbe;                ///< Spielerfarbe? (0 bis 3)
  uint8_t unbekannt1[8];        //
  uint16_t gegner_besiegt;      ///< Anzahl besiegter Gegner (jeweils +1500 Punkte)
  uint16_t triumphboegen;       ///< Anzahl bereits gebauter Triumphbögen
  uint16_t soldaten_besiegt;    ///< Anzahl Soldaten, die dieser Spieler besiegt hat
  uint16_t soldaten_gefallen;   ///< Anzahl gefallener Soldaten
  uint16_t schiffe_gesunken;    ///< Anzahl gesunkener Schiffe
  uint16_t schiffe_versenkt;    ///< Anzahl Schiffe, die dieser Spieler versenkt hat
  uint8_t unbekannt2[24];       //
  uint32_t freigeschaltet;      ///< Bitflags für Gruppen freigeschalteter Gebäude
  uint16_t zufriedenheit;       ///< Zufriedenheit der Bevölkerung (Anzahl Denkmäler) Punkte pro Denkmal: zwischen 169,16666 und 169,25
  uint16_t denkmaeler;          ///< Anzahl bereits gebauter Denkmäler
  uint32_t unbekannt3;          ///< immer 0xffffffff?
  uint8_t unbekannt4[264];      //
  Vertrag handelsvertraege[3];  ///< Handelsverträge mit den drei anderen Spielern
  Vertrag friedensvertraege[3]; ///< Friedensverträge mit den drei anderen Spielern
  uint8_t unbekannt5[584];      //
  char name[112];               ///< Name dieses Spielers

  static constexpr char kennung[] = "PLAYER4";
} __attribute__((packed));

struct Auftrag // 1684
{
  uint32_t spieler;   ///< Spieler, für den dieser Auftrag bestimmt ist
  char text[13][128]; ///< 13 Zeilen Text
  char padding[16];

  static constexpr char kennung[] = "AUFTRAG2";
};

struct Ware_handler
{
  uint32_t ware;       ///< Warenkennung
  uint32_t unbekannt1; ///< Verkaufs- oder Einkaufspreis? (Vermutung)
  uint32_t unbekannt2; ///< Verkaufs- oder Einkaufspreis? (Vermutung)
};

struct Handler // 604 bytes
{
  uint32_t unbekannt;
  uint32_t leer[5];
  Ware_handler waren[24];
  uint8_t leer2[292];

  static constexpr char kennung[] = "HANDLER";
};

struct Laderaum
{
  uint16_t ware;   ///< Warenkennung bzw. Einheitenkennung von Soldaten
  uint16_t menge;  ///< Menge in 1/32 t bzw. Gesundheit von Soldaten normiert auf 50t
  uint32_t aktion; ///< 0 = einladen, 1 = ausladen
};

struct Handelsroute // 36 bytes
{
  uint8_t id;           // 0x35, 0x36, 0x37     oder 4 Bytes Kurs für Patrouille
  uint8_t kontornummer; //
  uint16_t leer1;       // 0x0000
  Laderaum ladung[2];   //
  uint8_t leer2[16];    // 0
} __attribute__((packed));

enum class Schiffstyp : uint16_t
{
  KL_HANDELSSCHIFF = 0x15,    ///< kleines Handelsschiff
  GR_HANDELSSCHIFF = 0x17,    ///< großes Handelsschiff
  KL_KRIEGSSCHIFF = 0x19,     ///< kleines Kriegsschiff
  GR_KRIEGSSCHIFF = 0x1b,     ///< großes Kriegsschiff
  FLIEGENDER_HAENDLER = 0x1d, ///< fliegender Händler
  PIRATENSCHIFF = 0x1f,       ///< Piratenschiff
  FAHRENDER_HAENDLER = 0x25,  ///< fahrender Händler
};

struct Ship // 436 bytes
{
  char name[28];         ///< Name des Schiffs, NULL-terminiert
  uint16_t x_pos;        ///< X-Position des Schiffs auf der Karte
  uint16_t y_pos;        ///< Y-Position des Schiffs auf der Karte
  uint32_t a[3];         // 0x00000000 0x00000000 0x00000000
  uint32_t kurs_start;   // Y-Koordinate: Byte2[4:7]Byte3, X-Koordinate: Byte2[0:3]Byte4
  uint32_t kurs_ziel;    // Byte1: 0x00 für stehen, 0x37 für fahren
  uint32_t kurs_aktuell; //
  uint32_t b;            //
  uint16_t lp;           ///< Zustand (Lebenspunkte) des Schiffes
  uint16_t c;            // 0x0000, 0x000c, 0x0010, 0x0011
  uint8_t d1;            // 0, 1, 2, 3, 5, 6
  uint8_t d2;            // 0x50
  uint8_t bewaffnung;    ///< Anzahl Kanonen
  uint8_t flags;         ///< z.B. 0x00, 0x02, 0x08, 0x0a, 0x06, 0x09;  Flag 0x04: zum Verkauf anbieten, 0x02: auf Patrouille
  uint16_t preis;        ///< Verkaufspreis = 18,75 * `preis`.  `preis` reicht von 0x0000 bis 0x0180
  uint16_t id;           ///< aufsteigender Wert, beginnend bei 0, gesunkene Schiffe eingeschlossen
  uint16_t typ;          ///< Schiffstyp.  Auch der fahrende Händler zählt als Schiff!
  uint8_t g;             // 1, 2, 5
  uint8_t spieler;       ///< Spieler, dem das Schiff gehört.  0-3: normale Spieler, 4: fliegender Händler, 5: Piraten
  uint8_t h1;            // 0 bis 6
  uint8_t h2;            // 0xff, 0x10, 0x0d, 0x01, 0x06, 0x0e, 0x0b, 0x11, 0x07, 0x0a
  uint8_t h3;            // 0, selten 1
  uint8_t h4;            // 0 bis 3
  uint16_t richtung;     ///< Gegenwärtige Fahrtrichtung (0 bis 7)

  // nach 82 bytes je 36 bytes pro eintrag

  Handelsroute handelsrouten[8];
  uint16_t j;
  Laderaum ladung[8];

  static constexpr char kennung[] = "SHIP4";
} __attribute__((packed));

struct Soldat // 68 bytes
{
  uint16_t x_pos_2;      ///< X-Position auf der Karte in 1/2 Feldern
  uint16_t y_pos_2;      ///< Y-Position auf der Karte in 1/2 Feldern
  uint16_t lp;           ///< Zustand (Lebenspunkte) der Einheit, z.B. 0x0240, 0x0280, 0x01e0, 0x0180
  uint16_t typ;          ///< Typ und Uniformfarbe. 1-4: Infanterist, 5-8: Kavallerist, 9-12: Musketier, 13-16: Kanonier, 33: Eingeborener
  uint16_t id;           ///< ID (fortlaufend nummeriert)
  uint8_t kurs1[4];      // Byte1: 0x33, 0x34, 0x38
  uint16_t d;            // 0x0000
  uint16_t e;            // 0x0000
  uint8_t f;             // 0x00
  uint8_t unbekannt1;    // Nicht die Inselnummer
  uint8_t unbekannt2[4]; //
  uint8_t spieler;       ///< Spieler, dem die Einheit gehört (0 bis 3 bzw. 6 für Eingeborene)
  uint8_t h;             // 1, 4, 5, 6, 7
  uint8_t i;             // 0x2d (45)
  uint8_t richtung;      ///< Gegenwärtige Blickrichtung (0 bis 7)
  uint8_t leer1;         //
  uint8_t patrouille;    ///< Einheit ist auf Patrouille (0 = nein, 1 = ja)
  uint8_t kurs2[4];      ///< 4 Bytes Kurs
  uint8_t kurs3[4];      ///< 4 Bytes Kurs
  uint8_t leer[30];      ///< Füllbytes

  static constexpr char kennung[] = "SOLDAT3";
};

struct Turm // 36 bytes
{
  uint16_t x_pos_global; ///< X-Position des Turmes auf der Karte (?)
  uint16_t y_pos_global; ///< Y-Position des Turmes auf der Karte (?)
  uint32_t c1;           // 0x00260300 spiel-id?
  uint16_t id;           ///< Fortlaufende Nummerierung inkl. zerstörte Türme
  uint8_t c;             // 0x33
  uint8_t inselnummer;   ///< Nummer der Insel, auf der der Turm steht
  uint8_t x_pos;         ///< X-Position des Turmes auf der Insel
  uint8_t y_pos;         ///< Y-Position des Turmes auf der Insel
  uint16_t d;            // 0x0000
  uint32_t e;            // 0x00000000
  uint16_t f;            // 0x0000
  uint8_t g;             // 0x06
  uint8_t inselnummer2;  //
  uint32_t spieler;      ///< Spieler, dem der Turm gehört
  uint32_t h;            // 0x00000000
  uint32_t i;            // 0x00000000

  static constexpr char kennung[] = "TURM";
};

// struct Timer // (Vielfaches von) 4 bytes
// {
//   uint32_t a;
//
//   static constexpr char kennung[] = "TIMERS";
// };

#endif
