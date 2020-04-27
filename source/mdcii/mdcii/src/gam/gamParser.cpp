#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "gam/gamParser.hpp"

GamParser::GamParser(std::string gam)
{
  auto files = Files::instance();
  auto path = files->find_path_for_file(gam);
  if (path == "")
  {
    throw("canno find file");
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
    if (chunkName == "INSEL3" || chunkName == "INSEL4" || chunkName == "INSEL5")
    {
      auto i = std::make_shared<Island5>(c->chunk.data, c->chunk.length, chunkName);
      islands.push_back(i);
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
    else if (chunkName == "AUFTRAG" || chunkName == "AUFTRAG2")
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
  }


  std::cout << "chunks: " << chunks.size() << std::endl;
  std::cout << "islands: " << islands.size() << std::endl;
  if (mission2)
  {
    std::cout << "mission2: " << mission2->missions.size() << std::endl;
  }
  if (mission4)
  {
    std::cout << "mission4: " << mission4->missions.size() << std::endl;
  }
  // for (auto m : missions->missions)
  // {
  //   std::cout << std::string(m->infotxt[0]) << std::endl;
  // }
  f.close();
}
