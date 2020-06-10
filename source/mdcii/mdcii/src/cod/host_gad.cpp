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

#include "cod/host_gad.hpp"


Hostgad::Hostgad(std::shared_ptr<Cod_Parser> cod)
  : cod(cod)
{
  generate_gadgets();
}

std::experimental::optional<HostGadGadget*> Hostgad::get_gadget(int id)
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
int Hostgad::get_gadgets_size()
{
  return gadgets_vec.size();
}
HostGadGadget* Hostgad::get_gadgets_by_index(int index)
{
  return gadgets_vec[index];
}

void Hostgad::generate_gadgets()
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

HostGadGadget Hostgad::generate_gadget(const cod_pb::Object* obj)
{
  HostGadGadget h;
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