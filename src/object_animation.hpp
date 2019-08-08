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
    }
  }

  std::vector<std::vector<Tile>> get_animation(int rotation = 0) { return AnimationTilesPerRotation[rotation]; }
  std::vector<Tile> get_animation_step(int rotation, int animation_step = 0) { return AnimationTilesPerRotation[rotation][animation_step]; }
  Tile get_animation_tile(int rotation, int animation_step = 0, int tile = 0) { return AnimationTilesPerRotation[rotation][animation_step][tile]; }


private:
  void calculate_tiles()
  {
    TilesForAnimation.clear();
    Animations.clear();
    AnimationTilesPerRotation.clear();

    // Rotate = 0: There is only one image for each view
    if (rotate == 0)
    {
      Tile t;
      t.gfx = startGfx;
      t.x = 0;
      t.y = 0;
      TilesForAnimation.push_back(t);
      for (int v = 0; v < 4; v++)
      {
        Animations.push_back(TilesForAnimation);
        AnimationTilesPerRotation.push_back(Animations);
        Animations.clear();
      }
      return;
    }

    // Rotate != 0, AnimZeit == TIMENEVER: There is one image for each view
    if (animTime <= 0)
    {
      int inc = 0;
      for (int i = 0; i < 4; i++)
      {
        Tile t;
        t.gfx = startGfx + inc++;
        t.x = 0;
        t.y = 0;
        TilesForAnimation.push_back(t);
        Animations.push_back(TilesForAnimation);
        TilesForAnimation.clear();
        AnimationTilesPerRotation.push_back(Animations);
        Animations.clear();
      }
      return;
    }

    // Rotate == 1, AnimZeit != TIMENEVER: There must be a set AnimAdd and AnimCount -> Multiple images for each view
    {
      if (rotate == 1)
      {
        for (int v = 0; v < 4; v++)
        {
          for (int a = 0; a < animCount; a++)
          {
            Tile t;
            t.gfx = startGfx + a * animAdd + v;
            t.x = 0;
            t.y = 0;
            TilesForAnimation.push_back(t);
            Animations.push_back(TilesForAnimation);
            TilesForAnimation.clear();
          }
          AnimationTilesPerRotation.push_back(Animations);
          Animations.clear();
        }
        return;
      }
    }

    // Rotate != 0, AnimZeit != TIMENEVER: There must be a set AnimAdd and AnimCount -> Multiple images for each view
    {
      for (int v = 0; v < 4; v++)
      {
        for (int a = 0; a < animCount; a++)
        {
          int inc = 0;
          for (int i = 0; i < rotate; i++)
          {
            Tile t;
            t.gfx = startGfx + inc++ + a * animAdd + v * rotate;
            t.x = 0;
            t.y = 0;
            TilesForAnimation.push_back(t);
          }
          Animations.push_back(TilesForAnimation);
          TilesForAnimation.clear();
        }
        AnimationTilesPerRotation.push_back(Animations);
        Animations.clear();
      }
      return;
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
  std::vector<std::vector<Tile>> Animations;
  std::vector<std::vector<std::vector<Tile>>> AnimationTilesPerRotation;
  struct
  {
    int width;
    int height;
  } size;
  int id;

  std::shared_ptr<Haeuser> haeuser;
};

#endif