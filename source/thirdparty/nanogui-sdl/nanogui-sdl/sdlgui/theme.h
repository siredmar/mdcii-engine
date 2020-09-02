/*
    sdlgui/theme.h -- Storage class for basic theme-related properties

    The text box widget was contributed by Christian Schueller.

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <mutex>
#include <sdlgui/common.h>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Rect;

NAMESPACE_BEGIN(sdlgui)

struct Texture
{
    SDL_Texture* tex = nullptr;
    SDL_Rect rrect;
    bool dirty = false;

    inline int w() const
    {
        return rrect.w;
    }
    inline int h() const
    {
        return rrect.h;
    }
};

void SDL_RenderCopy(SDL_Renderer* renderer, Texture& tex, const Vector2i& pos);
void SDL_RenderCopyOriginal(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* srcrect, const SDL_Rect* dstrect);
/**
 * \class Theme theme.h sdlgui/theme.h
 *
 * \brief Storage class for basic theme-related properties.
 */

class Theme : public Object
{
public:
    Theme(SDL_Renderer* ctx);

    /* Spacing-related parameters */
    int mStandardFontSize;
    int mButtonFontSize;
    int mTextBoxFontSize;
    int mWindowCornerRadius;
    int mWindowHeaderHeight;
    int mWindowDropShadowSize;
    int mButtonCornerRadius;
    float mTabBorderWidth;
    int mTabInnerMargin;
    int mTabMinButtonWidth;
    int mTabMaxButtonWidth;
    int mTabControlWidth;
    int mTabButtonHorizontalPadding;
    int mTabButtonVerticalPadding;

    std::mutex loadMutex;

    /* Generic colors */
    Color mDropShadow;
    Color mTransparent;
    Color mBorderDark;
    Color mBorderLight;
    Color mBorderMedium;
    Color mTextColor;
    Color mDisabledTextColor;
    Color mTextColorShadow;
    Color mIconColor;

    /* Button colors */
    Color mButtonGradientTopFocused;
    Color mButtonGradientBotFocused;
    Color mButtonGradientTopUnfocused;
    Color mButtonGradientBotUnfocused;
    Color mButtonGradientTopPushed;
    Color mButtonGradientBotPushed;

    /* TextBox colors */
    Color mTextBoxGradientTopFocused;
    Color mTextBoxGradientBotFocused;
    Color mTextBoxGradientTopUnfocused;
    Color mTextBoxGradientBotUnfocused;
    Color mTextBoxGradientTopBackground;
    Color mTextBoxGradientBotBackground;
    Color mTextBoxBorderFocused;
    Color mTextBoxBorderUnfocused;
    Color mTextBoxCursor;
    Color mTextBoxSelection;

    /* Window colors */
    Color mWindowFillUnfocused;
    Color mWindowFillFocused;
    Color mWindowTitleUnfocused;
    Color mWindowTitleFocused;

    /* Slider coloes */
    Color mSliderKnobOuter;
    Color mSliderKnobInner;

    Color mWindowHeaderGradientTop;
    Color mWindowHeaderGradientBot;
    Color mWindowHeaderSepTop;
    Color mWindowHeaderSepBot;

    Color mWindowPopup;
    Color mWindowPopupTransparent;

    void getTexAndRect(SDL_Renderer* renderer, int x, int y, const char* text,
        const char* fontname, size_t ptsize, SDL_Texture** texture, SDL_Rect* rect, SDL_Color* textColor, int lineWidth = -1);

    void getTexAndRectUtf8(SDL_Renderer* renderer, int x, int y, const char* text,
        const char* fontname, size_t ptsize, SDL_Texture** texture, SDL_Rect* rect, SDL_Color* textColor, int lineWidth = -1);

    std::string breakText(SDL_Renderer* renderer, const char* string, const char* fontname, int ptsize,
        float breakRowWidth);

    int getTextWidth(const char* fontname, size_t ptsize, const char* text);
    int getUtf8Width(const char* fontname, size_t ptsize, const char* text);
    int getTextBounds(const char* fontname, size_t ptsize, const char* text, int* w, int* h);
    int getUtf8Bounds(const char* fontname, size_t ptsize, const char* text, int* w, int* h);

    void getTexAndRectUtf8(SDL_Renderer* renderer, Texture& tx, int x, int y, const char* text,
        const char* fontname, size_t ptsize, const Color& textColor, int lineWidth = -1);

protected:
    virtual ~Theme(){};
};

NAMESPACE_END(sdlgui)
