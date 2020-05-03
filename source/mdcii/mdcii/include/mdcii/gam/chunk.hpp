// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
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

#ifndef _GAM_CHUNKS_HPP_
#define _GAM_CHUNKS_HPP_

#include <inttypes.h>

struct ChunkData
{
  std::string name;
  uint32_t length;
  uint8_t* data;
};

class Chunk
{
public:
  Chunk(std::istream& f);
  ~Chunk();
  ChunkData chunk;
};

// /*========================================+
// I  Struktur EVENT unter Spielers !!       I
// +=========================================*/
// typedef struct
// {
//   uint8_t kind;
//   uint8_t playernr;
//   uint8_t leer1[2];
//   uint32_t timecnt;
// } TRELATION;

// /*========================================+
// I  Struktur Vertrag   !!                  I
// +=========================================*/
// typedef struct
// {
//   uint8_t status;   // Welche Vertragsstufe (keine, angeboten, erhalten, abgeschlossen)
//   uint8_t leer1[3]; // Reserve ist immer gut!!
//   uint32_t timecnt; // Zeitpunkt des Abschlußes
// } TVERTRAG;

// /*========================================+
// I Structur Iso-Objekt-Beschreibung!!      I
// +=========================================*/
// typedef struct
// {
//   uint8_t kind;
//   union {
//     uint8_t inselnr;
//     TFPOS posh;
//   };
//   union {
//     TIPOS pos;
//     uint16_t nr;
//     struct
//     {
//       uint8_t stadtnr;
//       uint8_t leer1;
//     };
//   };
// } TISOOBJ;

// /*========================================+
// I  Struktur AUFTRAGWARE !!                I
// +=========================================*/
// typedef struct
// {
//   uint8_t ware;
//   uint8_t anz;
// } TWAREMONO;

// /*========================================+
// I  Struktur AUFTRAGCITY !!                I
// +=========================================*/
// typedef struct
// {
//   int wohnanz;
//   int bgruppnr;
//   uint32_t bgruppwohn;
// } TSTADTMIN;

// /*========================================+
// I  Struktur AUFTRAGWARE !!                I
// +=========================================*/
// typedef struct
// {
//   uint8_t ware;
//   uint8_t leer1;
//   uint16_t lager;
// } TWAREMIN;

// /*========================================+
// I  Struktur AUFTRAG-Save für Spieler !!   I
// +=========================================*/
// typedef struct
// {
//   int nr;
//   uint32_t flags;
//   uint32_t looseflags;
//   uint32_t leer1[5];
//   TWAREMONO waremono[2];
//   uint8_t helpplayernr;
//   uint8_t leer2[6];
//   uint8_t killanz;
//   uint8_t killplayernr[16];
//   int killstadtanz;
//   int stadtanz;
//   int wohnanz;
//   int money;
//   int bilanz;
//   int meldnr;
//   int handelsbilanz;
//   int stadtanzmin;
//   short stadtanzminfrmd;
//   short leer4[1];
//   int leer3[2];
//   char infotxt[2048];
//   TWAREMIN waremin[2];
//   TSTADTMIN stadtmin[4];
//   TSTADTMIN stadtlowfrmd;
//   TSTADTMIN stadtminall;
//   TSTADTMIN stadtminfrmd;
// } TAUFTRAGSAVE;

// /*========================================+
// I  Spielerstruktur - SAVE !!              I
// +=========================================*/
// typedef struct
// {
//   int money;               // Kapitalstand
//   uint8_t kind;            // Menschliche oder Computerspieler, Piraten, Eingeborene,...
//   uint8_t playernr;        // Spielerstrukturnummer
//   uint8_t humananz;        // Anzahl menschlicher Spieler
//   uint8_t colornr;         // Welche Farbe hat sich Spieler ausgesucht??
//   uint8_t killer;          // Welcher Spieler hat diesem ausgeschalten??
//   uint8_t leer1[1];        // Reserve ist immer gut !!
//   uint16_t stirbcnt;       // Wielange ist Spieler schon Schiffe- und Städtelos??
//   uint8_t auftragnr;       // Welche Aufgabe ist zu meistern ??
//   uint8_t lockflg;         // Spieler darf nicht vom Menschen übernommen werden
//   uint8_t schlossbauanz;   // Es wurde bereits ein Schloss gebaut
//   uint8_t kathedralbauanz; // Es wurde bereits ein Schloss gebaut
//   uint16_t triumphanz;     // Wieviele Triumphbögen hat Spieler bekommen
//   uint16_t triumphbauanz;  // Wieviele Triumphbögen hat Spieler gebaut
//   uint16_t killsoldatanz;  // Wieviele Soldaten wurden besiegt
//   uint16_t losssoldatanz;  // Wieviele Soldaten sind gefallen
//   uint16_t lossshipanz;    // Wieviele Schiffe sind gesunken
//   uint16_t killshipanz;    // Wieviele Schiffe wurden versenkt
//   uint32_t compflags;      // Szenarioinstruktionen für Computergegner??
//   uint16_t humanwohnmax;   // Wieviele Leute dürfen max. in menschlicher Siedlung leben
//   uint16_t wohnmin;        // Ab wahn wird Computergegner sauer ??
//   uint8_t bgruppmin;       // Unter welchen Pegel darf Computer nicht fallen??
//   uint8_t bgruppmaxhuman;  // Uber welchen Pegel darf kein Spieler sein??
//   uint8_t leer3[2];        // Reserve ist immer Gut !!
//   uint32_t leer4[3];       // Reserve ist immer Gut !!
//   uint32_t bauinfraflags;  // Welche Gebäude können bereits gebaut werden??
//   uint16_t denkanz;        // Wieviele Denkmäler hat Spieler bekommen
//   uint16_t denkbauanz;     // Wieviele Denkmäler hat Spieler gebaut
//   uint16_t weltx, welty;   // Wo befand sich Spieler auf der Weltkarte??
//   uint16_t waregive[16];   // Wieviel Waren wurden von Spielern geschenkt??
//   uint16_t waretrad[16];   // Wieviel Ware, die gebraucht wurde, wurden von Spielern geliefert??
//   uint32_t hitpoint[16];   // Wieviel Treffer wurden von Spielern ausgeteilt??
//   TVERTRAG frieden[16];    // Mit wem wurden Friedensverträge vereinbart??
//   TVERTRAG handel[16];     // Mit wem wurden Handelsverträge vereinbart??
//   TRELATION relation[64];  // Ereignisse unter Spielern !!
//   char name[64];           // Voller Spielername
//   char callname[48];       // Abgekürzter Name
// } TPLAYERSAVE;

// /*========================================+
// I  Zeitcounter für die Rechenroutineen    I
// +=========================================*/
// typedef struct
// {
//   uint8_t cnt_stadt;
//   uint8_t cnt_insel;
//   uint8_t cnt_werft;
//   uint8_t cnt_militar;
//   uint8_t cnt_prod;
//   uint8_t cnt_leer4[31];
//   uint8_t cnt_leer3[32];
//   uint8_t cnt_siedler[32];
//   uint8_t cnt_wachs[32];
//   uint32_t time_stadt;
//   uint32_t time_insel;
//   uint32_t time_werft;
//   uint32_t time_militar;
//   uint32_t time_prod;
//   uint32_t time_werkzeugcnt;
//   uint32_t time_werkzeugmax;
//   uint32_t time_game;
//   uint8_t spez_noerzoutflg;
//   uint8_t spez_tutorflg;
//   uint8_t spez_complevel;
//   uint8_t spez_missnr;
//   uint32_t spez_szeneflags;
//   uint32_t spez_gameid;
//   uint32_t spez_stadtnamenr;
//   uint32_t time_nextduerr;
//   uint32_t time_piratsec;
//   uint32_t spez_misssubnr;
//   uint32_t spez_shipmax;
//   TISOOBJ spez_vulkanobj;
//   TISOOBJ spez_vulkanlastobj;
//   uint32_t time_nextvulkan;
//   uint32_t spez_vulkancnt;
//   uint32_t time_leer4[17];
//   uint32_t time_leer3[32];
//   uint32_t time_siedler[32];
//   uint32_t time_wachs[32];
// } TTIMERSAVE;

// /*========================================+
// I  Hirschposition und Daten !!            I
// +=========================================*/
// typedef struct
// {
//   uint32_t inselnr : 8;
//   uint32_t posx : 8;
//   uint32_t posy : 8;
//   uint32_t timecnt;
// } THIRSCHSAVE;

// /*============================================+
// I  Struktur SAVE-Kontorware !!                I
// +=============================================*/
// typedef struct
// {
//   uint32_t vkpreis : 10;
//   uint32_t ekpreis : 10;
//   uint32_t vkflg : 1;
//   uint32_t ekflg : 1;
//   uint32_t lagerres : 16;
//   uint32_t ownlager : 16;
//   uint32_t minlager : 16;
//   uint32_t bedarf : 16;
//   uint32_t lager;
//   uint32_t hausid : 16;
// } TKONTWARESAVE;

// /*============================================+
// I  Struktur SAVE-Kontor!!                     I
// +=============================================*/
// #define MAXKONTWARESAVE 50
// typedef struct
// {
//   uint32_t inselnr : 8;
//   uint32_t posx : 8;
//   uint32_t posy : 8;
//   uint32_t stadtnr : 4;
//   TKONTWARESAVE waren[50]; //  ACHTUNG falls WARE_MAX > 50
// } TKONTORSAVE;

// /*============================================+
// I  Struktur SAVE-Marktware !!                 I
// +=============================================*/
// typedef struct
// {
//   uint32_t wareneed; //  Letzter Verbrauch
//   uint32_t waregive; //  Deckungssatz der Bedürfnisse
//   uint32_t leer1;    //  Reserve ist immer gut !!
//   uint16_t hausid;   //  Hausnummer für Warentyp !!
//   uint8_t wareproz;  //  Versorgungsgrad mit dieser Ware
//   uint8_t leer2;     //  Reserve ist immer gut !!
// } TMARKTWARESAVE;

// /*============================================+
// I   Struktur SAVE-Marktplatz !!               I
// +=============================================*/
// typedef struct
// {
//   uint32_t inselnr : 8;
//   uint32_t stadtnr : 4;     // Auf welcher Insel und Stadt??
//   TMARKTWARESAVE waren[16]; // ACHTUNG: Falls MARKT_WAREMAX > 16
// } TMARKTSAVE;

// /*========================================+
// I  Structur SAVE-Stadt !!                 I
// +=========================================*/
// typedef struct
// {
//   uint8_t inselnr;           //  Nummer der Insel
//   uint8_t stadtnr;           //  Nummer der Stadt auf Insel
//   uint8_t playernr;          //  Spieler der Stadt besitzt
//   uint8_t speedcnt;          //  Zeitzähler
//   uint8_t baustopflg : 1;    //  Siedler brauchen nicht weiter ausbauen
//   uint8_t lastmeldnr;        //  Welche Größe wurde zuletzt gemeldet??
//   uint16_t stimmung;         //  Wieviele Märkte gibt es in Stadt
//   uint32_t einkauf;          //  Wieviel Geld wurde für Wareneinkauf ausgegeben??
//   uint32_t verkauf;          //  Wieviel Geld wurde mit den Warenverkauf gemacht??
//   uint32_t unglueckanz;      //  Wieviele Ungluecke??
//   uint32_t marktanz;         //  Wieviele Märkte gibt es in Stadt
//   uint32_t playercredit[16]; //  Guthaben der Spieler in dieser Stadt!!
//   uint32_t wohnres;          //  Leute, die in Häuser aufgenommen werden wollen
//   uint32_t bgruppwohn[7];    //  Leute je Bevölkerungsgruppe
//   uint8_t bgruppproz[7];     //  Versorgungsquote
//   uint8_t bgruppsteuer[7];   //  Steuerquoten
//   uint8_t nahrproz;          //  "
//   char name[32];             //  Name der Stadt
// } TSTADTSAVE;

// /*========================================+
// I  Beschreibung zum Abspeichern eines     I
// I  Hauses in der Landschaft !!            I
// +=========================================*/
// typedef struct
// {
//   uint16_t id;
//   uint8_t posx;
//   uint8_t posy;
//   uint32_t ausricht : 2;
//   uint32_t animcnt : 4;
//   uint32_t inselnr : 8;
//   uint32_t stadtnr : 3;
//   uint32_t randnr : 5;
//   uint32_t playernr : 4;
// } TISOSAVE;

// /*============================================+
// I  Struktur Waren Abspeichen !!               I
// +=============================================*/
// typedef struct
// {
//   uint16_t hausid;
//   uint16_t menge;
//   union {
//     int kind;
//     int preis;
//     int proz;
//   };
// } TWARESAVE;

// /*========================================+
// I  Structur Save-Händler-Ware !!          I
// +=========================================*/
// typedef struct
// {
//   //   THANDWARE;
//   uint32_t hausid : 16;
// } THANDWARESAVE;

// /*========================================+
// I  Structur Save-Händler !!               I
// +=========================================*/
// typedef struct
// {
//   uint32_t kind : 8;
//   uint32_t objnr : 16;
//   uint32_t handnr : 8;
//   THANDWARESAVE waren[50]; //  ACHTUNG falls WARE_MAX > 50
// } THANDLERSAVE;

// /*============================================+
// I  Struktur SAVE-Ware in Schiffsroute !!      I
// +=============================================*/
// typedef struct
// {
//   uint16_t hausid;
//   uint16_t menge;
//   uint8_t vkflg : 1;
//   uint8_t leer1[3];
// } TROUTWARESAVE;

// /*============================================+
// I  Struktur SAVE-Route für Schiff !!          I
// +=============================================*/
// typedef struct
// {
//   TISOOBJ dstobj;
//   TROUTWARESAVE waren[4]; // ACHTUNG: MAXROUTEWARE
// } TSHIPROUTSAVE;

// /*============================================+
// I  Struktur Waren am Schiff - Abspeichern!!   I
// +=============================================*/
// typedef struct
// {
//   uint32_t hausid : 16;
//   uint32_t menge : 14;
//   uint32_t preis : 10;
// } TSHIPWARESAVE;

// /*============================================+
// I  Struktur Save - Schiffsdaten !!            I
// +=============================================*/
// typedef struct
// {
//   char name[24]; // ACHTUNG: MAXNAMECHAR
//   uint16_t endposx;
//   uint16_t endposy;
//   uint16_t strtposx;
//   uint16_t strtposy;
//   uint16_t homex;
//   uint16_t homey;
//   uint16_t compjob;
//   uint16_t leer1;
//   uint32_t leer2;
//   TISOOBJ homeobj;
//   TISOOBJ dstobj;
//   TISOOBJ srcobj;
//   TISOOBJ fightobj;
//   uint16_t energy;
//   uint16_t userw;
//   uint8_t routecnt;
//   uint8_t radius;
//   uint8_t kanonen;
//   uint8_t endladeflg : 1;
//   uint8_t routeflg : 1;
//   uint8_t vkflg : 1;
//   uint8_t routeonceflg : 1;
//   uint8_t repairflg : 1;
//   uint16_t vkproz;
//   uint16_t objnr;
//   uint16_t figurnr;
//   uint8_t kind;
//   uint8_t playernr;
//   uint8_t handlernr;
//   uint8_t inselnr;
//   uint8_t animnr;
//   uint8_t routenr;
//   uint8_t ausricht;
//   uint8_t verfolgcnt;
//   TSHIPROUTSAVE route[8]; // ACHTUNG: MAXSHIPROUTE
//   TSHIPWARESAVE waren[8]; // ACHTUNG: MAXSHIPWARE
// } TSHIPSAVE;

// /*============================================+
// I  Struktur Route für Schiff !!               I
// +=============================================*/
// typedef struct
// {
//   TISOOBJ dstobj;
// } TSOLDATROUTE;

// /*============================================+
// I  Struktur Save - Soldatendaten !!           I
// +=============================================*/
// typedef struct
// {
//   uint16_t posx;
//   uint16_t posy;
//   uint16_t energy;
//   uint16_t figurnr;
//   uint16_t objnr;
//   TISOOBJ homeobj;
//   TISOOBJ fightobj;
//   TISOOBJ dstobj;
//   uint8_t kind;
//   uint8_t inselnr;
//   uint8_t playernr;
//   uint8_t animnr;
//   uint8_t radius;
//   uint8_t ausricht;
//   uint8_t routenr;
//   uint8_t routeflg : 1;
//   uint8_t routeonceflg : 1;
//   TSOLDATROUTE route[4]; // ACHTUNG: MAXSOLDATROUTE !!
//   uint32_t leer5;
//   uint32_t leer6;
//   uint32_t leer7;
//   uint32_t leer8;
//   uint32_t leer9;
// } TSOLDATSAVE;

// /*============================================+
// I  Struktur Save - Soldatendaten !!           I
// +=============================================*/
// typedef struct
// {
//   uint16_t posx;
//   uint16_t posy;
//   uint16_t energy;
//   uint16_t figurnr;
//   uint8_t kind;
//   uint8_t inselnr;
//   uint8_t playernr;
//   uint8_t animnr;
//   uint8_t ausricht;
//   uint8_t leer[3];
//   TISOOBJ homeobj;
// } TSOLDATINSEL;

// /*============================================+
// I  Struktur Save - Soldatendaten !!           I
// +=============================================*/
// typedef struct
// {
//   uint16_t strtposx;
//   uint16_t strtposy;
//   uint16_t energy;
//   uint16_t figurnr;
//   uint16_t objnr;
//   TISOOBJ homeobj;
//   TISOOBJ fightobj;
//   TISOOBJ dstobj;
//   uint8_t kind;
//   uint8_t inselnr;
//   uint8_t playernr;
//   uint8_t animnr;
//   uint8_t radius;
//   uint8_t ausricht;
//   uint32_t leer5;
//   uint32_t leer6;
// } TTURMSAVE;

// /*========================================+
// I  Structur SAVE-Landschaftsteil !!       I
// +=========================================*/
// typedef struct
// {
//   uint8_t inselnr;
//   uint8_t felderx;
//   uint8_t feldery;
//   uint8_t strtduerrflg : 1;
//   uint8_t nofixflg : 1;
//   uint8_t vulkanflg : 1;
//   uint16_t posx;
//   uint16_t posy;
//   uint16_t hirschreviercnt;
//   uint16_t speedcnt;
//   uint8_t stadtplayernr[11];
//   uint8_t vulkancnt;
//   uint8_t schatzflg;
//   uint8_t rohstanz;
//   uint8_t eisencnt;
//   uint8_t playerflags;
//   TERZBERG eisenberg[4];
//   TERZBERG vulkanberg[4];
//   uint32_t rohstflags;
//   uint16_t filenr;
//   uint16_t sizenr;
//   uint8_t klimanr;
//   uint8_t orginalflg;
//   uint8_t duerrproz;
//   uint8_t rotier;
//   uint32_t seeplayerflags;
//   uint32_t duerrcnt;
//   uint32_t leer3;
// } TINSELSAVE;

// /*========================================+
// I  Beschreibung einer Inselvariante       I
// +=========================================*/
// #define MAXSIZEKIND 5
// typedef struct
// {
//   uint8_t klimanr;
//   uint8_t sizenr;
//   uint8_t nativflg : 1;
//   uint8_t inselnr;
//   uint16_t filenr;
//   uint16_t leer1;
//   int posx, posy;
// } TINSELHEADSAVE;

// /*========================================+
// I  Speichern Beschreibung einer Mission!  I
// +=========================================*/
// typedef struct
// {
//   char name[64];
//   int nativanz[5];
//   int leer1[3];
//   int rohstmax;
//   int inselanz;
//   int goldminsizenr;
//   int goldmaxsizenr;
//   int leer3;
//   TWARESAVE leer2[4];
//   TWARESAVE erze[4];
//   TWARESAVE rohst[12];
//   TWARESAVE goodies[4];
//   TWARESAVE handware[8];
//   TINSELHEADSAVE insel[50];
// } TSZENESAVE;

// /*============================================+
// I  Struktur Soldat im Militärgebäude !!       I
// +=============================================*/
// typedef struct
// {
//   uint32_t figurnr : 12; // Welche Figur befindet sich in der Kaserne?? (FIGUR_MAX <= 256!!)
//   uint32_t trainflg : 1; // Wird gerade eine Einheit ausgebildet
//   uint32_t waffflg : 1;  // Waffe ist für diese Figur bereits vorhanden
//   uint32_t leer1 : 18;   // Reserve ist immer gut
//   uint32_t energy : 16;  // Aktueller Energypegel
//   uint32_t expert : 16;  // Erreichter Ausbildungsstand
// } TMILOBJSAVE;

// /*============================================+
// I  Struktur Militär-Gebäude !!                I
// +=============================================*/
// typedef struct
// {
//   uint32_t inselnr : 8;    //
//   uint32_t posx : 8;       // Standort des Militärischen Gebäudes
//   uint32_t posy : 8;       //
//   uint32_t speedcnt : 8;   // Wenn (Zeitzähler == Speedzähler) timer++ (MAXROHWACHSCNT)
//   uint32_t timer : 16;     // Zeit bis zur nächsten Produktionsrunde
//   uint16_t wafflager[4];   // Aktueller Waffenbestand in der Kaserne
//   TMILOBJSAVE objlist[12]; // Objekte in der Kaserne !!
// } TMILITARSAVE;

// /*============================================+
// I  Struktur SAVE-Siedler !!                   I
// +=============================================*/
// typedef struct
// {
//   uint32_t inselnr : 8;   //
//   uint32_t posx : 8;      // Standort des Wohnhauses.
//   uint32_t posy : 8;      //
//   uint32_t speedcnt : 8;  // Wenn (Zeitzähler == Speedzähler) timer++ (MAXROHWACHSCNT)
//   uint32_t stadtcnt : 8;  // Merker ob Vermehrung neu gerechnet wurde
//   uint32_t bgruppnr : 8;  // Welche Bevölkerungsgruppe??
//   uint32_t marktdist : 8; // Entfernung zum Marktplatz (MAXMARKTDIST)
//   uint32_t anz : 16;      // Anzahl hat einen Kommastellenbereich
//   uint32_t speed : 4;     // Welcher Zeitzähler gilt ??
//   uint32_t leer1 : 1;     // Wird zur Bevölkerungsgruppenr. hinzugezählt
//   uint32_t infraflg : 1;  // Nötigen Infrastruktureinrichtungen vorhanden?
//   uint32_t marktflg : 1;  // im Umkreis eines Marktes ??
//   uint32_t kirchflg : 1;  // im Umkreis einer Kirche ??
//   uint32_t wirtflg : 1;   // im Umkreis eines Wirtshauses ??
//   uint32_t badeflg : 1;   // im Umkreis eines Badehauses ??
//   uint32_t theatflg : 1;  // im Umkreis eines Theaters ??
//   uint32_t hqflg : 1;     // im Umkreis des Hauptquartiers ??
//   uint32_t schulflg : 1;  // im Umkreis einer Schule ??
//   uint32_t hschulflg : 1; // Im Umkreis einer Hochschule ??
//   uint32_t kapellflg : 1; // Im Umkreis einer Kapelle ??
//   uint32_t klinikflg : 1; // im Umkreis eines Arztes ??
//   uint32_t leer5 : 1;     // Reserve ist immer gut
//   uint32_t leer6 : 1;     // Reserve ist immer gut
//   uint32_t leer7 : 8;     // Reserve ist immer gut
//   uint32_t leer8 : 8;     // Reserve ist immer gut
//   uint32_t leer9 : 8;     // Reserve ist immer gut
// } TSIEDLERSAVE;

// /*============================================+
// I   Struktur Save-Prod.-Listen-Eintrag!!      I
// +=============================================*/
// typedef struct
// {
//   uint8_t inselnr;        //
//   uint8_t posx;           //  Wo befindet sich Stätte
//   uint8_t posy;           //
//   uint8_t speed;          //  Welcher Speedzähler (MAXWACHSSPEEDKIND)
//   uint32_t speedcnt : 8;  //  Wenn (Zeitzähler == Speedzähler) timer++ (MAXROHWACHSCNT)
//   uint32_t lager : 24;    //  Lagerbestand an Fertigprodukten
//   uint16_t timer;         //  Zeitzähler in Sekunden
//   uint16_t worklager;     //  Bestand an Betriebsstoffen (vorwiegend Holz)
//   uint32_t rohlager : 24; //  Lagerbestand an Rohstoffen
//   uint32_t rohhebe : 8;   //  Was wird derzeit produziert??
//   uint16_t prodcnt;       //  Wie oft wurde produziert ??
//   uint16_t timecnt;       //  Zähler für Produktionsstatistik!!
//   uint8_t aktivflg : 1;   //  Produktionsstätte derzeit aktiv??
//   uint8_t marktflg : 1;   //  Eine Verbindung mit dem Hauptquartier besteht!!
//   uint8_t animcnt : 4;    //  Aktuelle Animationsstufe (ACHTUNG: TISOFELD!!)
//   uint8_t nomarktflg : 1; //  Marktfahrer dürfen hier keine Waren abholen
//   uint8_t leer3 : 1;      //  Reserve ist immer gut
//   uint8_t norohstcnt : 4; //  Seit längerer Zeit kein Rohstoff erreichbar
//   uint8_t leer1;          //  Reserve ist immer gut
//   uint8_t leer2;          //  Reserve ist immer gut
// } TPRODLISTSAVE;

// /*============================================+
// I   Struktur Save - Werft !!                  I
// +=============================================*/
// typedef struct
// {
//   uint32_t inselnr : 8;    //
//   uint32_t posx : 8;       //  Wo befindet sich Stätte
//   uint32_t posy : 8;       //
//   uint32_t speedcnt : 3;   //  Wenn (Zeitzähler == Speedzähler) timer++ (MAXROHWACHSCNT)
//   uint32_t aktivflg : 1;   //  Wird gerade an einem Schiff gebaut??
//   uint32_t timer : 16;     //  Zeitzähler in Sekunden
//   uint32_t bauship : 8;    //  Welcher Schiffstyp wird produziert
//   uint32_t lager : 16;     //  Wieviel Schiffsbauteile sind schon vorhanden
//   uint32_t rohlager : 16;  //  Lagerbestand an Rohstoffen
//   uint32_t repshipnr : 16; //  Nummer des Schiffes das repariert wird
//   uint32_t worklager : 16; //  Reserve ist immer gut
//   uint32_t leer3 : 8;      //  Reserve ist immer gut
//   uint32_t leer4 : 8;      //  Reserve ist immer gut
//   uint32_t leer5;          //  Reserve ist immer gut
// } TWERFTSAVE;

// /*============================================+
// I   Struktur Save - Rohstoff-Wachstum!!       I
// +=============================================*/
// typedef struct
// {
//   uint32_t inselnr : 8;  //
//   uint32_t posx : 8;     //  Wo befindet sich Stätte
//   uint32_t posy : 8;     //
//   uint32_t speed : 8;    //  Welcher Speedzähler (MAXWACHSSPEEDKIND)
//   uint32_t speedcnt : 8; //  Wenn (Zeitzähler != Speedzähler) aktfeld->animcnt++ (MAXROHWACHSCNT)
//   uint32_t animcnt : 8;  //  Aktuelle Wachstumsstufe !!
// } TROHWACHSSAVE;

#endif // _GAM_CHUNKS_HPP_