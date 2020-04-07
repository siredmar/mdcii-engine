/*
    sdlgui/textureview.cpp -- SDL_Texture view

    The texture view widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/textureview.h>
#include <sdlgui/theme.h>

NAMESPACE_BEGIN(sdlgui)

TextureView::TextureView(Widget* parent, SDL_Texture* texture)
  : Widget(parent)
  , mTexture(texture)
{
}

Vector2i TextureView::preferredSize(SDL_Renderer* ctx) const
{
  int w, h;
  SDL_QueryTexture(mTexture, nullptr, nullptr, &w, &h);
  return Vector2i(w, h);
}

void TextureView::draw(SDL_Renderer* renderer)
{
  Widget::draw(renderer);
  SDL_Point ap = getAbsolutePos();
  int w, h;
  SDL_QueryTexture(mTexture, nullptr, nullptr, &w, &h);
  auto mImageSize = Vector2i(w, h);

  int x, y;
  auto pos = absolutePosition();
  x = pos[0];
  y = pos[1];
  SDL_Rect rect{x, y, w, h};

  SDL_RenderCopy(renderer, mTexture, NULL, &rect);
}

NAMESPACE_END(sdlgui)
