#ifndef _MISSION_HPP_
#define _MISSION_HPP_

#include <inttypes.h>
#include <string>


struct MissionGoodsData // Auftragware, WaremMoni
{
  uint8_t ware;
  uint8_t anz;
};

struct CityMinData // Auftragcity
{
  int32_t wohnanz;
  int32_t bgruppnr;
  uint32_t bgruppwohn;
};

struct WareMinData // Auftragware
{
  uint8_t ware;
  uint8_t leer1;
  uint16_t lager;
};

struct MissionData // Auftrag
{
  int32_t nr;
  uint32_t flags;
  uint32_t looseflags;
  uint32_t leer1[5];
  MissionGoodsData waremono[2];
  uint8_t helpplayernr;
  uint8_t leer2[6];
  uint8_t killanz;
  uint8_t killplayernr[16];
  int32_t killstadtanz;
  int32_t stadtanz;
  int32_t wohnanz;
  int32_t money;
  int32_t bilanz;
  int32_t meldnr;
  int32_t handelsbilanz;
  int32_t stadtanzmin;
  uint16_t stadtanzminfrmd;
  uint16_t leer4[1];
  int32_t leer3[2];
  char infotxt[2048];
  WareMinData waremin[2];
  CityMinData stadtmin[4];
  CityMinData stadtlowfrmd;
  CityMinData stadtminall;
  CityMinData stadtminfrmd;
};

class Mission
{
public:
  Mission(uint8_t* data, uint32_t length, const std::string& name);
  MissionData mission;

private:
  std::string name;
};


#endif // _MISSION_HPP