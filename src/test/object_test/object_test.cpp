#define CATCH_CONFIG_MAIN

#include "../catch2/catch.hpp"

#include "../../cod_parser.hpp"
#include "../../files.hpp"
#include "../../haeuser.hpp"
#include "../../object.hpp"
#include "../../object_animation.hpp"
#include "../../object_animations.hpp"

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


TEST_CASE("Animations - only one tile for each direction, 1x1")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
  std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
  Object tree(1352, 1, 2, 0, object_animations);
  for (int i = 0; i < 4; i++)
  {
    std::cout << "Animation step: " << i % 4 << std::endl;
    tree.draw();
  }
}

TEST_CASE("Animations - animate building 2x2, view 0")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
  std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
  Object building(501, 1, 2, 0, object_animations);
  std::cout << "Building 501, view 0" << std::endl;
  for (int i = 0; i < 17; i++)
  {
    std::cout << "Animation step: " << i % 16 << std::endl;
    building.draw();
  }
}

TEST_CASE("Animations - animate building 2x2, view 1")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
  std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
  Object building(501, 1, 2, 1, object_animations);
  std::cout << "Building 501, view 1" << std::endl;
  for (int i = 0; i < 17; i++)
  {
    std::cout << "Animation step: " << i % 16 << std::endl;
    building.draw();
  }
}
