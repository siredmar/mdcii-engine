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

#include "cod/basegad_dat.hpp"

Basegad::Basegad(std::shared_ptr<CodParser> cod)
    : cod(cod)
{
    GenerateGadgets();
}

std::experimental::optional<BaseGadGadget*> Basegad::GetGadget(int id)
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
int Basegad::GetGadgetsSize()
{
    return gadgetsVector.size();
}
BaseGadGadget* Basegad::GetGadgetsByIndex(int index)
{
    return gadgetsVector[index];
}

void Basegad::GenerateGadgets()
{
    for (int o = 0; o < cod->objects.object_size(); o++)
    {
        auto obj = cod->objects.object(o);
        if (obj.name() == "GADGET")
        {
            for (int i = 0; i < obj.objects_size(); i++)
            {
                auto gadget = GenerateGadget(&obj.objects(i));
                gadgets[gadget.Id] = gadget;
                gadgetsVector.push_back(&gadgets[gadget.Id]);
            }
        }
    }
}

BaseGadGadget Basegad::GenerateGadget(const cod_pb::Object* obj)
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
                        h.Id = var.value_int() - idOffset;
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
                    h.Kind = kindMap[var.value_string()];
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