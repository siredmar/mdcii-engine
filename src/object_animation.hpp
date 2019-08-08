#ifndef OBJECT_ANIMATION_H
#define OBJECT_ANIMATION_H

#include "haeuser.hpp"

#include <memory>
#include <vector>

class Object_Animation
{
public:
  class Tile
  {
  public:
    int x;
    int y;
    int gfx;
  };

  Object_Animation(int ObjectId, std::shared_ptr<Haeuser> haeuser)
    : haeuser(haeuser)
  {
    auto h = haeuser->get_haus(ObjectId);
    if (h)
    {
      animCount = h.value()->AnimAnz;
      animAdd = h.value()->AnimAdd;
      animTime = h.value()->AnimTime;
      rotate = h.value()->Rotate;
      size.width = h.value()->Size.w;
      size.height = h.value()->Size.h;
      id = h.value()->Id;
      startGfx = h.value()->Gfx;
      calculate_tiles();
      // AnimationTilesPerRotation.reserve(4);
      // for (int v = 0; v < 4; v++)
      // {
      //   AnimationTilesPerRotation[v].reserve(1);
      // }
    }
  }

  std::vector<Tile> get_tiles(int rotation) { return AnimationTilesPerRotation[rotation]; }

  void start_animation() { startAnimation = true; }

private:
  void calculate_tiles()
  {
    if (animCount == 0)
    {
      Tile t;
      t.gfx = startGfx;
      t.x = 0;
      t.y = 0;
      TilesForAnimation.push_back(t);
      for (int v = 0; v < 4; v++)
      {
        AnimationTilesPerRotation.push_back(TilesForAnimation);
      }
    }
    else
    {
      // for (int i = 0; i < animCount; i++)
      // {
      //   if (rotate == 0) // There is only one image for each view
      //   {
      //     for (int v = 0; v < 4; v++)
      //     {
      //       Tile t;
      //       t.gfx = startGfx;
      //       t.x = 0;
      //       t.y = 0;
      //       TilesForAnimation.push_back(t);
      //     }
      //   }
      //   else
      //   {
      //   }
      // }
    }
  }

  bool startAnimation;
  int startGfx;  // Gfx
  int animCount; // AnimAnz
  int animAdd;   // AnimAdd
  int animTime;  // AnimTime
  int rotate;    // Rotate
  const int views = 4;
  const int tiles = 1;
  std::vector<Tile> TilesForAnimation;
  std::vector<std::vector<Tile>> AnimationTilesPerRotation;
  struct
  {
    int width;
    int height;
  } size;
  int id;

  std::shared_ptr<Haeuser> haeuser;
};

#endif