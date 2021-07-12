
// This file is part of the MDCII Game Engine.
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

#include <fstream>
#include <string.h>

#include "framebuffer/framebuffer_pal8.hpp"
#include "framebuffer/palette.hpp"

FramebufferPal8::FramebufferPal8(uint32_t width, uint32_t height, uint32_t color, uint8_t* buffer, uint32_t bufferLength)
    : Framebuffer(width, height, 1, color, buffer, bufferLength)
{
    Clear();
}

void FramebufferPal8::DrawBshImageFull(BshImage& image, int x, int y)
{
    auto palette = Palette::Instance();
    uint8_t* quelle = image.buffer;
    uint8_t* zielzeile;
    uint8_t* target = zielzeile = this->buffer + y * this->bufferLength + x;
    uint32_t restbreite = this->bufferLength;

    while (1)
    {
        uint8_t ch = *(quelle++);
        if (ch == 0xff)
        {
            for (uint32_t i = restbreite; i < width; i++)
            {
                *(target++) = palette->GetTransparentColor();
            }
            break;
        }
        if (ch == 0xfe)
        {
            target = zielzeile += restbreite;
        }
        else
        {
            target += ch;
            uint8_t p = *(quelle++);
            for (ch = p; ch > 0; ch--)
            {
                uint8_t data =  *(quelle++);
                *(target++) = data;
            }
        }
    }
}

void FramebufferPal8::DrawBshImagePartial(BshImage& image, int x, int y)
{
    unsigned char ch;

    uint8_t* quelle = image.buffer;
    uint8_t* zielzeile;
    uint8_t* target = zielzeile = this->buffer + y * (int)this->bufferLength + x;
    int restbreite = this->bufferLength;

    if (x >= 0 && x + image.width < this->width)
    {
        uint8_t* anfang = this->buffer;
        uint8_t* ende = this->buffer + this->bufferLength * this->height;

        while (target < anfang)
        {
            ch = *(quelle++);
            if (ch == 0xff)
            {
                return;
            }
            if (ch == 0xfe)
            {
                target = zielzeile += restbreite;
                if (target >= ende)
                {
                    return;
                }
            }
            else
            {
                target += ch;

                ch = *(quelle++);
                quelle += ch;
                target += ch;
            }
        }
        while ((ch = *(quelle++)) != 0xff)
        {
            if (ch == 0xfe)
            {
                target = zielzeile += restbreite;
                if (target >= ende)
                {
                    return;
                }
            }
            else
            {
                target += ch;

                for (ch = *(quelle++); ch > 0; ch--)
                    *(target++) = *(quelle++);
            }
        }
    }
    else
    {
        int u = 0;
        int v = 0;
        while ((ch = *(quelle++)) != 0xff)
        {
            if (ch == 0xfe)
            {
                target = zielzeile += restbreite;
                u = 0;
                v++;
                if (y + v >= static_cast<int>(this->height))
                {
                    return;
                }
            }
            else
            {
                u += ch;
                target += ch;

                ch = *(quelle++);
                if (y + v >= 0)
                {
                    for (; ch > 0; ch--, u++, quelle++, target++)
                    {
                        if (x + u >= 0 && x + u < static_cast<int>(width))
                        {
                            *target = *quelle;
                        }
                    }
                }
                else
                {
                    u += ch;
                    quelle += ch;
                    target += ch;
                    ch = 0;
                }
            }
        }
    }
}

void FramebufferPal8::DrawBshImage(BshImage& image, int x, int y)
{
    if (x >= static_cast<int>(width) || y >= static_cast<int>(height) || x + static_cast<int>(image.width) < 0 || y + static_cast<int>(image.height) < 0)
    {
        return;
    }
    if ((x < 0) || (y < 0) || (x + static_cast<int>(image.width) > static_cast<int>(width)) || (y + static_cast<int>(image.height) > static_cast<int>(height)))
    {
        DrawBshImagePartial(image, x, y);
    }
    else
    {
        DrawBshImageFull(image, x, y);
    }
}

void FramebufferPal8::DrawPixel(int x, int y, uint8_t color)
{
    if (x < 0 || y < 0 || x >= static_cast<int>(width) || y >= static_cast<int>(height))
    {
        return;
    }
    buffer[y * bufferLength + x] = color;
}

void FramebufferPal8::ExportPNM(const char* path)
{
    std::ofstream pnm;
    pnm.open(path, std::ios_base::out | std::ios_base::binary);
    pnm << "P5\n"
        << width << " " << height << "\n255\n";
    pnm.write((char*)buffer, static_cast<std::streamsize>(width * height * format));
    pnm.close();
}

void FramebufferPal8::ExportBMP(const char* path)
{
    auto palette = Palette::Instance();
    uint32_t bytes_pro_zeile = (width + 3) & 0xfffffffc;
    struct tagBITMAPFILEHEADER
    {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    } __attribute__((packed)) bmfh = { 19778, bytes_pro_zeile * height + 1078, 0, 0, 1078 };
    struct tagBITMAPINFOHEADER
    {
        uint32_t biSize;
        int32_t biWidth;
        int32_t biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t biXPelsPerMeter;
        int32_t biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    } __attribute__((packed)) bmih = { 40, (int32_t)width, (int32_t)height, 1, 8, 0, 0, 0, 0, 0, 0 };

    std::ofstream bmp;
    bmp.open(path, std::ios_base::out | std::ios_base::binary);
    bmp.write((char*)&bmfh, sizeof(struct tagBITMAPFILEHEADER));
    bmp.write((char*)&bmih, sizeof(struct tagBITMAPINFOHEADER));

    for (int i = 0; i < palette->size(); i++)
    {
        bmp << palette->GetColor(i).getBlue() << palette->GetColor(i).getGreen() << palette->GetColor(i).getRed() << (char)0;
    }

    for (int i = height - 1; i >= 0; i--)
    {
        bmp.write((char*)&buffer[width * i], width);
        uint32_t null = 0;
        bmp.write((char*)&null, bytes_pro_zeile - width);
    }

    bmp.close();
}

void FramebufferPal8::Clear()
{
    for (uint32_t i = 0; i < (width * height); i++)
        buffer[i] = color;
}
