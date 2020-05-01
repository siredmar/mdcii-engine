#ifndef _MISSION_HPP_
#define _MISSION_HPP_

#include <inttypes.h>
#include <string>
#include <variant>
#include <vector>

enum class MonopolyGood : uint8_t
{
  Ore = 0x02,    // Erz
  Gold = 0x03,   // Gold
  Tabaco = 0x2e, // Tabak
  Spices = 0x2f, // Gewuerze
  Sugar = 0x30,  // Zuckerrohr
  Wool = 0x31,   // Baumwolle
  Whine = 0x32,  // Wein
  Cacao = 0x33   // Kakao
};

struct MissionGoods
{
  MonopolyGood ware;
  uint8_t anz;
};

enum class BGruppWohn : int32_t
{
  Pioneer = 0,
  Settlers = 1,
  Citizens = 2,
  Merchants = 3,
  Aristocrats = 4
};

struct CityMin
{
  int32_t wohnanz;
  BGruppWohn bgruppnr;
  uint32_t bgruppwohn;
};

enum class Goods : uint8_t
{
  None = 0,
  Empty = 1,
  Ore = 2,        // Eisenerz
  Gold = 3,       // Gold
  Wool = 4,       // Wolle
  Sugar = 5,      // Zucker
  Tabaco = 6,     // Tabak
  Cattle = 7,     // Schlachtvieh
  Corn = 8,       // Korn
  Flour = 9,      // Mehl
  Iron = 10,      // Eisen
  Sword = 11,     // Schwerter
  Musket = 12,    // Musketen
  Canon = 13,     // Kanonen
  Food = 14,      // Nahrung
  Cigaretts = 15, // Tabakwaren
  Spices = 16,    // Gewuerze
  Cacao = 17,     // Kakao
  Alcohol = 18,   // Alkohol
  Fabric = 19,    // Stoffe
  Clothing = 20,  // Kleidung
  Jewellery = 21, // Schmuck
  Tools = 22,     // Werkzeug
  Wood = 23,      // Holz
  Stone = 24,     // Ziegel
};

struct WareMin
{
  Goods ware;      // index of good // TODO find out good indexes
  uint8_t leer1;   // empty
  uint16_t amount; // amount in 1/32 t (bitshift by 5)
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
  uint8_t empty6 : 5;         // empty
  uint8_t empty7;             // empty
};

struct Mission4LooseFlags
{
  uint8_t stadtlowfrmd : 1;    // enables stadtlowfrmd
  uint8_t stadtanzminfrmd : 1; // enables stadtanzminfrmd
  uint8_t empty1 : 2;          // empty
  uint8_t stadtminfrmd : 1;    // enables stadtminfrmd
  uint8_t empty2 : 3;          // empty
  uint32_t empty3 : 24;        // empty
};

enum class EndMovie : int
{
  EndMovieDisabled = 0x54,
  EndMovie1 = 0x55,
  EndMovie2 = 0x56,
  EndMovie3 = 0x57,
  EndMovie4 = 0x5F,
  EndMovie5 = 0x60,
  EndMovie6 = 0x62,
  EndMovie7 = 0x63,
  EndMovie8 = 0x64,
  EndMovie9 = 0x65,
  EndMovie10 = 0x66,
  EndMovie11 = 0x67,
  EndMovie12 = 0x68,
  EndMovie13 = 0x69,
  EndMovie14 = 0x6A,
  EndMovie15 = 0x6B
};

enum class HelpPlayer : uint8_t
{
  Player0 = 0,
  Player1 = 1,
  Player2 = 2,
  Player3 = 3,
  AnyPlayer = 7
};

enum class MissionNumber : uint32_t
{
  Mission0 = 0,
  Mission1 = 1,
  Mission2 = 2,
  Mission3 = 3
};

struct Mission4Data // Auftrag4
{
  MissionNumber nr;              // mission number, max 4 missions
  Mission4Flags flags;           // flags that enable specific parts of the mission to win the game
  Mission4LooseFlags looseflags; // flags that enable specific parts of the mission to loose the game
  uint32_t empty1[5];            // empty
  MissionGoods waremono[2];      // monopoly for goods
  HelpPlayer helpPlayer;         // which player shall receive help
  uint8_t empty2[6];             // empty
  uint8_t killanz;               // unused? Number of opponents to be killed?
  uint8_t killplayernr[16];      // which players shall be killed ([0]: any, [1] ... [4]: players 1 - 4, [5] not set, [5]: empty, [6]: pirates, [7...15] empty)
  int32_t killstadtanz;          // unused? Number of opponents cities to be destroyed?
  int32_t stadtanz;              // unused? Number of own cities to be built? maybe number for stadtmin?
  int32_t wohnanz;               // how much settlers must be reached (overall sum)
  int32_t money;                 // how much money shall be reached
  int32_t bilanz;                // how good must the `balance sheet` be
  EndMovie endMovie;             // movie number that is shown when mission is finished
  int32_t handelsbilanz;         // how good must the `trading balance sheet` be
  int32_t stadtanzmin;           // unused? maybe number for stadtmin?
  int16_t stadtanzminfrmd;       // foreign allied city goal to `_hold_ number of _cities_`
  uint8_t empty3[10];            // empty
  char infotxt[2048];            // mission info text
  WareMin waremin[2];            // goal to have specific amount of goods. Enabled by `store1` and `store2` in flags
  CityMin stadtmin[4];           // goal to reach cities with specific settlers. Enabled by `city1`, `city2` and `city3` in flags
  CityMin stadtlowfrmd;          // foreign allied city goal to `_hold_ specific amount of _settlers_`
  CityMin stadtminall;           // unused?
  CityMin stadtminfrmd;          // foreign allied city goal to `_reach_ specific amount of _settlers_`
};

class Mission4
{
public:
  Mission4(uint8_t* data, uint32_t length, const std::string& name);
  std::vector<Mission4Data> missions;

private:
  std::string name;
};

struct Mission2Data // Auftrag2
{
  MissionNumber nr;      // mission for specific player (0...3)
  char infotxt[13][128]; // 13 lines of text
  char padding[16];      // empty
};

class Mission2
{
public:
  Mission2(uint8_t* data, uint32_t length, const std::string& name);
  std::vector<Mission2Data> missions;

private:
  std::string name;
};

#endif // _MISSION_HPP