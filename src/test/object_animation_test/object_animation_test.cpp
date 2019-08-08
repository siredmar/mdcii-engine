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
                   "  Size:       1, 1\r\n"
                   "  Rotate:     0\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
                   "  AnimAnz:    0\r\n"
                   "\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         21011\r\n"
                   "  Gfx:        1092\r\n"
                   "  Size:       1, 1\r\n"
                   "  Rotate:     1\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
                   "  AnimAnz:    0\r\n"
                   "\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         22311\r\n"
                   "  Gfx:        552\r\n"
                   "  Size:       1, 1\r\n"
                   "  Rotate:     1\r\n"
                   "  AnimTime:   130\r\n"
                   "  AnimAdd:    4\r\n"
                   "  AnimAnz:    6\r\n"
                   "\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         20501\r\n"
                   "  Gfx:        1816\r\n"
                   "  Size:       2, 2\r\n"
                   "  Rotate:     4\r\n"
                   "  AnimTime:   90\r\n"
                   "  AnimAdd:    16\r\n"
                   "  AnimAnz:    16\r\n"
                   "\r\n"
                   "EndObj;\r\n";


TEST_CASE("Animation - only one tile for each direction, 1x1")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(1352, haeuser);
  auto tiles = anim.get_animation_step(0);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);

  tiles = anim.get_animation_step(1);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);

  tiles = anim.get_animation_step(2);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);

  tiles = anim.get_animation_step(3);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1491);
}

TEST_CASE("Animation - unique tile for each direction, 1x1")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(1011, haeuser);
  auto tiles = anim.get_animation_step(0, 0);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1092);

  tiles = anim.get_animation_step(1, 0);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1093);

  tiles = anim.get_animation_step(2, 0);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1094);

  tiles = anim.get_animation_step(3, 0);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0].gfx == 1095);
}


TEST_CASE("Animation - multiple animation steps for for each direction, 1x1")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(2311, haeuser);
  int animAnz = haeuser->get_haus(2311).value()->AnimAnz;

  // Get tiles for rotation 0, animation step 0
  auto tiles_anim = anim.get_animation(0);
  REQUIRE(tiles_anim.size() == animAnz);
  REQUIRE(tiles_anim[0][0].gfx == 552);
  REQUIRE(tiles_anim[1][0].gfx == 556);
  REQUIRE(tiles_anim[2][0].gfx == 560);
  REQUIRE(tiles_anim[3][0].gfx == 564);
  REQUIRE(tiles_anim[4][0].gfx == 568);
  REQUIRE(tiles_anim[5][0].gfx == 572);

  // Get tiles for rotation 1, animation step 0
  tiles_anim = anim.get_animation(1);
  REQUIRE(tiles_anim.size() == animAnz);
  REQUIRE(tiles_anim[0][0].gfx == 553);
  REQUIRE(tiles_anim[1][0].gfx == 557);
  REQUIRE(tiles_anim[2][0].gfx == 561);
  REQUIRE(tiles_anim[3][0].gfx == 565);
  REQUIRE(tiles_anim[4][0].gfx == 569);
  REQUIRE(tiles_anim[5][0].gfx == 573);

  // Get tiles_anim for rotation 2, animation step 0
  tiles_anim = anim.get_animation(2);
  REQUIRE(tiles_anim.size() == animAnz);
  REQUIRE(tiles_anim[0][0].gfx == 554);
  REQUIRE(tiles_anim[1][0].gfx == 558);
  REQUIRE(tiles_anim[2][0].gfx == 562);
  REQUIRE(tiles_anim[3][0].gfx == 566);
  REQUIRE(tiles_anim[4][0].gfx == 570);
  REQUIRE(tiles_anim[5][0].gfx == 574);

  // Get tiles_anim for rotation 3, animation step 0
  tiles_anim = anim.get_animation(3);
  REQUIRE(tiles_anim.size() == animAnz);
  REQUIRE(tiles_anim[0][0].gfx == 555);
  REQUIRE(tiles_anim[1][0].gfx == 559);
  REQUIRE(tiles_anim[2][0].gfx == 563);
  REQUIRE(tiles_anim[3][0].gfx == 567);
  REQUIRE(tiles_anim[4][0].gfx == 571);
  REQUIRE(tiles_anim[5][0].gfx == 575);
}

TEST_CASE("Animation - single tile of animation steps for for each direction, 1x1")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(2311, haeuser);
  // Get tiles for rotation 0, animation step 0 to 5
  auto tile = anim.get_animation_tile(0, 0, 0);
  REQUIRE(tile.gfx == 552);
  tile = anim.get_animation_tile(0, 1, 0);
  REQUIRE(tile.gfx == 556);
  tile = anim.get_animation_tile(0, 2, 0);
  REQUIRE(tile.gfx == 560);
  tile = anim.get_animation_tile(0, 3, 0);
  REQUIRE(tile.gfx == 564);
  tile = anim.get_animation_tile(0, 4, 0);
  REQUIRE(tile.gfx == 568);
  tile = anim.get_animation_tile(0, 5, 0);
  REQUIRE(tile.gfx == 572);

  // Get tiles for rotation 1, animation step 0 to 5
  tile = anim.get_animation_tile(1, 0, 0);
  REQUIRE(tile.gfx == 553);
  tile = anim.get_animation_tile(1, 1, 0);
  REQUIRE(tile.gfx == 557);
  tile = anim.get_animation_tile(1, 2, 0);
  REQUIRE(tile.gfx == 561);
  tile = anim.get_animation_tile(1, 3, 0);
  REQUIRE(tile.gfx == 565);
  tile = anim.get_animation_tile(1, 4, 0);
  REQUIRE(tile.gfx == 569);
  tile = anim.get_animation_tile(1, 5, 0);
  REQUIRE(tile.gfx == 573);
}

TEST_CASE("Animation - get animation steps for different views, 2x2")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(501, haeuser);
  auto tiles = anim.get_animation_step(0, 0);
  REQUIRE(tiles.size() == haeuser->get_haus(501).value()->Rotate);
  REQUIRE(tiles[0].gfx == 1816);
  REQUIRE(tiles[1].gfx == 1817);
  REQUIRE(tiles[2].gfx == 1818);
  REQUIRE(tiles[3].gfx == 1819);

  tiles = anim.get_animation_step(0, 1);
  REQUIRE(tiles[0].gfx == 1832);
  REQUIRE(tiles[1].gfx == 1833);
  REQUIRE(tiles[2].gfx == 1834);
  REQUIRE(tiles[3].gfx == 1835);

  tiles = anim.get_animation_step(0, 2);
  REQUIRE(tiles[0].gfx == 1848);
  REQUIRE(tiles[1].gfx == 1849);
  REQUIRE(tiles[2].gfx == 1850);
  REQUIRE(tiles[3].gfx == 1851);

  tiles = anim.get_animation_step(0, 3);
  REQUIRE(tiles[0].gfx == 1864);
  REQUIRE(tiles[1].gfx == 1865);
  REQUIRE(tiles[2].gfx == 1866);
  REQUIRE(tiles[3].gfx == 1867);

  tiles = anim.get_animation_step(1, 0);
  REQUIRE(tiles[0].gfx == 1820);
  REQUIRE(tiles[1].gfx == 1821);
  REQUIRE(tiles[2].gfx == 1822);
  REQUIRE(tiles[3].gfx == 1823);

  tiles = anim.get_animation_step(1, 1);
  REQUIRE(tiles[0].gfx == 1836);
  REQUIRE(tiles[1].gfx == 1837);
  REQUIRE(tiles[2].gfx == 1838);
  REQUIRE(tiles[3].gfx == 1839);

  tiles = anim.get_animation_step(1, 2);
  REQUIRE(tiles[0].gfx == 1852);
  REQUIRE(tiles[1].gfx == 1853);
  REQUIRE(tiles[2].gfx == 1854);
  REQUIRE(tiles[3].gfx == 1855);

  tiles = anim.get_animation_step(1, 3);
  REQUIRE(tiles[0].gfx == 1868);
  REQUIRE(tiles[1].gfx == 1869);
  REQUIRE(tiles[2].gfx == 1870);
  REQUIRE(tiles[3].gfx == 1871);
}

TEST_CASE("Animation - get whole animation for different views, 2x2")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(501, haeuser);
  auto tiles = anim.get_animation(0);
  REQUIRE(tiles.size() == haeuser->get_haus(501).value()->AnimAnz);
  REQUIRE(tiles[0][0].gfx == 1816);
  REQUIRE(tiles[0][1].gfx == 1817);
  REQUIRE(tiles[0][2].gfx == 1818);
  REQUIRE(tiles[0][3].gfx == 1819);
  REQUIRE(tiles[1][0].gfx == 1832);
  REQUIRE(tiles[1][1].gfx == 1833);
  REQUIRE(tiles[1][2].gfx == 1834);
  REQUIRE(tiles[1][3].gfx == 1835);
  REQUIRE(tiles[2][0].gfx == 1848);
  REQUIRE(tiles[2][1].gfx == 1849);
  REQUIRE(tiles[2][2].gfx == 1850);
  REQUIRE(tiles[2][3].gfx == 1851);
  REQUIRE(tiles[3][0].gfx == 1864);
  REQUIRE(tiles[3][1].gfx == 1865);
  REQUIRE(tiles[3][2].gfx == 1866);
  REQUIRE(tiles[3][3].gfx == 1867);

  tiles = anim.get_animation(1);
  REQUIRE(tiles[0][0].gfx == 1820);
  REQUIRE(tiles[0][1].gfx == 1821);
  REQUIRE(tiles[0][2].gfx == 1822);
  REQUIRE(tiles[0][3].gfx == 1823);
  REQUIRE(tiles[1][0].gfx == 1836);
  REQUIRE(tiles[1][1].gfx == 1837);
  REQUIRE(tiles[1][2].gfx == 1838);
  REQUIRE(tiles[1][3].gfx == 1839);
  REQUIRE(tiles[2][0].gfx == 1852);
  REQUIRE(tiles[2][1].gfx == 1853);
  REQUIRE(tiles[2][2].gfx == 1854);
  REQUIRE(tiles[2][3].gfx == 1855);
  REQUIRE(tiles[3][0].gfx == 1868);
  REQUIRE(tiles[3][1].gfx == 1869);
  REQUIRE(tiles[3][2].gfx == 1870);
  REQUIRE(tiles[3][3].gfx == 1871);
}