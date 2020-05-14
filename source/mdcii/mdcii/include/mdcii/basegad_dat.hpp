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

#ifndef _BASEGAD_DAT_H_
#define _BASEGAD_DAT_H_

#include <experimental/optional>
#include <map>

#include "cod_parser.hpp"


enum class BaseGadKindType
{
  UNSET = 0,
  GAD_GFX
};

struct BaseGadGadget
{
  int Id = -1;
  int Blocknr = -1;
  int Gfxnr = -1;
  BaseGadKindType Kind = BaseGadKindType::UNSET;
  int Noselflg = -1;
  int Pressoff = -1;
  struct
  {
    int x;
    int y;
  } Pos;

  struct
  {
    int h;
    int w;
  } Size;
};

class Basegad
{
public:
  explicit Basegad(std::shared_ptr<Cod_Parser> cod)
    : cod(cod)
  {
    generate_gadgets();
  }

  std::experimental::optional<BaseGadGadget*> get_gadget(int id)
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
  int get_gadgets_size()
  {
    return gadgets_vec.size();
  }
  BaseGadGadget* get_gadgets_by_index(int index)
  {
    return gadgets_vec[index];
  }

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

  BaseGadGadget generate_gadget(const cod_pb::Object* obj)
  {
    BaseGadGadget h;
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
          else if (var.name() == "Blocknr")
          {
            h.Blocknr = var.value_int();
          }
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
          else if (var.name() == "Pressoff")
          {
            h.Pressoff = var.value_int();
          }
          else if (var.name() == "Blocknr")
          {
            h.Blocknr = var.value_int();
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
        }
      }
    }
    return h;
  }

private:
  const int id_offset = 30000;
  std::map<int, BaseGadGadget> gadgets;
  std::vector<BaseGadGadget*> gadgets_vec;
  std::shared_ptr<Cod_Parser> cod;

  std::map<std::string, BaseGadKindType> KindMap = {{"GAD_GFX", BaseGadKindType::GAD_GFX}};
};

#endif //_BASEGAD_DAT_H_