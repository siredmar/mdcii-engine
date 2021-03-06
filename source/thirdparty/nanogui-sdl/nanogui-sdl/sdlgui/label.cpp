/*
    sdlgui/label.cpp -- Text label with an arbitrary font, color, and size

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <iostream>

#include <SDL2/SDL.h>

#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#include <sdlgui/label.h>
#include <sdlgui/theme.h>
#include <sdlgui/vscrollpanel.h>

NAMESPACE_BEGIN(sdlgui)

Label::Label(Widget* parent, const std::string& caption, const std::string& font, int fontSize)
    : Widget(parent)
    , mParent(parent)
    , mCaption(caption)
    , mFont(font)
    , mMultiline(false)
{
    mType = "Label";
    if (mTheme)
    {
        mFontSize = mTheme->mStandardFontSize;
        mColor = mTheme->mTextColor;
    }

    if (fontSize >= 0)
    {
        mFontSize = fontSize;
    }

    _texture.dirty = true;
}

void Label::setTheme(Theme* theme)
{
    Widget::setTheme(theme);
    if (mTheme)
    {
        mFontSize = mTheme->mStandardFontSize;
        mColor = mTheme->mTextColor;
    }
}

Vector2i Label::preferredSize(SDL_Renderer* ctx) const
{
    if (mCaption == "")
    {
        return Vector2i::Zero();
    }

    // if (mFixedSize.x > 0)
    // {
    //     int w, h;
    //     // const_cast<Label*>(this)->mTheme->getUtf8Bounds(mFont.c_str(), fontSize(), mCaption.c_str(), &w, &h);
    //     SDL_QueryTexture(_texture.tex, nullptr, nullptr, &w, &h);
    //     return Vector2i(mFixedSize.x, h);
    // }
    // else
    // {
    int w, h;
    // const_cast<Label*>(this)->mTheme->getUtf8Bounds(mFont.c_str(), fontSize(), mCaption.c_str(), &w, &h);
    SDL_QueryTexture(_texture.tex, nullptr, nullptr, &w, &h);
    return Vector2i(w, h);
    // }
}

void Label::setFontSize(int fontSize)
{
    Widget::setFontSize(fontSize);
    _texture.dirty = true;
}

// void Label::save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture)
// {
//     SDL_Texture* target = SDL_GetRenderTarget(renderer);
//     SDL_SetRenderTarget(renderer, texture);
//     int width, height;
//     SDL_QueryTexture(texture, NULL, NULL, &width, &height);
//     SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
//     SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
//     IMG_SavePNG(surface, file_name);
//     SDL_FreeSurface(surface);
//     SDL_SetRenderTarget(renderer, target);
// }

void Label::draw(SDL_Renderer* renderer)
{
    static bool bmp = false;
    Widget::draw(renderer);

    // if (mParent->getType() == "VScrollPanel")
    // {
    //     std::cout << " true" << std::endl;
    // }

    if (_texture.dirty)
    {
        mTheme->getTexAndRectUtf8(renderer, _texture, 0, 0, mCaption.c_str(), mFont.c_str(), fontSize(), mColor, mMultiline);
        if (mMultiline != -1)
        {
            // if (bmp == false)
            // {
            //     save_texture("text.png", renderer, _texture.tex);
            // }
            // bmp = true;

            setSize(preferredSize(nullptr));
        }
    }

    if (mParent->getType() == "VScrollPanel" && mParent->size().y < mSize.y)
    {
        auto scrollParent = dynamic_cast<VScrollPanel*>(mParent);
        float maxScroll = scrollParent->scroll(); // * float(scrollParent->size().y) / float(mSize.y);
        // float maxScroll = scrollParent->scroll() * float(scrollParent->size().y - 20) / float(mSize.y);
        std::cout << "msize.y: " << mSize.y << ", parent.y: " << scrollParent->size().y << " maxScroll: " << maxScroll << " " << scrollParent->scroll() << std::endl;
        // SDL_Rect srcrect{ 0, (maxScroll * scrollParent->size().y) - (mSize.y - _texture.rrect.h) * 0.5f, scrollParent->size().x, scrollParent->size().y };
        int y = (maxScroll * scrollParent->size().y);
        std::cout << "src: x: " << 0 << ", y: " << y << ", w: " << mSize.x << ", h: " << scrollParent->size().y << std::endl;
        int y_end = y + scrollParent->size().y;
        std::cout << "y_end: " << y_end << std::endl;
        if (y_end > mSize.y)
        {
            y = mSize.y;
        }
        SDL_Rect srcrect{ 0, y, mSize.x, scrollParent->size().y };
        SDL_Rect destrect{ scrollParent->absolutePosition().x, scrollParent->absolutePosition().y, _texture.w(), scrollParent->size().y };
        SDL_RenderCopyOriginal(renderer, _texture.tex, &srcrect, &destrect);
    }
    else
    {
        if (mFixedSize.x > 0)
        {
            SDL_RenderCopy(renderer, _texture, absolutePosition());
        }
        else
        {
            SDL_RenderCopy(renderer, _texture, absolutePosition() + Vector2i(0, (mSize.y - _texture.rrect.h) * 0.5f));
        }
    }
}

void Label::drawScroll(SDL_Renderer* renderer, Vector2i pos, Vector2i scroll, Vector2i parentSize)
{
    Widget::draw(renderer);
    if (_texture.dirty)
    {
        mTheme->getTexAndRectUtf8(renderer, _texture, 0, 0, mCaption.c_str(), mFont.c_str(), fontSize(), mColor, mMultiline);
        if (mMultiline != -1)
        {
            setSize(Vector2i{ _texture.w(), _texture.h() });
        }
    }

    SDL_Rect srcrect{ pos.x, pos.y - scroll.y, parentSize.x, parentSize.y };
    SDL_Rect destrect{ pos.x, pos.y, parentSize.x, parentSize.y };

    SDL_RenderCopyOriginal(renderer, _texture.tex, &srcrect, &destrect);

    // if (mFixedSize.x > 0)
    // {
    //     SDL_RenderCopy(renderer, _texture, absolutePosition());
    // }
    // else
    // {
    //     SDL_RenderCopy(renderer, _texture, absolutePosition() + Vector2i(0, (mSize.y - _texture.rrect.h) * 0.5f));
    // }
}

NAMESPACE_END(sdlgui)
