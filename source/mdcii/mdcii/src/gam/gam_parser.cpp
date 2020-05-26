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

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <iostream>

#include "files.hpp"
#include "gam/gam_parser.hpp"

GamParser::GamParser(const std::string& gam, bool peek)
{
  auto files = Files::instance();
  auto path = files->find_path_for_file(gam);
  if (path == "")
  {
    throw("cannot find file");
  }
  std::ifstream f;
  f.open(path, std::ios_base::in | std::ios_base::binary);
  while (!f.eof())
  {
    try
    {
      chunks.push_back(std::make_shared<Chunk>(f));
    }
    catch (const std::exception& e)
    {
      std::cerr << e.what() << '\n';
    }
  }
  for (unsigned int chunkIndex = 0; chunkIndex < chunks.size(); chunkIndex++)
  {
    auto chunkName = std::string(chunks[chunkIndex]->chunk.name);

    if (chunkName == "AUFTRAG" || chunkName == "AUFTRAG2")
    {
      mission2 = std::make_shared<Mission2>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
    }
    else if (chunkName == "AUFTRAG4")
    {
      mission4 = std::make_shared<Mission4>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
    }
    else if (chunkName == "SZENE")
    {
      sceneSave = std::make_shared<SceneSave>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
    }
    else if (chunkName == "SZENE_RANKING")
    {
      sceneRanking = std::make_shared<SceneRanking>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
    }
    else if (chunkName == "RANDTAB")
    {
      // more to come later
    }
    else if (chunkName == "NAME")
    {
      // more to come later
    }
    else if (chunkName == "CUSTOM")
    {
      // more to come later
    }
    else
    {
      // other chunks than parsed in peek mode
    }
    if (peek == false)
    {
      if (chunkName == "INSEL3")
      {
        auto i = std::make_shared<Island3>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
        islands3.push_back(i);
      }
      else if (chunkName == "INSEL4" || chunkName == "INSEL5")
      {
        std::cout << "chunkIndex: " << chunkIndex << std::endl;
        auto i = std::make_shared<Island5>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
        islands5.push_back(i);
      }
      else if (chunkName == "INSELHAUS")
      {
        islands5.back()->setIslandHouse(chunks[chunkIndex]);
      }
      else if (chunkName == "KONTOR2")
      {
        islands5.back()->setWarehouse2(chunks[chunkIndex]);
      }
      else if (chunkName == "PRODLIST2")
      {
        // more to come later
      }
      else if (chunkName == "ROHWACHS2")
      {
        // more to come later
      }
      else if (chunkName == "SIEDLER")
      {
        // more to come later
      }
      else if (chunkName == "WERFT")
      {
        // more to come later
      }
      else if (chunkName == "MILITAR")
      {
        // more to come later
      }
      else if (chunkName == "MARKT2")
      {
        // more to come later
      }
      else if (chunkName == "STADT3" || chunkName == "STADT4")
      {
        // more to come later
      }
      else if (chunkName == "HIRSCH2")
      {
        // more to come later
      }
      else if (chunkName == "SHIP4")
      {
        // more to come later
      }
      else if (chunkName == "HANDLER")
      {
        // more to come later
      }
      else if (chunkName == "SOLDAT2" || chunkName == "SOLDAT3")
      {
        // more to come later
      }
      else if (chunkName == "SOLDATINSEL")
      {
        // more to come later
      }
      else if (chunkName == "TURM")
      {
        // more to come later
      }
      else if (chunkName == "TIMERS")
      {
        // more to come later
      }
      else if (chunkName == "PLAYER2" || chunkName == "PLAYER3" || chunkName == "PLAYER4")
      {
        // more to come later
      }
      else
      {
        // INSELHAUS is handeled by INSEL4 and INSEL5
        // others are unknown chunks
      }
    }
  }

  std::cout << "overall chunks num: " << chunks.size() << std::endl;
  if (peek == false)
  {
    std::cout << "islands5: " << islands5.size() << std::endl;
    std::cout << "islands3: " << islands3.size() << std::endl;
  }
  if (mission2)
  {
    std::cout << "mission2: " << mission2->missions.size() << std::endl;
  }
  if (mission4)
  {
    std::cout << "mission4: " << mission4->missions.size() << std::endl;
  }
  if (sceneRanking)
  {
    std::cout << "scene ranking: " << sceneRanking->sceneRanking.ranking << std::endl;
  }
  if (sceneSave)
  {
    std::cout << "scene number of islands: " << sceneSave->sceneSave.islandsCount << std::endl;
  }
  f.close();
}

int GamParser::getSceneRanking()
{
  if (sceneRanking)
  {
    return sceneRanking->sceneRanking.ranking;
  }
  else
  {
    return -1;
  }
}