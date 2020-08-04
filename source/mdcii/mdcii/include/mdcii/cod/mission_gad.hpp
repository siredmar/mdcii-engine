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

#ifndef _MISSION_GAD_H_
#define _MISSION_GAD_H_

#include <experimental/optional>
#include <map>

#include "cod_parser.hpp"

enum class MissionGadKindType
{
    UNSET = 0,
    GAD_TEXTL,
    GAD_GFX
};

struct MissionGadGadget
{
    int Id = -1;
    // Ignore Blocknr for now
    // int Blocknr = -1;
    int Gfxnr = -1;
    MissionGadKindType Kind = MissionGadKindType::UNSET;
    int Noselflg = -1;
    int Basenr = -1;
    int Reiheflg = -1;
    int Pressoff = -1;
    std::vector<int> Color = { -1, -1 };
    struct
    {
        int x;
        int y;
    } Pos;
    struct
    {
        int x;
        int y;
    } Posoffs;
    struct
    {
        int h;
        int w;
    } Size;
};

class Missiongad
{
public:
    explicit Missiongad(std::shared_ptr<CodParser> cod);
    std::experimental::optional<MissionGadGadget*> GetGadget(int id);
    int GetGadgetsSize();
    MissionGadGadget* GetGadgetByIndex(int index);

private:
    void GenerateGadgets();
    MissionGadGadget GenerateGadget(const cod_pb::Object* obj);

private:
    const int idOffset = 0;
    std::map<int, MissionGadGadget> gadgets;
    std::vector<MissionGadGadget*> gadgetsVector;
    std::shared_ptr<CodParser> cod;

    std::map<std::string, MissionGadKindType> kindMap = { { "GAD_GFX", MissionGadKindType::GAD_GFX }, { "GAD_TEXTL", MissionGadKindType::GAD_TEXTL } };
};

#endif //_HOST_GAD_H_