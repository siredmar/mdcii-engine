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

#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <boost/random.hpp>

#include <iostream>

#include "gam/gam_parser.hpp"

GamParser::GamParser(const std::string& gam, bool peek)
  : files(Files::Instance())
{
  auto path = files->FindPathForFile(gam);
  if (path == "")
  {
    throw("[EER] cannot find file: " + gam);
  }

  chunks = Chunk::ReadChunks(path);
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
        auto i5 = std::make_shared<Island5>(*i);
        islands5.push_back(i5);
      }
      else if (chunkName == "INSEL4")
      {
        auto i = std::make_shared<Island4>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
        auto i5 = std::make_shared<Island5>(*i);
        islands5.push_back(i5);
      }
      else if (chunkName == "INSEL5")
      {
        auto i = std::make_shared<Island5>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
        islands5.push_back(i);
      }
      else if (chunkName == "INSELHAUS")
      {
        islands5.back()->AddIslandHouse(chunks[chunkIndex]);
      }
      else if (chunkName == "HIRSCH2")
      {
        islands5.back()->SetDeer(chunks[chunkIndex]);
      }
      else if (chunkName == "KONTOR2")
      {
        warehouse = std::make_shared<Warehouse2>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
      }
      else if (chunkName == "PRODLIST2")
      {
        productionList = std::make_shared<ProductionList>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
      }
      else if (chunkName == "WERFT")
      {
        shipyard = std::make_shared<Shipyard>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
      }
      else if (chunkName == "MILITAR")
      {
        military = std::make_shared<Military>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
      }
      else if (chunkName == "SIEDLER")
      {
        settlers = std::make_shared<Settlers>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
      }
      else if (chunkName == "MARKT2")
      {
        marketPlace = std::make_shared<MarketPlace>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
      }
      else if (chunkName == "ROHWACHS2")
      {
        rawGrowth = std::make_shared<RawGrowth>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
      }
      else if (chunkName == "STADT3" || chunkName == "STADT4")
      {
        city = std::make_shared<City4>(chunks[chunkIndex]->chunk.data, chunks[chunkIndex]->chunk.length, chunkName);
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
        // unknown chunk
      }
    }
  }

  if (peek == false)
  {
    // generate random islands from scene
    if (sceneSave)
    {
      for (int i = 0; i < sceneSave->sceneSave.islandsCount; i++)
      {
        RandomIsland island = sceneSave->sceneSave.islands[i];
        std::shared_ptr<Island5> i5;
        if (island.fileNumber != SceneSave::islandRandom)
        {
          i5 = std::make_shared<Island5>(SceneIslandbyFile(island.size, island.climate, island.fileNumber));
        }
        else
        {
          i5 = std::make_shared<Island5>(SceneRandomIsland(island.size, island.climate));
        }
        i5->SetIslandNumber(island.islandNumber);
        // Todo: make random raw growth rates, ores, ...
        islands5.push_back(i5);
      }
    }

    if (islands5.size())
    {
      for (auto i : islands5)
      {
        i->Finalize();
      }
    }
  }

  std::cout << "overall chunks num: " << chunks.size() << std::endl;
  if (peek == false)
  {
    std::cout << "islands5: " << islands5.size() << std::endl;
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
  if (productionList)
  {
    std::cout << "prodlist size: " << productionList->productionList.size() << std::endl;
  }
  if (shipyard)
  {
    std::cout << "shipyards: " << shipyard->shipyard.size() << std::endl;
  }
  if (military)
  {
    std::cout << "military buildings: " << military->military.size() << std::endl;
  }
  if (warehouse)
  {
    std::cout << "warehouses: " << warehouse->warehouses.size() << std::endl;
  }
  if (settlers)
  {
    std::cout << "settlers: " << settlers->settlers.size() << std::endl;
  }
  if (marketPlace)
  {
    std::cout << "marketplace: " << marketPlace->marketPlace.size() << std::endl;
  }
  if (rawGrowth)
  {
    std::cout << "rawGrowth: " << rawGrowth->rawGrowth.size() << std::endl;
  }
  if (city)
  {
    std::cout << "city: " << city->city.size() << std::endl;
  }
}

Island5 GamParser::SceneRandomIsland(SizeType size)
{
  return SceneRandomIsland(size, ClimateType::Any);
}

Island5 GamParser::SceneRandomIsland(SizeType size, ClimateType climate)
{
  if (climate == ClimateType::Any)
  {
    climate = static_cast<ClimateType>(Island5::RandomIslandClimate());
  }
  auto islands = files->GrepFiles(islandClimateMap[static_cast<IslandClimate>(climate)] + "/" + islandSizeMap[static_cast<IslandSize>(size)]);
  std::time_t now = std::time(0);
  boost::random::mt19937 gen{static_cast<std::uint32_t>(now)};
  int randomIndex = gen() % (islands.size() - 1);
  return SceneIslandbyFile(size, climate, randomIndex);
}

Island5 GamParser::SceneIslandbyFile(SizeType size, ClimateType climate, uint16_t fileNumber)
{
  auto islandFile = Island5::IslandFileName(static_cast<IslandSize>(size), fileNumber, static_cast<IslandClimate>(climate));
  auto path = files->FindPathForFile(islandFile);
  if (path.empty())
  {
    throw("[EER] cannot find island file: " + islandFile);
  }
  std::vector<std::shared_ptr<Chunk>> chunks = Chunk::ReadChunks(path);
  Island5 i(chunks[0]->chunk.data, chunks[0]->chunk.length, chunks[0]->chunk.name.c_str());
  auto islandHouse = std::make_shared<IslandHouse>(
      chunks[1]->chunk.data, chunks[1]->chunk.length, chunks[1]->chunk.name.c_str(), i.GetIslandData().width, i.GetIslandData().height);
  i.SetIslandFile(fileNumber);
  i.AddIslandHouse(islandHouse);
  return i;
}

int GamParser::GetSceneRanking()
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