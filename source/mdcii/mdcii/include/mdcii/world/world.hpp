#ifndef __WORLD_HPP_
#define __WORLD_HPP_

#include "gam/gam_parser.hpp"
#include "gam/island.hpp"
#include "gam/islandhouse.hpp"

class World
{
public:
  explicit World(GamParser gamParser);
  std::experimental::optional<std::shared_ptr<Island5>> IslandOnPosition(uint16_t x, uint16_t y);
  bool IslandNumberOnPosition(uint8_t number, uint16_t x, uint16_t y);
  IslandHouseData TileOnPosition(uint16_t x, uint16_t y);
  enum
  {
    Width = 500,
    Height = 350
  };

private:
  GamParser gamParser;
};


#endif // __WORLD_HPP_