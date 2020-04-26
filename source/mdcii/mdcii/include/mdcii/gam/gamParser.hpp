#ifndef _GAM_PARSER_HPP
#define _GAM_PARSER_HPP

#include <memory>
#include <string>

#include "chunk.hpp"
#include "files.hpp"
#include "island.hpp"
#include "missions.hpp"

class GamParser
{
public:
  GamParser(std::string gam);

private:
  std::vector<std::shared_ptr<Chunk>> chunks;
  std::vector<std::shared_ptr<Island5>> islands;
  std::shared_ptr<Mission2> mission2;
  std::shared_ptr<Mission4> mission4;
};

#endif // _GAM_PARSER_HPP