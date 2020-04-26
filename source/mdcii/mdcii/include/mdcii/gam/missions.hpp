#ifndef _MISSION_HPP_
#define _MISSION_HPP_

#include <inttypes.h>
#include <string>
#include <variant>
#include <vector>

typedef unsigned char BYTE;
typedef unsigned short UWORD;
typedef unsigned int UINT;

enum class MonopolyGood
{
  ore = 0x02,       // erz
  gold = 0x03,      // gold
  cacao = 0x33,     // kakao
  wool = 0x31,      // baumwolle
  spices = 0x2f,    // gewuerze
  tabaco = 0x2e,    // tabak
  whine = 0x32,     // wein
  sugar_cane = 0x30 // zuckerrohr
};

struct MissionGoodsData // Auftragware, WaremMoni
{
  uint8_t ware; // MonopolyGood
  uint8_t anz;
};

struct CityMinData // Auftragcity
{
  int wohnanz;
  int bgruppnr;
  UINT bgruppwohn;
};

struct WareMinData // Auftragware
{
  BYTE ware;   // index of good
  BYTE leer1;  // empty
  UWORD lager; // amount in 1/32 t (bitshift by 5)
};

struct Mission2Data // Auftrag2
{
  uint32_t nr;           // mission for specific player (0...3)
  char infotxt[13][128]; // 13 lines of text
  char padding[16];      // empty
};

struct Mission4Flags
{
  uint8_t city0 : 1;          // 0, 0x00001, enables goal for city 1
  uint8_t city1 : 1;          // 1, 0x00002, enables goal for city 2
  uint8_t city2 : 1;          // 2, 0x00004, enables goal for city 3
  uint8_t money : 1;          // 3, 0x00008, enables money
  uint8_t helpplayer : 1;     // 4, 0x00010, enables stadtminfrmd, enables helpplayernr,
  uint8_t empty1 : 1;         // 5, 0x00020, empty
  uint8_t empty2 : 1;         // 6, 0x00040, empty
  uint8_t wohnanz : 1;        // 7, 0x00080, enables how much settlers overall sum
  uint8_t balance : 1;        // 8, 0x00100, enables bilanz
  uint8_t empty3 : 1;         // 9, 0x00200, enables handeslsbilanz
  uint8_t killplayernr : 1;   // 10, 0x00400, enables killplayernr[]
  uint8_t empty4 : 1;         // 11, 0x00800, empty
  uint8_t store1 : 1;         // 12, 0x01000, enables waremin[0]
  uint8_t store2 : 1;         // 13, 0x02000, enables waremin[1]
  uint8_t monopoly1 : 1;      // 14, 0x04000, enables monopoly for good -> waremono[0]
  uint8_t monopoly2 : 1;      // 15, 0x08000, enables monopoly for good -> waremono[1]
  uint8_t tradingbalance : 1; // 16, 0x10000
  uint8_t palace : 1;         // 17, 0x20000, a palace needs to be built
  uint8_t cathedral : 1;      // 18, 0x40000, a cathedrale needs to be built
  uint8_t empty6 : 5;
  uint8_t empty7;
};

struct Mission4LooseFlags
{
  uint8_t stadtlowfrmd : 1;    // enables stadtlowfrmd
  uint8_t stadtanzminfrmd : 1; // enables stadtanzminfrmd
  uint8_t empty1 : 2;          // empty
  uint8_t stadtminfrmd : 1;    // enables stadtminfrmd
  uint8_t emprty2 : 3;
  uint32_t empty3 : 24;
};

struct Mission4Data // Auftrag4
{
  int nr;                        // mission for specific player (0...3)
  Mission4Flags flags;           // flags that enable specific parts of the mission to win the game
  Mission4LooseFlags looseflags; // flags that enable specific parts of the mission to loose the game
  UINT empty[5];                 // empty
  MissionGoodsData waremono[2];  // monopoly for goods
  BYTE helpplayernr;             // which player shall be helped: 0, 1, 2, 3 (players), 6 (native), 7 (any)
  BYTE leer2[6];                 // empty
  BYTE killanz;                  // unused? Number of opponents to be killed?
  BYTE killplayernr[16];         // which players shall be killed ([0]: 0, ...[3]: 3, [4] not set, [5]: any, [6]: pirates)
  int killstadtanz;              // unused? Number of opponents cities to be destroyed?
  int stadtanz;                  // unused? Number of own cities to be built? maybe number for stadtmin?
  int wohnanz;                   // how much settlers must be reached (overall sum)
  int money;                     // how much money shall be reached
  int bilanz;                    // how good must the balance sheet be
  int meldnr;                    // unused?
  int handelsbilanz;             // how good must the trading balance sheet be
  int stadtanzmin;               // unused? maybe number for stadtmin?
  short stadtanzminfrmd;         // foreign allied city goals to _hold_ number of _cities_
  short leer4[1];                // empty
  int leer3[2];                  // empty
  char infotxt[2048];            // mission info text
  WareMinData waremin[2];        // goal to have specific amount of goods. Enabled by `store1` and `store2` in flags
  CityMinData stadtmin[4];       // goal to reach cities with specific settlers. Enabled by `city1`, `city2` and `city3` in flags
  CityMinData stadtlowfrmd;      // foreign allied city goal to _hold_ specific amount of _settlers_
  CityMinData stadtminall;       // unused?
  CityMinData stadtminfrmd;      // foreign allied city goal to _reach_ specific amount of _settlers_
};

class Mission4
{
public:
  Mission4(uint8_t* data, uint32_t length, const std::string& name);
  // multiple Missions are placed in one chunk
  std::vector<Mission4Data> missions;

private:
  std::string name;
};

class Mission2
{
public:
  Mission2(uint8_t* data, uint32_t length, const std::string& name);
  // multiple Missions are placed in one chunk
  std::vector<Mission2Data> missions;

private:
  std::string name;
};

#endif // _MISSION_HPP