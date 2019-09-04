#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include "object_animation.hpp"
#include "object_animations.hpp"

// #include "Matrix.hpp"

#include <memory>
#include <tuple>
#include <vector>

class Object
{
public:
  enum class Player
  {
    PLAYER_1 = 0,
    PLAYER_2,
    PLAYER_3,
    PLAYER_4,
    PIRATE,
    NEUTRAL
  };

  Object(int id, int x, int y, int rot, Player player, std::shared_ptr<Object_Animations> animations)
    : x(x)
    , y(y)
    , animations(animations)
    , animation_trigger(false)
    , animimation_steps(0)
    , current_animation_step(0)
    , rot(rot)
    , player(player)
  {
    ani = animations->get_animation(id);
    animimation_steps = ani->animation->get_animation(rot).size();
    size = ani->animation->get_size();
    std::vector<std::vector<Object_Animation::Tile>> initialIndexes;
    for (int y = 0; y < size.height; y++)
    {
      std::vector<Object_Animation::Tile> line;
      for (int x = 0; x < size.width; x++)
      {
        Object_Animation::Tile t;
        t.gfx = x + y * size.width;
        t.x = x;
        t.y = y;
        line.push_back(t);
      }
      initialIndexes.push_back(line);
    }
    std::vector<std::vector<std::vector<Object_Animation::Tile>>> RotationGfxIndexes_temp;

    RotationGfxIndexes_temp.push_back(initialIndexes);

    auto rotatedIndexes = RotateIndexesClockwise(1, initialIndexes);
    RotationGfxIndexes_temp.push_back(rotatedIndexes);

    rotatedIndexes = RotateIndexesClockwise(2, rotatedIndexes);
    RotationGfxIndexes_temp.push_back(rotatedIndexes);

    rotatedIndexes = RotateIndexesClockwise(3, rotatedIndexes);
    RotationGfxIndexes_temp.push_back(rotatedIndexes);

    for (auto views : RotationGfxIndexes_temp)
    {
      std::vector<Object_Animation::Tile> line;
      for (auto y : views)
      {
        for (auto x : y)
        {
          line.push_back(x);
        }
      }
      RotationGfxIndexes.push_back(line);
    }
    // for (auto v : RotationGfxIndexes)
    // {
    //   for (auto line : v)
    //   {
    //     std::cout << line;
    //   }
    //   std::cout << std::endl;
    // }
  }

  std::vector<std::vector<Object_Animation::Tile>> RotateIndexesClockwise(int rot, std::vector<std::vector<Object_Animation::Tile>> v)
  {
    std::vector<std::vector<Object_Animation::Tile>> rotated;
    for (size_t x = 0; x < v[0].size(); x++)
    {
      std::vector<Object_Animation::Tile> newRow;
      for (int y = v.size() - 1; y >= 0; y--)
      {
        v[y][x].x = x;
        v[y][x].y = y;
        newRow.push_back(v[y][x]);
      }
      rotated.push_back(newRow);
    }

    
    return rotated;
  }

  void trigger_animation()
  {
    animation_trigger = true;
  }

  void animate()
  {
    // TODO: Add timing from SDL
    if (++current_animation_step % animimation_steps == 0)
    {
      current_animation_step = 0;
    }
  }

  std::experimental::optional<Object_Animation::Tile> FindTileByIndex(int rot, int i)
  {
    for (auto e : RotationGfxIndexes[rot])
    {
      if (e.gfx == i)
      {
        return e;
      }
    }
    return {};
  }

  std::vector<std::tuple<int, int, int>> render()
  {
    std::vector<std::tuple<int, int, int>> ret;
    // int count = 0;
    // for (auto i : RotationGfxIndexes[rot])
    for (int i = 0; i < size.height * size.width; i++)
    {
      auto tile_to_use = FindTileByIndex(rot, i);
      // auto tile_to_use = RotationGfxIndexes[rot][i];
      int tiles_index = 0;
      int final_x = 0;
      int final_y = 0;
      switch (rot)
      {
        case 0:
          final_x = x + tile_to_use.value().x;
          final_y = y + tile_to_use.value().y;
          break;
        case 1:
          final_x = x + tile_to_use.value().x;
          final_y = y + tile_to_use.value().y;
          break;
        case 2:
          final_x = x + tile_to_use.value().x;
          final_y = y + tile_to_use.value().y;
          break;
        case 3:
          final_x = x + tile_to_use.value().x;
          final_y = y + tile_to_use.value().y;
          break;
      }
      int gfx = ani->animation->get_animation_tile(rot, current_animation_step, tile_to_use.value().gfx).gfx;
      std::tuple<int, int, int> coordinates = {final_x, final_y, gfx};
      ret.push_back(coordinates);
      // count++;
    }
    animate();
    return ret;
  }

private:
  bool animation_trigger;
  int animimation_steps;
  int current_animation_step;
  int x;
  int y;
  int rot;
  Player player;
  Object_Animation::Size size;
  std::shared_ptr<Object_Animations::Animation> ani;
  std::shared_ptr<Object_Animations> animations;

  std::vector<std::vector<Object_Animation::Tile>> RotationGfxIndexes;
};

#endif