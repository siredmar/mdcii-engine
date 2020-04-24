#ifndef _GAM_PARSER_HPP
#define _GAM_PARSER_HPP

#include <memory>
#include <string>

#include "chunk.hpp"
#include "files.hpp"
#include "island.hpp"
#include "mission.hpp"

class GamParser
{
public:
  GamParser(std::string gam);

private:
  std::vector<std::shared_ptr<Chunk>> chunks;
  std::vector<std::shared_ptr<Island5>> islands;
  std::vector<std::shared_ptr<Mission>> missions;
};

#endif // _GAM_PARSER_HPP