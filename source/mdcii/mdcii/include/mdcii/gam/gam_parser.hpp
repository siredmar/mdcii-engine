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

#ifndef _GAM_PARSER_HPP
#define _GAM_PARSER_HPP

#include <memory>
#include <string>

#include "chunk.hpp"
#include "island.hpp"
#include "military.hpp"
#include "missions.hpp"
#include "productionlist.hpp"
#include "scene.hpp"
#include "shipyard.hpp"
#include "warehouse.hpp"

class GamParser
{
public:
  explicit GamParser(const std::string& gam, bool peek);
  int getSceneRanking();

private:
  std::vector<std::shared_ptr<Chunk>> chunks;
  std::vector<std::shared_ptr<Island5>> islands5; // INSEL5
  std::vector<std::shared_ptr<Island3>> islands3; // INSEL3
  std::shared_ptr<Mission2> mission2;             // AUFTRAG2
  std::shared_ptr<Mission4> mission4;             // AUFTRAG4
  std::shared_ptr<SceneRanking> sceneRanking;     // SZENE_RANKING
  std::shared_ptr<SceneSave> sceneSave;           // SZENE2
  std::shared_ptr<Shipyard> shipyard;             // WERFT
  std::shared_ptr<Military> military;             // MILITAR
};

#endif // _GAM_PARSER_HPP