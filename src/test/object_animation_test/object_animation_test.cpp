#define CATCH_CONFIG_MAIN

#include "../catch2/catch.hpp"

#include "../../cod_parser.hpp"
#include "../../files.hpp"
#include "../../haeuser.hpp"
#include "../../object_animation.hpp"

std::string file = "Objekt: HAUS\r\n"
                   "  Nummer:     0\r\n"
                   "  Id:         21352\r\n"
                   "  Gfx:        1491\r\n"
                   "Size:       1, 1\r\n"
                   "  Rotate:     0\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
                   "  AnimAnz:    0\r\n"
                   "\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         21011\r\n"
                   "  Gfx:        1092\r\n"
                   "Size:       1, 1\r\n"
                   "  Rotate:     1\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
                   "  AnimAnz:    0\r\n"
                   "\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         22311\r\n"
                   "  Gfx:        552\r\n"
                   "Size:       1, 1\r\n"
                   "  Rotate:     4\r\n"
                   "  AnimTime:   130\r\n"
                   "  AnimAdd:    4\r\n"
                   "  AnimAnz:    6\r\n"
                   "\r\n"
                   "EndObj;\r\n";


TEST_CASE("Animation only one view for each direction")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(1352, haeuser);
  auto tiles = anim.get_tiles(0);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);

  tiles = anim.get_tiles(1);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);

  tiles = anim.get_tiles(2);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);

  tiles = anim.get_tiles(3);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);
}
