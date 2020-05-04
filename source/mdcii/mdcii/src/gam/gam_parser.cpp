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
    chunks.push_back(std::make_shared<Chunk>(f));
  }
  for (auto& c : chunks)
  {
    auto chunkName = std::string(c->chunk.name);

    if (chunkName == "AUFTRAG" || chunkName == "AUFTRAG2")
    {
      mission2 = std::make_shared<Mission2>(c->chunk.data, c->chunk.length, chunkName);
    }
    else if (chunkName == "AUFTRAG4")
    {
      mission4 = std::make_shared<Mission4>(c->chunk.data, c->chunk.length, chunkName);
    }
    else if (chunkName == "SZENE")
    {
    }
    else if (chunkName == "SZENE_RANKING")
    {
      sceneRanking = std::make_shared<SceneRanking>(c->chunk.data, c->chunk.length, chunkName);
    }
    else if (chunkName == "RANDTAB")
    {
    }
    else if (chunkName == "NAME")
    {
    }
    else if (chunkName == "CUSTOM")
    {
    }
    else
    {
      //
    }
    if (peek == false)
    {
      if (chunkName == "INSEL3")
      {
        auto i = std::make_shared<Island3>(c->chunk.data, c->chunk.length, chunkName);
        islands3.push_back(i);
      }
      else if (chunkName == "INSEL4" || chunkName == "INSEL5")
      {
        auto i = std::make_shared<Island5>(c->chunk.data, c->chunk.length, chunkName);
        islands5.push_back(i);
      }
      else if (chunkName == "INSELHAUS")
      {
      }
      else if (chunkName == "PRODLIST2")
      {
      }
      else if (chunkName == "ROHWACHS2")
      {
      }
      else if (chunkName == "SIEDLER")
      {
      }
      else if (chunkName == "WERFT")
      {
      }
      else if (chunkName == "MILITAR")
      {
      }
      else if (chunkName == "KONTOR2")
      {
        kontor2 = std::make_shared<Kontor2>(c->chunk.data, c->chunk.length, chunkName);
      }
      else if (chunkName == "MARKT2")
      {
      }
      else if (chunkName == "STADT3" || chunkName == "STADT4")
      {
      }
      else if (chunkName == "HIRSCH2")
      {
      }
      else if (chunkName == "SHIP4")
      {
      }
      else if (chunkName == "HANDLER")
      {
      }
      else if (chunkName == "SOLDAT2" || chunkName == "SOLDAT3")
      {
      }
      else if (chunkName == "SOLDATINSEL")
      {
      }
      else if (chunkName == "TURM")
      {
      }
      else if (chunkName == "TIMERS")
      {
      }
      else if (chunkName == "PLAYER2" || chunkName == "PLAYER2" || chunkName == "PLAYER4")
      {
      }
      else
      {
        //
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