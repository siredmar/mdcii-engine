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
    if (std::string(c->chunk.name) == "INSEL5")
    {
      auto i = std::make_shared<Island5>(c->chunk.data, c->chunk.length);
      islands.push_back(i);
    }
  }
  std::cout << "chunks: " << chunks.size() << std::endl;
  std::cout << "islands: " << islands.size() << std::endl;
}
