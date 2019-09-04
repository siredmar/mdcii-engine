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

  // std::vector<std::tuple<int, int, int>> render()
  // {
  //   std::vector<std::tuple<int, int, int>> ret;
  //   int x_offset = 0;
  //   int y_offset = 0;
  //   // int i = size.height * size.width + rot;
  //   // for (int x = 0; i < size.width; x++)
  //   // {
  //   //   for (int y = 0; y < size.heigth; y++)
  //   //   {
  //   //   }
  //   // }

  //   for (int i = 0 + rot; i < size.height * size.width + rot; i++)
  //   {
  //     int x_pos = i % size.width;
  //     int y_pos = (i / size.width) % size.height;

  //     int gfx = ani->animation->get_animation_tile(rot, current_animation_step, i % (size.height * size.width)).gfx;
  //     int final_x = 0;
  //     int final_y = 0;
  //     switch (rot)
  //     {
  //       case 0:
  //         final_x = x + x_offset;
  //         final_y = y + y_offset;
  //         break;
  //       case 1:
  //         final_x = x - x_offset;
  //         final_y = y + y_offset;
  //         break;
  //       case 2:
  //         final_x = x + x_offset;
  //         final_y = y - y_offset;
  //         break;
  //       case 3:
  //         final_x = x - x_offset;
  //         final_y = y - y_offset;
  //         break;
  //     }
  //     std::tuple<int, int, int> coordinates = {final_x, final_y, gfx};
  //     x_offset++;
  //     ret.push_back(coordinates);
  //     if (i - rot == size.width - 1)
  //     {
  //       y_offset++;
  //       x_offset = 0;
  //     }
  //   }
  //   animate();
  //   return ret;
  // }


  std::vector<std::tuple<int, int, int>> render()
  {
    std::vector<std::tuple<int, int, int>> ret;
    for (int i = 0; i < size.height * size.width; i++)
    {
      int x_pos = i % size.width;
      int y_pos = (i / size.width) % size.height;

      int gfx = ani->animation->get_animation_tile(rot, current_animation_step, i % (size.height * size.width)).gfx;
      int final_x = x + x_pos;
      int final_y = x + y_pos;
      // switch (rot)
      // {
      //   case 0:
      //     final_x = x + x_offset;
      //     final_y = y + y_offset;
      //     break;
      //   case 1:
      //     final_x = x - x_offset;
      //     final_y = y + y_offset;
      //     break;
      //   case 2:
      //     final_x = x + x_offset;
      //     final_y = y - y_offset;
      //     break;
      //   case 3:
      //     final_x = x - x_offset;
      //     final_y = y - y_offset;
      //     break;
      // }
      std::tuple<int, int, int> coordinates = {final_x, final_y, gfx};
      // x_offset++;
      ret.push_back(coordinates);
      // if (i - rot == size.width - 1)
      // {
      //   // y_offset++;
      //   // x_offset = 0;
      // }
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
};

#endif