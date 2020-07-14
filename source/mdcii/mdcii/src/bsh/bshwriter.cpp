
// This file is part of the MDCII Game Engine.
// Copyright (C) 2017  Benedikt Freisen
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

#include "bsh/bshwriter.hpp"

#include <boost/crc.hpp>

using namespace std;

istream& operator>>(istream& is, const char* str)
{
  string temp;
  is >> temp;
  if (temp != str)
    throw new istream::failure("unexpected string constant in input");
  return is;
}

BshWriter::BshWriter(int transparentColor, int extraColumns, bool isZei)
{
  this->transparentColor = transparentColor;
  this->extraColumns = extraColumns;
  this->isZei = isZei;
}

BshWriter::~BshWriter()
{
}

enum zustand_t
{
  TRANSPARENT,
  PIXEL,
};

#define ist_transparent(a) ((a) == transparentColor)

void BshWriter::WriteBsh(uint8_t* image, int width, int height, vector<uint8_t>& target)
{
  int tz = 0;
  int zz = 0;
  int pixel_index = 0;
  zustand_t zustand = TRANSPARENT;

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      if (zustand == TRANSPARENT)
      {
        if (x == 0 && y != 0)
        {
          zz++;
          tz = 0;
        }
        if (ist_transparent(image[y * width + x]))
        {
          tz++;
        }
        else
        {
          for (int i = 0; i < zz; i++)
          {
            target.push_back(0xfe);
          }
          zz = 0;
          while (tz > 253)
          {
            target.push_back(253);
            target.push_back(0);
            tz -= 253;
          }
          target.push_back(tz);
          tz = 1;
          pixel_index = y * width + x;
          zustand = PIXEL;
        }
      }
      else
      {
        if (x == 0 || ist_transparent(image[y * width + x]))
        {
          // tz Pixel ausgeben
          while (tz > 253)
          {
            target.push_back(253);
            for (int i = 0; i < 253; i++)
            {
              target.push_back(image[pixel_index]);
              pixel_index++;
            }
            target.push_back(0);
            tz -= 253;
          }
          target.push_back(tz);
          for (int i = 0; i < tz; i++)
          {
            target.push_back(image[pixel_index]);
            pixel_index++;
          }
          tz = 1;
          if (x == 0 && !ist_transparent(image[y * width + x]))
          {
            target.push_back(0xfe);
            target.push_back(0);
          }
          else
          {
            if (ist_transparent(image[y * width + x]))
            {
              zustand = TRANSPARENT;
              if (x == 0)
              {
                zz++;
              }
            }
          }
        }
        else
        {
          tz++;
        }
      }
    }
  }
  if (zustand == PIXEL)
  {
    // tz Pixel ausgeben
    while (tz > 253)
    {
      target.push_back(253);
      for (int i = 0; i < 253; i++)
      {
        target.push_back(image[pixel_index]);
        pixel_index++;
      }
      target.push_back(0);
      tz -= 253;
    }
    target.push_back(tz);
    for (int i = 0; i < tz; i++)
    {
      target.push_back(image[pixel_index]);
      pixel_index++;
    }
  }
  target.push_back(0xff);
}

void BshWriter::ReadPGM(const char* path, uint8_t*& image, int& width, int& height)
{
  ifstream pgm;
  pgm.open(path, ios_base::in | ios_base::binary);
  pgm.unsetf(ios::skipws);
  pgm >> "P5";
  pgm.setf(ios::skipws);
  pgm >> width >> height >> "255";
  pgm.get();
  image = new uint8_t[width * height];
  pgm.read(reinterpret_cast<char*>(image), static_cast<std::streamsize>(width * height));
  pgm.close();
}

void BshWriter::AttachPGM(const char* path)
{
  int width, height;
  uint8_t* image;
  ReadPGM(path, image, width, height);
  images.emplace_back();
  ImageWithMeta& target = images.back();
  WriteBsh(image, width, height, target.data);
  delete[] image;
  while (target.data.size() % 4 != 0)
    target.data.push_back(0);
  target.typ = 1;
  target.length = target.data.size() + 16;
  target.width = (width > extraColumns) ? width - extraColumns : 1;
  target.height = height;
  boost::crc_32_type crccomp;
  crccomp.process_bytes(target.data.data(), target.data.size());
  target.crc = crccomp.checksum();
  target.duplicateOf = -1;
  target.offset = 0;
}

void BshWriter::WriteFile(const char* path)
{
  if (images.size() < 1)
    return;

  fstream bsh;
  bsh.open(path, fstream::in | fstream::out | fstream::trunc | fstream::binary);
  if (isZei)
  {
    char signatur_zei[16] = {'Z', 'E', 'I', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    bsh.write(signatur_zei, sizeof(signatur_zei));
  }
  else
  {
    char signatur_bsh[16] = {'B', 'S', 'H', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    bsh.write(signatur_bsh, sizeof(signatur_bsh));
  }

  for (unsigned int i = 0; i < images.size(); i++)
  {
    for (unsigned int j = 0; j < i; j++)
    {
      if (images[i].crc == images[j].crc && images[i].width == images[j].width && images[i].height == images[j].height && images[i].typ == images[j].typ
          && images[i].length == images[j].length && images[i].data == images[j].data)
      {
        images[i].duplicateOf = j;
        break;
      }
    }
  }

  uint32_t versatz_absolut = images.size() * sizeof(uint32_t);
  for (unsigned int i = 0; i < images.size(); i++)
  {
    if (images[i].duplicateOf == -1)
    {
      images[i].offset = versatz_absolut;
      versatz_absolut += images[i].length;
    }
    else
      images[i].offset = images[images[i].duplicateOf].offset;
  }

  uint32_t groesse = images.size() * sizeof(uint32_t);
  for (ImageWithMeta& image : images)
  {
    if (image.duplicateOf == -1)
      groesse += image.length;
  }
  bsh.write(reinterpret_cast<char*>(&groesse), sizeof(groesse));

  for (ImageWithMeta& image : images)
  {
    bsh.write(reinterpret_cast<char*>(&image.offset), sizeof(image.offset));
  }

  for (ImageWithMeta& image : images)
  {
    if (image.duplicateOf == -1)
    {
      bsh.write(reinterpret_cast<char*>(&image.width), sizeof(image.width));
      bsh.write(reinterpret_cast<char*>(&image.height), sizeof(image.height));
      bsh.write(reinterpret_cast<char*>(&image.typ), sizeof(image.typ));
      bsh.write(reinterpret_cast<char*>(&image.length), sizeof(image.length));
      bsh.write(reinterpret_cast<char*>(image.data.data()), image.data.size());
    }
  }
}
