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
  explicit Basegad(std::shared_ptr<CodParser> cod);
  std::experimental::optional<BaseGadGadget*> GetGadget(int id);
  int GetGadgetsSize();
  BaseGadGadget* GetGadgetsByIndex(int index);

private:
  void GenerateGadgets();
  BaseGadGadget GenerateGadget(const cod_pb::Object* obj);

private:
  const int idOffset = 30000;
  std::map<int, BaseGadGadget> gadgets;
  std::vector<BaseGadGadget*> gadgetsVector;
  std::shared_ptr<CodParser> cod;

  std::map<std::string, BaseGadKindType> kindMap = {{"GAD_GFX", BaseGadKindType::GAD_GFX}};
};

#endif //_BASEGAD_DAT_H_