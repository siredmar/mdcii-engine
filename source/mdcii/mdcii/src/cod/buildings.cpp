
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

#include <experimental/optional>
#include <map>
#include <memory>

#include "cod/buildings.hpp"

Buildings* Buildings::_instanceRawPtr = 0;

void Buildings::CreateInstance(const std::shared_ptr<CodParser> cod)
{
  static CGuard g;
  if (!_instanceRawPtr)
  {
    _instanceRawPtr = new Buildings(cod);
  }
}

Buildings::Buildings(std::shared_ptr<CodParser> cod)
  : cod(cod)
{
  GenerateBuildings();
}

std::shared_ptr<Buildings> Buildings::Instance()
{
  if (not _instanceRawPtr)
  {
    throw std::string("[Err] Haeuser not initialized yet!");
  }
  static std::shared_ptr<Buildings> _instance = std::make_shared<Buildings>(*_instanceRawPtr);
  return _instance;
}

std::experimental::optional<Building*> Buildings::GetHouse(int id)
{
  if (buildings.find(id) == buildings.end())
  {
    return {};
  }
  else
  {
    return &buildings[id];
  }
}

int Buildings::GetBuildingsSize()
{
  return buildingsVector.size();
}

Building* Buildings::GetBuildingByIndex(int index)
{
  return buildingsVector[index];
}

void Buildings::GenerateBuildings()
{
  for (int o = 0; o < cod->objects.object_size(); o++)
  {
    auto obj = cod->objects.object(o);
    if (obj.name() == "HAUS")
    {
      for (int i = 0; i < obj.objects_size(); i++)
      {
        auto haus = GenerateBuilding(&obj.objects(i));
        buildings[haus.Id] = haus;
        buildingsVector.push_back(&buildings[haus.Id]);
      }
    }
  }
}

Building Buildings::GenerateBuilding(const cod_pb::Object* obj)
{
  Building h;
  if (obj->has_variables() == true)
  {
    for (int i = 0; i < obj->variables().variable_size(); i++)
    {
      auto var = obj->variables().variable(i);
      if (var.name() == "Id")
      {
        if (var.value_int() == 0)
        {
          h.Id = 0;
        }
        else
        {
          h.Id = var.value_int() - idOffset;
        }
      }
      else if (var.name() == "Gfx")
      {
        h.Gfx = var.value_int();
      }
      else if (var.name() == "Blocknr")
      {
        h.Blocknr = var.value_int();
      }
      else if (var.name() == "Kind")
      {
        h.Kind = ObjectKindMap[var.value_string()];
      }
      else if (var.name() == "Posoffs")
      {
        h.Posoffs = var.value_int();
      }
      else if (var.name() == "Wegspeed")
      {
        for (int v = 0; v < var.value_array().value_size(); v++)
        {
          h.Wegspeed.push_back(var.value_array().value(v).value_int());
        }
      }
      else if (var.name() == "Highflg")
      {
        h.Highflg = var.value_int();
      }
      else if (var.name() == "Einhoffs")
      {
        h.Einhoffs = var.value_int();
      }
      else if (var.name() == "Bausample")
      {
        h.Bausample = BausampleMap[var.value_string()];
      }
      else if (var.name() == "Ruinenr")
      {
        h.Ruinenr = RuinenrMap[var.value_string()];
      }
      else if (var.name() == "Maxenergy")
      {
        h.Maxenergy = var.value_int();
      }
      else if (var.name() == "Maxbrand")
      {
        h.Maxbrand = var.value_int();
      }
      else if (var.name() == "Size")
      {
        h.Size.w = var.value_array().value(0).value_int();
        h.Size.h = var.value_array().value(1).value_int();
      }
      else if (var.name() == "Rotate")
      {
        h.Rotate = var.value_int();
      }
      else if (var.name() == "RandAnz")
      {
        h.RandAnz = var.value_int();
      }
      else if (var.name() == "AnimTime")
      {
        if (var.value_string() == "TIMENEVER")
        {
          h.AnimTime = -1;
        }
        else
        {
          h.AnimTime = var.value_int();
        }
      }
      else if (var.name() == "AnimFrame")
      {
        h.AnimFrame = var.value_int();
      }
      else if (var.name() == "AnimAdd")
      {
        h.AnimAdd = var.value_int();
      }
      else if (var.name() == "Baugfx")
      {
        h.Baugfx = var.value_int();
      }
      else if (var.name() == "PlaceFlg")
      {
        h.PlaceFlg = var.value_int();
      }
      else if (var.name() == "AnimAnz")
      {
        h.AnimAnz = var.value_int();
      }
      else if (var.name() == "KreuzBase")
      {
        h.KreuzBase = var.value_int();
      }
      else if (var.name() == "NoShotFlg")
      {
        h.NoShotFlg = var.value_int();
      }
      else if (var.name() == "Strandflg")
      {
        h.Strandflg = var.value_int();
      }
      else if (var.name() == "Ausbauflg")
      {
        h.Ausbauflg = var.value_int();
      }
      else if (var.name() == "Tuerflg")
      {
        h.Tuerflg = var.value_int();
      }
      else if (var.name() == "Randwachs")
      {
        h.Randwachs = var.value_int();
      }
      else if (var.name() == "RandAdd")
      {
        h.RandAdd = var.value_int();
      }
      else if (var.name() == "Strandoff")
      {
        h.Strandoff = var.value_int();
      }
      else if (var.name() == "Destroyflg")
      {
        h.Destroyflg = var.value_int();
      }
    }
  }
  if (obj->objects_size() > 0)
  {
    for (int o = 0; o < obj->objects_size(); o++)
    {
      auto nestedObj = obj->objects(o);
      if (nestedObj.name() == "HAUS_PRODTYP")
      {
        for (int i = 0; i < nestedObj.variables().variable_size(); i++)
        {
          auto var = nestedObj.variables().variable(i);
          if (var.name() == "Kind")
          {
            h.HouseProductionType.Kind = ProdtypKindMap[var.value_string()];
          }
          else if (var.name() == "Ware")
          {
            h.HouseProductionType.Ware = WareMap[var.value_string()];
          }
          else if (var.name() == "Workstoff")
          {
            h.HouseProductionType.Workstoff = WorkstoffMap[var.value_string()];
          }
          else if (var.name() == "Erzbergnr")
          {
            h.HouseProductionType.Erzbergnr = ErzbergnrMap[var.value_string()];
          }
          else if (var.name() == "Rohstoff")
          {
            h.HouseProductionType.Rohstoff = RohstoffMap[var.value_string()];
          }
          else if (var.name() == "MAXPRODCNT")
          {
            h.HouseProductionType.Maxprodcnt = MaxprodcntMap[var.value_string()];
          }
          else if (var.name() == "Bauinfra")
          {
            h.HouseProductionType.Bauinfra = BauinfraMap[var.value_string()];
          }
          else if (var.name() == "Figurnr")
          {
            h.HouseProductionType.Figurnr = FigurnrMap[var.value_string()];
          }
          else if (var.name() == "Rauchfignr")
          {
            h.HouseProductionType.Rauchfignr = RauchfignrMap[var.value_string()];
          }
          else if (var.name() == "Maxware")
          {
            for (int v = 0; v < var.value_array().value_size(); v++)
            {
              h.HouseProductionType.Maxware.push_back(var.value_array().value(v).value_int());
            }
          }
          else if (var.name() == "Kosten")
          {
            for (int v = 0; v < var.value_array().value_size(); v++)
            {
              h.HouseProductionType.Kosten.push_back(var.value_array().value(v).value_int());
            }
          }
          else if (var.name() == "BGruppe")
          {
            h.HouseProductionType.BGruppe = var.value_int();
          }
          else if (var.name() == "LagAniFlg")
          {
            h.HouseProductionType.LagAniFlg = var.value_int();
          }
          else if (var.name() == "NoMoreWork")
          {
            h.HouseProductionType.NoMoreWork = var.value_int();
          }
          else if (var.name() == "Workmenge")
          {
            h.HouseProductionType.Workmenge = var.value_int();
          }
          else if (var.name() == "Doerrflg")
          {
            h.HouseProductionType.Doerrflg = var.value_int();
          }
          else if (var.name() == "Anicontflg")
          {
            h.HouseProductionType.Anicontflg = var.value_int();
          }
          else if (var.name() == "MakLagFlg")
          {
            h.HouseProductionType.MakLagFlg = var.value_int();
          }
          else if (var.name() == "Nativflg")
          {
            h.HouseProductionType.Nativflg = var.value_int();
          }
          else if (var.name() == "NoLagVoll")
          {
            h.HouseProductionType.NoLagVoll = var.value_int();
          }
          else if (var.name() == "Radius")
          {
            h.HouseProductionType.Radius = var.value_int();
          }
          else if (var.name() == "Rohmenge")
          {
            h.HouseProductionType.Rohmenge = var.value_int();
          }
          else if (var.name() == "Prodmenge")
          {
            h.HouseProductionType.Prodmenge = var.value_int();
          }
          else if (var.name() == "Randwachs")
          {
            h.HouseProductionType.Randwachs = var.value_int();
          }
          else if (var.name() == "Maxlager")
          {
            h.HouseProductionType.Maxlager = var.value_int();
          }
          else if (var.name() == "Maxnorohst")
          {
            h.HouseProductionType.Maxnorohst = var.value_int();
          }
          else if (var.name() == "Arbeiter")
          {
            h.HouseProductionType.Arbeiter = var.value_int();
          }
          else if (var.name() == "Figuranz")
          {
            h.HouseProductionType.Figuranz = var.value_int();
          }
          else if (var.name() == "Interval")
          {
            h.HouseProductionType.Interval = var.value_int();
          }
        }
      }
      else if (nestedObj.name() == "HAUS_BAUKOST")
      {
        for (int i = 0; i < nestedObj.variables().variable_size(); i++)
        {
          auto var = nestedObj.variables().variable(i);
          if (var.name() == "Money")
          {
            h.HouseBuildCosts.Money = var.value_int();
          }
          else if (var.name() == "Werkzeug")
          {
            h.HouseBuildCosts.Werkzeug = var.value_int();
          }
          else if (var.name() == "Holz")
          {
            h.HouseBuildCosts.Holz = var.value_int();
          }
          else if (var.name() == "Ziegel")
          {
            h.HouseBuildCosts.Ziegel = var.value_int();
          }
          else if (var.name() == "Kanon")
          {
            h.HouseBuildCosts.Kanon = var.value_int();
          }
        }
      }
    }
  }
  return h;
}
