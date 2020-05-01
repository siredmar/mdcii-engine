#ifndef _GAM_PARSER_HPP
#define _GAM_PARSER_HPP

#include <memory>
#include <string>

#include "chunk.hpp"
#include "files.hpp"
#include "island.hpp"
#include "kontor.hpp"
#include "missions.hpp"

class GamParser
{
public:
  GamParser(std::string gam);

private:
  std::vector<std::shared_ptr<Chunk>> chunks;
  std::vector<std::shared_ptr<Island5>> islands5;
  std::vector<std::shared_ptr<Island3>> islands3;
  std::shared_ptr<Mission2> mission2;
  std::shared_ptr<Mission4> mission4;
  std::shared_ptr<Kontor2> kontor2;
};

#endif // _GAM_PARSER_HPP