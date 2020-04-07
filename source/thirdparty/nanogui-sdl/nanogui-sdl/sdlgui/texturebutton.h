/*
    sdl_gui/texturebutton.h -- Texture button

    The texture button widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

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
 * \class TextureBUtton texturebutton.h sdl_gui/texturebutton.h
 *
 * \brief Texture button widget.
 *
 */
class TextureButton : public Widget
{
public:
  enum Flags
  {
    OnClick = (1 << 0), // 1
    OnHover = (1 << 1), // 2
  };

  TextureButton(Widget* parent, SDL_Texture* primaryTexture);
  TextureButton(Widget* parent, SDL_Texture* primaryTexture, const std::function<void()>& callback)
    : TextureButton(parent, primaryTexture)
  {
    setCallback(callback);
  }
  TextureButton(Widget* parent, SDL_Texture* primaryTexture, SDL_Texture* secondaryTexture, const std::function<void()>& callback)
    : TextureButton(parent, primaryTexture)
  {
    setSecondaryTexture(secondaryTexture);
    setCallback(callback);
  }

  bool pushed() const { return mPushed; }
  void setPushed(bool pushed) { mPushed = pushed; }

  /// Set the push callback (for any type of button)
  std::function<void()> callback() const { return mCallback; }
  void setCallback(const std::function<void()>& callback) { mCallback = callback; }

  int textureSwitchFlags() const { return mFlags; }
  void setTextureSwitchFlags(int buttonFlags) { mFlags = buttonFlags; }

  /// Get the primary SDL_Texture
  SDL_Texture* primaryTexture() const { return mPrimaryTexture; }
  /// Set the primary SDL_Texture
  void setPrimaryTexture(SDL_Texture* texture) { mPrimaryTexture = texture; }

  /// Get the secondary SDL_Texture
  SDL_Texture* secondaryTexture() const { return mSecondaryTexture; }
  /// Set the secondary SDL_Texture
  void setSecondaryTexture(SDL_Texture* texture) { mSecondaryTexture = texture; }

  /// Compute the size needed to fully display the texture button
  virtual Vector2i preferredSize(SDL_Renderer* ctx) const override;
  virtual bool mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) override;
  virtual bool mouseEnterEvent(const Vector2i& p, bool enter) override;
  /// Draw the texture button
  void draw(SDL_Renderer* renderer) override;

  TextureButton& withCallback(const std::function<void()>& callback)
  {
    setCallback(callback);
    return *this;
  }

  TextureButton& withSecondaryTexture(SDL_Texture* texture)
  {
    setSecondaryTexture(texture);
    return *this;
  }

  TextureButton& withTextureSwitchFlags(int flags)
  {
    setTextureSwitchFlags(flags);
    return *this;
  }

protected:
  SDL_Texture* mPrimaryTexture;
  SDL_Texture* mTexture;
  bool mPushed;
  int mFlags;
  bool _pushedTemp;
  SDL_Texture* mSecondaryTexture;
  std::function<void()> mCallback;
};

NAMESPACE_END(sdlgui)
