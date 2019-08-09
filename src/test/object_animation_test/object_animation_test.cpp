#define CATCH_CONFIG_MAIN

#include "../catch2/catch.hpp"

#include "../../cod_parser.hpp"
#include "../../files.hpp"
#include "../../haeuser.hpp"
#include "../../object_animation.hpp"

std::string file = "; object with only one tile for each view, 1x1\r\n"
                   "Objekt: HAUS\r\n"
                   "  Nummer:     0\r\n"
                   "  Id:         21352\r\n"
                   "  Gfx:        1491\r\n"
                   "  Size:       1, 1\r\n"
                   "  Rotate:     0\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
                   "\r\n"
                   "; object with separate tiles for each view, 1x1\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         21011\r\n"
                   "  Gfx:        1092\r\n"
                   "  Size:       1, 1\r\n"
                   "  Rotate:     1\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
                   "\r\n"
                   "; object with various animation steps containing separate tiles for each view, 1x1\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         22311\r\n"
                   "  Gfx:        552\r\n"
                   "  Size:       1, 1\r\n"
                   "  Rotate:     1\r\n"
                   "  AnimTime:   130\r\n"
                   "  AnimAdd:    4\r\n"
                   "  AnimAnz:    6\r\n"
                   "\r\n"
                   "; object with various animation steps containing separate tiles for each view, 2x2\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         20501\r\n"
                   "  Gfx:        1816\r\n"
                   "  Size:       2, 2\r\n"
                   "  Rotate:     4\r\n"
                   "  AnimTime:   90\r\n"
                   "  AnimAdd:    16\r\n"
                   "  AnimAnz:    16\r\n"
                   "\r\n"
                   "; object with no animation containing separate tiles for each view, 5x7\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         20831\r\n"
                   "  Gfx:        4964\r\n"
                   "  Size:       5, 7\r\n"
                   "  Rotate:     35\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
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

TEST_CASE("Animation - get tiles for object with no animation, 5x7")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);

  Object_Animation anim(831, haeuser);
  auto tiles = anim.get_animation(0);
  REQUIRE(tiles.size() == 1);
  REQUIRE(tiles[0][0].gfx == 4964);
  REQUIRE(tiles[0][1].gfx == 4965);
  REQUIRE(tiles[0][2].gfx == 4966);
  REQUIRE(tiles[0][3].gfx == 4967);
  REQUIRE(tiles[0][4].gfx == 4968);
  REQUIRE(tiles[0][5].gfx == 4969);
  REQUIRE(tiles[0][6].gfx == 4970);
  REQUIRE(tiles[0][7].gfx == 4971);
  REQUIRE(tiles[0][8].gfx == 4972);
  REQUIRE(tiles[0][9].gfx == 4973);
  REQUIRE(tiles[0][10].gfx == 4974);
  REQUIRE(tiles[0][11].gfx == 4975);
  REQUIRE(tiles[0][12].gfx == 4976);
  REQUIRE(tiles[0][13].gfx == 4977);
  REQUIRE(tiles[0][14].gfx == 4978);
  REQUIRE(tiles[0][15].gfx == 4979);
  REQUIRE(tiles[0][16].gfx == 4980);
  REQUIRE(tiles[0][17].gfx == 4981);
  REQUIRE(tiles[0][18].gfx == 4982);
  REQUIRE(tiles[0][19].gfx == 4983);
  REQUIRE(tiles[0][20].gfx == 4984);
  REQUIRE(tiles[0][21].gfx == 4985);
  REQUIRE(tiles[0][22].gfx == 4986);
  REQUIRE(tiles[0][23].gfx == 4987);
  REQUIRE(tiles[0][24].gfx == 4988);
  REQUIRE(tiles[0][25].gfx == 4989);
  REQUIRE(tiles[0][26].gfx == 4990);
  REQUIRE(tiles[0][27].gfx == 4991);
  REQUIRE(tiles[0][28].gfx == 4992);
  REQUIRE(tiles[0][29].gfx == 4993);
  REQUIRE(tiles[0][30].gfx == 4994);
  REQUIRE(tiles[0][31].gfx == 4995);
  REQUIRE(tiles[0][32].gfx == 4996);
  REQUIRE(tiles[0][33].gfx == 4997);
  REQUIRE(tiles[0][34].gfx == 4998);

  tiles = anim.get_animation(1);
  REQUIRE(tiles[0][0].gfx == 4999);
  REQUIRE(tiles[0][1].gfx == 5000);
  REQUIRE(tiles[0][2].gfx == 5001);
  REQUIRE(tiles[0][3].gfx == 5002);
  REQUIRE(tiles[0][4].gfx == 5003);
  REQUIRE(tiles[0][5].gfx == 5004);
  REQUIRE(tiles[0][6].gfx == 5005);
  REQUIRE(tiles[0][7].gfx == 5006);
  REQUIRE(tiles[0][8].gfx == 5007);
  REQUIRE(tiles[0][9].gfx == 5008);
  REQUIRE(tiles[0][10].gfx == 5009);
  REQUIRE(tiles[0][11].gfx == 5010);
  REQUIRE(tiles[0][12].gfx == 5011);
  REQUIRE(tiles[0][13].gfx == 5012);
  REQUIRE(tiles[0][14].gfx == 5013);
  REQUIRE(tiles[0][15].gfx == 5014);
  REQUIRE(tiles[0][16].gfx == 5015);
  REQUIRE(tiles[0][17].gfx == 5016);
  REQUIRE(tiles[0][18].gfx == 5017);
  REQUIRE(tiles[0][19].gfx == 5018);
  REQUIRE(tiles[0][20].gfx == 5019);
  REQUIRE(tiles[0][21].gfx == 5020);
  REQUIRE(tiles[0][22].gfx == 5021);
  REQUIRE(tiles[0][23].gfx == 5022);
  REQUIRE(tiles[0][24].gfx == 5023);
  REQUIRE(tiles[0][25].gfx == 5024);
  REQUIRE(tiles[0][26].gfx == 5025);
  REQUIRE(tiles[0][27].gfx == 5026);
  REQUIRE(tiles[0][28].gfx == 5027);
  REQUIRE(tiles[0][29].gfx == 5028);
  REQUIRE(tiles[0][30].gfx == 5029);
  REQUIRE(tiles[0][31].gfx == 5030);
  REQUIRE(tiles[0][32].gfx == 5031);
  REQUIRE(tiles[0][33].gfx == 5032);
  REQUIRE(tiles[0][34].gfx == 5033);

  tiles = anim.get_animation(2);
  REQUIRE(tiles[0][0].gfx == 5034);
  REQUIRE(tiles[0][1].gfx == 5035);
  REQUIRE(tiles[0][2].gfx == 5036);
  REQUIRE(tiles[0][3].gfx == 5037);
  REQUIRE(tiles[0][4].gfx == 5038);
  REQUIRE(tiles[0][5].gfx == 5039);
  REQUIRE(tiles[0][6].gfx == 5040);
  REQUIRE(tiles[0][7].gfx == 5041);
  REQUIRE(tiles[0][8].gfx == 5042);
  REQUIRE(tiles[0][9].gfx == 5043);
  REQUIRE(tiles[0][10].gfx == 5044);
  REQUIRE(tiles[0][11].gfx == 5045);
  REQUIRE(tiles[0][12].gfx == 5046);
  REQUIRE(tiles[0][13].gfx == 5047);
  REQUIRE(tiles[0][14].gfx == 5048);
  REQUIRE(tiles[0][15].gfx == 5049);
  REQUIRE(tiles[0][16].gfx == 5050);
  REQUIRE(tiles[0][17].gfx == 5051);
  REQUIRE(tiles[0][18].gfx == 5052);
  REQUIRE(tiles[0][19].gfx == 5053);
  REQUIRE(tiles[0][20].gfx == 5054);
  REQUIRE(tiles[0][21].gfx == 5055);
  REQUIRE(tiles[0][22].gfx == 5056);
  REQUIRE(tiles[0][23].gfx == 5057);
  REQUIRE(tiles[0][24].gfx == 5058);
  REQUIRE(tiles[0][25].gfx == 5059);
  REQUIRE(tiles[0][26].gfx == 5060);
  REQUIRE(tiles[0][27].gfx == 5061);
  REQUIRE(tiles[0][28].gfx == 5062);
  REQUIRE(tiles[0][29].gfx == 5063);
  REQUIRE(tiles[0][30].gfx == 5064);
  REQUIRE(tiles[0][31].gfx == 5065);
  REQUIRE(tiles[0][32].gfx == 5066);
  REQUIRE(tiles[0][33].gfx == 5067);
  REQUIRE(tiles[0][34].gfx == 5068);

  tiles = anim.get_animation(3);
  REQUIRE(tiles[0][0].gfx == 5069);
  REQUIRE(tiles[0][1].gfx == 5070);
  REQUIRE(tiles[0][2].gfx == 5071);
  REQUIRE(tiles[0][3].gfx == 5072);
  REQUIRE(tiles[0][4].gfx == 5073);
  REQUIRE(tiles[0][5].gfx == 5074);
  REQUIRE(tiles[0][6].gfx == 5075);
  REQUIRE(tiles[0][7].gfx == 5076);
  REQUIRE(tiles[0][8].gfx == 5077);
  REQUIRE(tiles[0][9].gfx == 5078);
  REQUIRE(tiles[0][10].gfx == 5079);
  REQUIRE(tiles[0][11].gfx == 5080);
  REQUIRE(tiles[0][12].gfx == 5081);
  REQUIRE(tiles[0][13].gfx == 5082);
  REQUIRE(tiles[0][14].gfx == 5083);
  REQUIRE(tiles[0][15].gfx == 5084);
  REQUIRE(tiles[0][16].gfx == 5085);
  REQUIRE(tiles[0][17].gfx == 5086);
  REQUIRE(tiles[0][18].gfx == 5087);
  REQUIRE(tiles[0][19].gfx == 5088);
  REQUIRE(tiles[0][20].gfx == 5089);
  REQUIRE(tiles[0][21].gfx == 5090);
  REQUIRE(tiles[0][22].gfx == 5091);
  REQUIRE(tiles[0][23].gfx == 5092);
  REQUIRE(tiles[0][24].gfx == 5093);
  REQUIRE(tiles[0][25].gfx == 5094);
  REQUIRE(tiles[0][26].gfx == 5095);
  REQUIRE(tiles[0][27].gfx == 5096);
  REQUIRE(tiles[0][28].gfx == 5097);
  REQUIRE(tiles[0][29].gfx == 5098);
  REQUIRE(tiles[0][30].gfx == 5099);
  REQUIRE(tiles[0][31].gfx == 5100);
  REQUIRE(tiles[0][32].gfx == 5101);
  REQUIRE(tiles[0][33].gfx == 5102);
  REQUIRE(tiles[0][34].gfx == 5103);
}
