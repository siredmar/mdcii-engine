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

#ifndef _HOST_GAD_H_
#define _HOST_GAD_H_

#include <experimental/optional>
#include <map>

#include "cod_parser.hpp"


enum class HostGadKindType
{
  UNSET = 0,
  GAD_TEXTL,
  GAD_GFX
};

struct HostGadGadget
{
  typedef struct Position
  {
    int x;
    int y;
  };

  typedef struct Size
  {
    int h;
    int w;
  };

  int Id = -1;
  // Ignore Blocknr for now
  // int Blocknr = -1;
  int Gfxnr = -1;
  HostGadKindType Kind = HostGadKindType::UNSET;
  int Noselflg = -1;
  Position Pos = {-1, -1};
  Size Size = {-1, -1};
  int Basenr = -1;
  int Reiheflg = -1;
  int Pressoff = -1;
  std::vector<int> Color = {-1, -1};
  Position Posoffs = {-1, -1};
};

class Hostgad
{
public:
  Hostgad(std::shared_ptr<Cod_Parser> cod)
    : cod(cod)
  {
    generate_gadgets();
  }

  std::experimental::optional<HostGadGadget*> get_gadget(int id)
  {
    if (gadgets.find(id) == gadgets.end())
    {
      return {};
    }
    else
    {
      return &gadgets[id];
    }
  }
  int get_gadgets_size() { return gadgets_vec.size(); }
  HostGadGadget* get_gadgets_by_index(int index) { return gadgets_vec[index]; }

private:
  void generate_gadgets()
  {
    for (int o = 0; o < cod->objects.object_size(); o++)
    {
      auto obj = cod->objects.object(o);
      if (obj.name() == "GADGET")
      {
        for (int i = 0; i < obj.objects_size(); i++)
        {
          auto gadget = generate_gadget(&obj.objects(i));
          gadgets[gadget.Id] = gadget;
          gadgets_vec.push_back(&gadgets[gadget.Id]);
        }
      }
    }
  }

  HostGadGadget generate_gadget(const cod_pb::Object* obj)
  {
    HostGadGadget h;
    if (obj->has_variables() == true)
    {
      for (int i = 0; i < obj->variables().variable_size(); i++)
      {
        if (obj->has_variables() == true)
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
          // Ignore Blocknr for now
          // else if (var.name() == "Blocknr")
          // {
          //   h.Blocknr = var.value_int();
          // }
          else if (var.name() == "Gfxnr")
          {
            h.Gfxnr = var.value_int();
          }
          else if (var.name() == "Kind")
          {
            h.Kind = KindMap[var.value_string()];
          }
          else if (var.name() == "Noselflg")
          {
            h.Noselflg = var.value_int();
          }
          else if (var.name() == "Pos")
          {
            h.Pos.x = var.value_array().value(0).value_int();
            h.Pos.y = var.value_array().value(1).value_int();
          }
          else if (var.name() == "Size")
          {
            h.Size.w = var.value_array().value(0).value_int();
            h.Size.h = var.value_array().value(1).value_int();
          }
          else if (var.name() == "Basenr")
          {
            h.Basenr = var.value_int();
          }
          else if (var.name() == "Reiheflg")
          {
            h.Reiheflg = var.value_int();
          }
          else if (var.name() == "Pressoff")
          {
            h.Pressoff = var.value_int();
          }
          else if (var.name() == "Color")
          {
            h.Color[0] = var.value_array().value(0).value_int();
            h.Color[1] = var.value_array().value(1).value_int();
          }
          else if (var.name() == "Posoffs")
          {
            h.Posoffs.x = var.value_array().value(0).value_int();
            h.Posoffs.y = var.value_array().value(1).value_int();
          }
        }
      }
      return h;
    }
  }

private:
  const int id_offset = 0;
  std::map<int, HostGadGadget> gadgets;
  std::vector<HostGadGadget*> gadgets_vec;
  std::shared_ptr<Cod_Parser> cod;

  std::map<std::string, HostGadKindType> KindMap = {{"GAD_GFX", HostGadKindType::GAD_GFX}, {"GAD_TEXTL", HostGadKindType::GAD_TEXTL}};
};

#endif //_HOST_GAD_H_