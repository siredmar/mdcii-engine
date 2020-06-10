
// This file is part of the MDCII Game Engine.
// Copyright (C) 2019  Armin Schlegel
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

#include "cod/haeuser.hpp"

Haeuser::Haeuser(std::shared_ptr<Cod_Parser> cod)
  : cod(cod)
{
  generate_haeuser();
}

std::experimental::optional<Haus*> Haeuser::get_haus(int id)
{
  if (haeuser.find(id) == haeuser.end())
  {
    return {};
  }
  else
  {
    return &haeuser[id];
  }
}

int Haeuser::get_haeuser_size()
{
  return haeuser_vec.size();
}

Haus* Haeuser::get_haeuser_by_index(int index)
{
  return haeuser_vec[index];
}

void Haeuser::generate_haeuser()
{
  for (int o = 0; o < cod->objects.object_size(); o++)
  {
    auto obj = cod->objects.object(o);
    if (obj.name() == "HAUS")
    {
      for (int i = 0; i < obj.objects_size(); i++)
      {
        auto haus = generate_haus(&obj.objects(i));
        haeuser[haus.Id] = haus;
        haeuser_vec.push_back(&haeuser[haus.Id]);
      }
    }
  }
}

Haus Haeuser::generate_haus(const cod_pb::Object* obj)
{
  Haus h;
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
          h.Id = var.value_int() - id_offset;
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
      auto nested_obj = obj->objects(o);
      if (nested_obj.name() == "HAUS_PRODTYP")
      {
        for (int i = 0; i < nested_obj.variables().variable_size(); i++)
        {
          auto var = nested_obj.variables().variable(i);
          if (var.name() == "Kind")
          {
            h.HAUS_PRODTYP.Kind = ProdtypKindMap[var.value_string()];
          }
          else if (var.name() == "Ware")
          {
            h.HAUS_PRODTYP.Ware = WareMap[var.value_string()];
          }
          else if (var.name() == "Workstoff")
          {
            h.HAUS_PRODTYP.Workstoff = WorkstoffMap[var.value_string()];
          }
          else if (var.name() == "Erzbergnr")
          {
            h.HAUS_PRODTYP.Erzbergnr = ErzbergnrMap[var.value_string()];
          }
          else if (var.name() == "Rohstoff")
          {
            h.HAUS_PRODTYP.Rohstoff = RohstoffMap[var.value_string()];
          }
          else if (var.name() == "MAXPRODCNT")
          {
            h.HAUS_PRODTYP.Maxprodcnt = MaxprodcntMap[var.value_string()];
          }
          else if (var.name() == "Bauinfra")
          {
            h.HAUS_PRODTYP.Bauinfra = BauinfraMap[var.value_string()];
          }
          else if (var.name() == "Figurnr")
          {
            h.HAUS_PRODTYP.Figurnr = FigurnrMap[var.value_string()];
          }
          else if (var.name() == "Rauchfignr")
          {
            h.HAUS_PRODTYP.Rauchfignr = RauchfignrMap[var.value_string()];
          }
          else if (var.name() == "Maxware")
          {
            for (int v = 0; v < var.value_array().value_size(); v++)
            {
              h.HAUS_PRODTYP.Maxware.push_back(var.value_array().value(v).value_int());
            }
          }
          else if (var.name() == "Kosten")
          {
            for (int v = 0; v < var.value_array().value_size(); v++)
            {
              h.HAUS_PRODTYP.Kosten.push_back(var.value_array().value(v).value_int());
            }
          }
          else if (var.name() == "BGruppe")
          {
            h.HAUS_PRODTYP.BGruppe = var.value_int();
          }
          else if (var.name() == "LagAniFlg")
          {
            h.HAUS_PRODTYP.LagAniFlg = var.value_int();
          }
          else if (var.name() == "NoMoreWork")
          {
            h.HAUS_PRODTYP.NoMoreWork = var.value_int();
          }
          else if (var.name() == "Workmenge")
          {
            h.HAUS_PRODTYP.Workmenge = var.value_int();
          }
          else if (var.name() == "Doerrflg")
          {
            h.HAUS_PRODTYP.Doerrflg = var.value_int();
          }
          else if (var.name() == "Anicontflg")
          {
            h.HAUS_PRODTYP.Anicontflg = var.value_int();
          }
          else if (var.name() == "MakLagFlg")
          {
            h.HAUS_PRODTYP.MakLagFlg = var.value_int();
          }
          else if (var.name() == "Nativflg")
          {
            h.HAUS_PRODTYP.Nativflg = var.value_int();
          }
          else if (var.name() == "NoLagVoll")
          {
            h.HAUS_PRODTYP.NoLagVoll = var.value_int();
          }
          else if (var.name() == "Radius")
          {
            h.HAUS_PRODTYP.Radius = var.value_int();
          }
          else if (var.name() == "Rohmenge")
          {
            h.HAUS_PRODTYP.Rohmenge = var.value_int();
          }
          else if (var.name() == "Prodmenge")
          {
            h.HAUS_PRODTYP.Prodmenge = var.value_int();
          }
          else if (var.name() == "Randwachs")
          {
            h.HAUS_PRODTYP.Randwachs = var.value_int();
          }
          else if (var.name() == "Maxlager")
          {
            h.HAUS_PRODTYP.Maxlager = var.value_int();
          }
          else if (var.name() == "Maxnorohst")
          {
            h.HAUS_PRODTYP.Maxnorohst = var.value_int();
          }
          else if (var.name() == "Arbeiter")
          {
            h.HAUS_PRODTYP.Arbeiter = var.value_int();
          }
          else if (var.name() == "Figuranz")
          {
            h.HAUS_PRODTYP.Figuranz = var.value_int();
          }
          else if (var.name() == "Interval")
          {
            h.HAUS_PRODTYP.Interval = var.value_int();
          }
        }
      }
      else if (nested_obj.name() == "HAUS_BAUKOST")
      {
        for (int i = 0; i < nested_obj.variables().variable_size(); i++)
        {
          auto var = nested_obj.variables().variable(i);
          if (var.name() == "Money")
          {
            h.HAUS_BAUKOST.Money = var.value_int();
          }
          else if (var.name() == "Werkzeug")
          {
            h.HAUS_BAUKOST.Werkzeug = var.value_int();
          }
          else if (var.name() == "Holz")
          {
            h.HAUS_BAUKOST.Holz = var.value_int();
          }
          else if (var.name() == "Ziegel")
          {
            h.HAUS_BAUKOST.Ziegel = var.value_int();
          }
          else if (var.name() == "Kanon")
          {
            h.HAUS_BAUKOST.Kanon = var.value_int();
          }
        }
      }
    }
  }
  return h;
}
