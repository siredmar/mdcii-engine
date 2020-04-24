#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "gam/gamParser.hpp"

GamParser::GamParser(std::string gam)
{
  auto files = Files::instance();
  auto path = files->find_path_for_file(gam);
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
    else if (chunkName == "AUFTRAG" || chunkName == "AUFTRAG2" || chunkName == "AUFTRAG4")
    {
      auto i = std::make_shared<Mission>(c->chunk.data, c->chunk.length, chunkName);
      missions.push_back(i);
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
  std::cout << "missions: " << missions.size() << std::endl;
  for (auto m : missions)
  {
    std::cout << std::string(m->mission.infotxt) << std::endl;
  }
  f.close();
}
