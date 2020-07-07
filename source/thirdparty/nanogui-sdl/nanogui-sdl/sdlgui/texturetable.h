/*
    sdl_gui/TextureTable.h -- Table view with TextureButtons
    The TextureTable widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <sdlgui/texturebutton.h>
#include <sdlgui/widget.h>

#include <vector>

NAMESPACE_BEGIN(sdlgui)

/**
 * \class TextureTable texturetable.h sdl_gui/texturetable.h
 *
 * \brief Texture view widget.
 *
 * Table view with TextureButtons
 */
class TextureTable : public Widget
{
public:
  TextureTable(Widget* parent, Vector2i pos, Vector2i size, std::function<Widget*(const std::vector<std::tuple<std::string, std::string, int>>&)> factory,
      const std::vector<std::tuple<std::string, std::string, int>>& data, unsigned int visibleElements = 10, int scrollBy = 3, int verticalMargin = 2,
      bool mouseScroll = false);

  /// Compute the size needed to fully display the texture view
  virtual Vector2i preferredSize(SDL_Renderer* ctx) const override;
  void setVisibleElements(int visible);
  void setScrollBy(int scroll);
  void setVerticalMargin(int margin);
  /// Set the position relative to the parent widget
  void setPosition(const Vector2i& pos);
  void setPosition(int x, int y);
  void setVisible(bool visible);
  void setMouseScroll(bool state);

  bool mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers);
  bool scrollEvent(const Vector2i& p, const Vector2f& rel);
  /// Draw the texture view
  void draw(SDL_Renderer* renderer) override;
  uint32_t elementsCount();
  void scrollNegative();
  void scrollPositive();
  void addElement(Widget widget);
  void addElements(std::vector<Widget> widget);
  void updateElements(std::vector<Widget> widget);
  bool removeElement(unsigned int index);
  void clear();

protected:
  Vector2i pos;
  Vector2i size;
  unsigned int visibleElements;
  int scrollBy;
  int verticalMargin;
  bool mouseScroll;
  int currentScrollIndex;
  int visibleFrom;
  int visibleTo;
  int visibleFromOld;
  int visibleToOld;
  Widget* tableaddr;
  bool setVisibleFlag = false;
};

NAMESPACE_END(sdlgui)
