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

#ifndef _TEMPLATE_HPP
#define _TEMPLATE_HPP

#include <inttypes.h>
#include <string>
#include <vector>

struct PeopleCountByGroup
{
  uint32_t pioneer;
  uint32_t settlers;
  uint32_t citizens;
  uint32_t merchants;
  uint32_t aristocrats;
  uint32_t empty0;
  uint32_t empty1;
};

struct SupplyRateByGroup
{
  uint8_t pioneer;
  uint8_t settlers;
  uint8_t citizens;
  uint8_t merchants;
  uint8_t aristocrats;
  uint8_t empty0;
  uint8_t empty1;
};

struct TaxRateByGroup
{
  uint8_t pioneer;
  uint8_t settlers;
  uint8_t citizens;
  uint8_t merchants;
  uint8_t aristocrats;
  uint8_t empty0;
  uint8_t empty1;
};


struct City4Data // STADT4
{
  uint8_t islandNumber;           //  number of island
  uint8_t cityNumber;             //  city number on the island
  uint8_t playerNumber;           //  player number owning the city
  uint8_t speedCnt;               //  speed counter
  bool expandFlag : 1;            //  people are not allowed to expand
  uint8_t lastReportNumber;       //  which city size was reported last
  uint16_t mood;                  //  ?
  uint32_t goodExpanses;          //  how much money was spent buying goods
  uint32_t goodIncome;            //  how much money was made with selling goods
  uint32_t disasterCount;         //  disasters count occured in the city
  uint32_t marketplaceCount;      //  marketplace count in the city
  uint32_t playerCredit[16];      //  Guthaben der Spieler in dieser Stadt!!
  uint32_t citizenReserve;        //  people that want to get to the city
  PeopleCountByGroup peopleCount; //  people count by cirizen group
  SupplyRateByGroup supplyRate;   //  supply rates by citizen group
  TaxRateByGroup taxRate;         //  tax rates by citizen group
  uint8_t foodSupplyPercentage;   //  percentage of food supplies
  char name[32];                  //  name of the city
};

class City4
{
public:
  City4()
  {
  }
  explicit City4(uint8_t* data, uint32_t length, const std::string& name);
  std::vector<City4Data> city;

private:
  std::string name;
};

#endif // _TEMPLATE_HPP