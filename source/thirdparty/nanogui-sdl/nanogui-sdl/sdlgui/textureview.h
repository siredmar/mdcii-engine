/*
    sdl_gui/textureview.h -- Texture view
    The texture view widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <sdlgui/widget.h>

NAMESPACE_BEGIN(sdlgui)

/**
 * \class TextureView textureview.h sdl_gui/textureview.h
 *
 * \brief Texture view widget.
 *
 * Lets display view an SDL_Texture.
 */
class TextureView : public Widget
{
public:
  TextureView(Widget* parent, SDL_Texture* texture);

  SDL_Texture* texture() const
  {
    return mTexture;
  }
  /// Set the primary SDL_Texture
  void setTexture(SDL_Texture* texture)
  {
    mTexture = texture;
  }

  /// Compute the size needed to fully display the texture view
  virtual Vector2i preferredSize(SDL_Renderer* ctx) const override;

  /// Draw the texture view
  void draw(SDL_Renderer* renderer) override;

protected:
  SDL_Texture* mTexture;
};

NAMESPACE_END(sdlgui)
