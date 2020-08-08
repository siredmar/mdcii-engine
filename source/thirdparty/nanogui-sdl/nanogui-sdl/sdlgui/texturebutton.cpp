/*
    sdlgui/texturebutton.cpp -- SDL_Texture with clickable behaviour like a button

    The texture button widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/texturebutton.h>
#include <sdlgui/theme.h>

NAMESPACE_BEGIN(sdlgui)

TextureButton::TextureButton(Widget* parent, SDL_Texture* texture)
    : Widget(parent)
    , mPrimaryTexture(texture)
    , mTexture(mPrimaryTexture)
    , mPushed(false)
    , mFlags(0)
    , _pushedTemp(false)
{
    int w, h;
    SDL_QueryTexture(mTexture, nullptr, nullptr, &w, &h);
    Vector2i size = { w, h };
    setSize(size);
}

Vector2i TextureButton::preferredSize(SDL_Renderer* ctx) const
{
    int w, h;
    SDL_QueryTexture(mTexture, nullptr, nullptr, &w, &h);
    return Vector2i(w, h);
}

void TextureButton::draw(SDL_Renderer* renderer)
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
    SDL_Rect rect{ x, y, w, h };

    SDL_RenderCopy(renderer, mTexture, NULL, &rect);
}

bool TextureButton::mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers)
{
    Widget::mouseButtonEvent(p, button, down, modifiers);
    /* Temporarily increase the reference count of the button in case the
     button causes the parent window to be destructed */
    ref<TextureButton> self = this;

    if (button == SDL_BUTTON_LEFT && mEnabled)
    {
        bool pushedBackup = mPushed;
        if (down)
        {
            if (mFlags & ToggleButton)
            {
                mPushed = !mPushed;
            }
            else
            {
                mPushed = true;
            }
            if (mFlags & OnClick)
            {
                mTexture = mSecondaryTexture;
            }
        }
        else if (mPushed)
        {
            if (contains(p) && mCallback)
            {
                mCallback();
            }
            if (mFlags & NormalButton)
            {
                mPushed = false;
                mTexture = mPrimaryTexture;
            }
        }
        else
        {
            mTexture = mPrimaryTexture;
        }

        return true;
    }
    return false;
}

bool TextureButton::mouseEnterEvent(const Vector2i& p, bool enter)
{
    Widget::mouseEnterEvent(p, enter);
    /* Temporarily increase the reference count of the button in case the
     button causes the parent window to be destructed */
    ref<TextureButton> self = this;
    if (mFlags & OnHover && mSecondaryTexture)
    {
        if (enter)
        {
            mTexture = mSecondaryTexture;
        }
        else
        {
            mTexture = mPrimaryTexture;
        }
    }
}

NAMESPACE_END(sdlgui)
