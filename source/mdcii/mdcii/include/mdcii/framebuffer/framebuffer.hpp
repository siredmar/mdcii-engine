
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
// Copyright (C) 2015  Benedikt Freisen
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef _FRAMEBUFFER_HPP_
#define _FRAMEBUFFER_HPP_

#include <inttypes.h>

#include "bsh/bshreader.hpp"
#include "bsh/zeireader.hpp"

class Framebuffer
{
public:
    uint32_t width;
    uint32_t height;

    explicit Framebuffer(uint32_t width, uint32_t height, uint32_t format = 1, uint32_t color = 0, uint8_t* buffer = NULL, uint32_t bufferLength = 0);
    virtual ~Framebuffer();
    void Init(uint32_t width, uint32_t height, uint32_t format, uint32_t color, uint8_t* buffer, uint32_t bufferLength);
    void Uninit();
    void Resize(uint32_t width, uint32_t height, uint32_t format, uint32_t color, uint8_t* buffer, uint32_t bufferLength);

    virtual void DrawBshImage(BshImage& image, int x, int y);
    void zeichne_bsh_bild_oz(BshImage& image, int x, int y);
    virtual void zeichne_bsh_bild_sp(BshImage& image, int x, int y, int sx, int sy, bool& schnitt);
    void zeichne_bsh_bild_sp_oz(BshImage& image, int x, int y, int sx, int sy, bool& schnitt);
    virtual void DrawPixel(int x, int y, uint8_t color) = 0;
    virtual void DrawRectangle(int x1, int y1, int x2, int y2, uint8_t color);
    virtual void DrawLine(int x1, int y1, int x2, int y2, uint8_t color, uint8_t pattern = 0xff);
    virtual void DrawZeiCharacter(ZeiCharacter& character, int x, int y);
    void DrawString(ZeiReader& zei, std::string s, int x, int y);
    void DrawString(ZeiReader& zei, std::wstring s, int x, int y);
    void SetFontColor(uint8_t font, uint8_t shadow);
    void FillWithColor(uint8_t color);
    virtual void ExportPNM(const char* path);
    virtual void ExportBMP(const char* path);
    virtual void Clear();

protected:
    uint8_t* buffer;
    uint8_t freeBuffer;
    uint32_t bufferLength;
    uint32_t format;
    uint32_t color;

    uint8_t fontColorTable[256];
};

#endif // _FRAMEBUFFER_HPP_