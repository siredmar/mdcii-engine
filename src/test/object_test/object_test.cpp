#define CATCH_CONFIG_MAIN

#include "../catch2/catch.hpp"

#include <cstdio>
#include <tuple>
#include <vector>

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
                   "; object with no animation containing separate tiles for each view, 5x7\r\n"
                   "  @Nummer: +1\r\n"
                   "  Id:         20100\r\n"
                   "  Gfx:        1\r\n"
                   "  Size:       2, 2\r\n"
                   "  Rotate:     4\r\n"
                   "  AnimTime:   TIMENEVER\r\n"
                   "  AnimAdd:    0\r\n"
                   "\r\n"
                   "EndObj;\r\n";

#define H 15
#define W 15
int map[H][W];

void clear_map()
{
  for (int h = 0; h < H; h++)
  {
    for (int w = 0; w < W; w++)
    {
      map[h][w] = -1;
    }
  }
}

void draw()
{
  for (int i = 0; i < H; i++)
  {
    printf("%.4d ", i);
  }
  std::cout << std::endl;
  for (int h = 0; h < H; h++)
  {
    printf("%.4d ", h);
    for (int w = 0; w < W; w++)
    {
      if (map[h][w] == -1)
      {
        std::cout << ".... ";
      }
      else
      {
        printf("%.4d ", map[h][w]);
      }
    }
    std::cout << std::endl;
  }
}


// TEST_CASE("Animations - only one tile for each direction, 1x1")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object tree(1352, 1, 2, 0, Object::Player::PLAYER_1, object_animations);
//   for (int i = 0; i < 4; i++)
//   {
//     std::cout << "Animation step: " << i % 4 << std::endl;
//     tree.render();
//   }
// }

// TEST_CASE("Animations - animate building 2x2, view 0, x,y: 0x0")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(501, 0, 0, 0, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 501, 2x2, view 0, x,y: 0x0" << std::endl;
//   clear_map();
//   for (int i = 0; i < 17; i++)
//   {
//     std::cout << "Animation step: " << i % 16 << std::endl;
//     std::vector<std::tuple<int, int, int>> coordinates = building.render();
//     for (auto const& c : coordinates)
//     {
//       std::cout << std::get<0>(c) << ", " << std::get<1>(c) << ": " << std::get<2>(c) << std::endl;
//     }
//   }
// }

// TEST_CASE("Animations - animate building 2x2, view 0, x,y: 10x10")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(501, 10, 10, 0, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 501,  2x2, view 0, x,y: 10x10" << std::endl;
//   clear_map();
//   for (int i = 0; i < 17; i++)
//   {
//     std::cout << "Animation step: " << i % 16 << std::endl;
//     std::vector<std::tuple<int, int, int>> coordinates = building.render();
//     for (auto const& c : coordinates)
//     {
//       map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
//     }
//     draw();
//   }
// }

// TEST_CASE("Animations - animate building 2x2, view 1, x,y: 0x0")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(501, 4, 7, 1, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 501, 2x2, view 1, x,y: 0x0" << std::endl;
//   clear_map();
//   for (int i = 0; i < 17; i++)
//   {
//     std::cout << "Animation step: " << i % 16 << std::endl;
//     std::vector<std::tuple<int, int, int>> coordinates = building.render();
//     for (auto const& c : coordinates)
//     {
//       map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
//     }
//     draw();
//   }
// }


// TEST_CASE("Animations - animate building 2x2, view 2, x,y: 0x0")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(501, 3, 6, 2, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 501, 2x2, view 2, x,y: 0x0" << std::endl;
//   clear_map();
//   for (int i = 0; i < 17; i++)
//   {
//     std::cout << "Animation step: " << i % 16 << std::endl;
//     std::vector<std::tuple<int, int, int>> coordinates = building.render();
//     for (auto const& c : coordinates)
//     {
//       map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
//     }
//     draw();
//   }
// }

TEST_CASE("Animations - animate building 2x2, view 3, x,y: 0x0")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
  std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
  Object building(501, 3, 3, 0, Object::Player::PLAYER_1, object_animations);
  std::cout << "Building 501, 2x2, view 0, x,y: 0x0" << std::endl;
  for (int i = 0; i < 2; i++)
  {
    clear_map();
    std::vector<std::tuple<int, int, int>> coordinates = building.render();
    for (auto const& c : coordinates)
    {
      map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
    }
    draw();
  }
}

TEST_CASE("Animations - animate building 2x2, view 3, x,y: 0x0")
{
  auto files = Files::create_instance(".");
  std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
  std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
  std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
  Object building(501, 3, 3, 1, Object::Player::PLAYER_1, object_animations);
  std::cout << "Building 501, 2x2, view 1, x,y: 0x0" << std::endl;
  for (int i = 0; i < 2; i++)
  {
    clear_map();
    std::vector<std::tuple<int, int, int>> coordinates = building.render();
    for (auto const& c : coordinates)
    {
      map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
    }
    draw();
  }
}

// TEST_CASE("Animations - rotation test - 2x2, view 0, x,y: 5x5")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(831, 5, 5, 0, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 100, 2x2, view 0, x,y: 5x5" << std::endl;
//   clear_map();
//   std::vector<std::tuple<int, int, int>> coordinates = building.render();
//   for (auto const& c : coordinates)
//   {
//     map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
//   }
//   draw();
// }

// TEST_CASE("Animations - rotation test - 2x2, view 1, x,y: 5x5")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(831, 5, 5, 1, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 100, 2x2, view 1, x,y: 5x5" << std::endl;
//   clear_map();
//   std::vector<std::tuple<int, int, int>> coordinates = building.render();
//   for (auto const& c : coordinates)
//   {
//     map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
//   }
//   draw();
// }

// TEST_CASE("Animations - rotation test - 2x2, view 2, x,y: 5x5")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(831, 5, 5, 2, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 100, 2x2, view 2, x,y: 5x5" << std::endl;
//   clear_map();
//   std::vector<std::tuple<int, int, int>> coordinates = building.render();
//   for (auto const& c : coordinates)
//   {
//     map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
//   }
//   draw();
// }

// TEST_CASE("Animations - rotation test - 2x2, view 3, x,y: 5x5")
// {
//   auto files = Files::create_instance(".");
//   std::shared_ptr<Cod_Parser> haeuser_cod = std::make_shared<Cod_Parser>(file);
//   std::shared_ptr<Haeuser> haeuser = std::make_shared<Haeuser>(haeuser_cod);
//   std::shared_ptr<Object_Animations> object_animations = std::make_shared<Object_Animations>(haeuser);
//   Object building(831, 5, 5, 3, Object::Player::PLAYER_1, object_animations);
//   std::cout << "Building 100, 2x2, view 3, x,y: 5x5" << std::endl;
//   clear_map();
//   std::vector<std::tuple<int, int, int>> coordinates = building.render();
//   for (auto const& c : coordinates)
//   {
//     map[std::get<1>(c)][std::get<0>(c)] = std::get<2>(c);
//   }
//   draw();
// }