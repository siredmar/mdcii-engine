#ifndef _INSEL_5_HPP
#define _INSEL_5_HPP

#include <inttypes.h>

struct OreMountainData // ErzbergData
{
  uint8_t ware;        // Welche Ware liegt hier??
  uint8_t posx;        // Position auf Insel
  uint8_t posy;        // "
  uint8_t playerflags; // Welche Spieler kennen Geheimnis (ACHTUNG: PLAYER_MAX)
  uint8_t kind;        // Welche Ausführung??
  uint8_t leer1;       // Reserve ist immer gut
  uint16_t lager;      // Wieviel liegt hier ??
};

struct Island5Data
{
  uint8_t inselnr;
  uint8_t felderx;
  uint8_t feldery;
  uint8_t strtduerrflg : 1;
  uint8_t nofixflg : 1;
  uint8_t vulkanflg : 1;
  uint16_t posx;
  uint16_t posy;
  uint16_t hirschreviercnt;
  uint16_t speedcnt;
  uint8_t stadtplayernr[11];
  uint8_t vulkancnt;
  uint8_t schatzflg;
  uint8_t rohstanz;
  uint8_t eisencnt;
  uint8_t playerflags;
  OreMountainData eisenberg[4];
  OreMountainData vulkanberg[4];
  uint32_t rohstflags;
  uint16_t filenr;
  uint16_t sizenr;
  uint8_t klimanr;
  uint8_t orginalflg;
  uint8_t duerrproz;
  uint8_t rotier;
  uint32_t seeplayerflags;
  uint32_t duerrcnt;
  uint32_t leer3;
};

class Island5
{
public:
  Island5(uint8_t* data, uint32_t length);

  Island5Data island5;
};

#endif // _INSEL_5_HPP