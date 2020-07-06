/*
    sdlgui/TextureTable.cpp -- Table view with TextureButtons

    The texture view widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/theme.h>

#include <sdlgui/texturetable.h>

#include <iostream>

NAMESPACE_BEGIN(sdlgui)

TextureTable::TextureTable(Widget* parent, Vector2i pos, std::function<Widget*(const std::vector<std::tuple<std::string, std::string, int>>&)> factory,
    const std::vector<std::tuple<std::string, std::string, int>>& data, unsigned int visibleElements, int scrollBy, int verticalMargin)
  : Widget(parent)
  , pos(pos)
  , visibleElements(visibleElements)
  , scrollBy(scrollBy)
  , verticalMargin(verticalMargin)
  , visibleFrom(0)
  , visibleTo(visibleFrom + visibleElements)
  , visibleFromOld(visibleFrom)
  , visibleToOld(visibleToOld)
  , tableaddr(factory(data))
{
  tableaddr->setSize(preferredSize(NULL));
  addChild(tableaddr);
  if (tableaddr->childCount() < visibleElements)
  {
    visibleTo = tableaddr->childCount();
    visibleToOld = visibleTo;
  }
}

Vector2i TextureTable::preferredSize(SDL_Renderer* ctx) const
{
  int w = tableaddr->childAt(0)->size()[0];
  int h = 0;
  for (int i = 0; i < visibleElements; i++)
  {
    h += tableaddr->childAt(0)->size()[1];
  }
  return Vector2i(w, h);
}

bool TextureTable::mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers)
{
  for (auto i = 0; i < tableaddr->childCount(); i++)
  {
    auto child = tableaddr->childAt(i);
    if (!child->visible())
      continue;
    bool contained = child->contains(p - _pos);
    bool prevContained = child->contains(p - _pos - rel);
    if (contained != prevContained)
      child->mouseEnterEvent(p, contained);
    if ((contained || prevContained) && child->mouseMotionEvent(p - _pos, rel, button, modifiers))
      return true;
  }
  return false;
}

bool TextureTable::scrollEvent(const Vector2i& p, const Vector2f& rel)
{
  if (rel.y < 0)
  {
    scrollNegative();
    setVisibleFlag = false;
    return true;
  }
  else if (rel.y > 0)
  {
    scrollPositive();
    setVisibleFlag = false;
    return true;
  }
  return Widget::scrollEvent(p, rel);
}

void TextureTable::draw(SDL_Renderer* renderer)
{
  Widget::draw(renderer);
  int y = verticalMargin;
  setVisibleFlag = false;
  if (setVisibleFlag == false)
  {
    for (int i = visibleFromOld; i < visibleToOld; i++)
    {
      tableaddr->childAt(i)->setVisible(false);
    }
    visibleFromOld = visibleFrom;
    visibleToOld = visibleTo;
    std::cout << "visibleFrom: " << visibleFrom << std::endl;
    std::cout << "visibleTo: " << visibleTo << std::endl;
    for (int i = visibleFrom; i < visibleTo; i++)
    {
      Widget* w = tableaddr->childAt(i);
      w->setPosition(0, y + verticalMargin);
      w->setVisible(true);
      if (setVisibleFlag == false)
      {
        setVisibleFlag = true;
      }
      y += w->childAt(0)->size()[1];
    }
  }
}

void TextureTable::setVisibleElements(int visible)
{
  visibleElements = visible;
}

void TextureTable::setScrollBy(int scroll)
{
  scrollBy = scroll;
}

void TextureTable::setVerticalMargin(int margin)
{
  verticalMargin = margin;
}

uint32_t TextureTable::elementsCount()
{
  return tableaddr->childCount();
}

void TextureTable::scrollPositive()
{
  if (visibleFrom - scrollBy <= 0)
  {
    visibleFrom = 0;
  }
  else
  {
    visibleFrom -= scrollBy;
  }
  int visibleRange = visibleElements;
  if (tableaddr->childCount() < visibleElements)
  {
    visibleRange = tableaddr->childCount();
  }
  visibleTo = visibleFrom + visibleRange;
}

void TextureTable::scrollNegative()
{
  if (visibleTo + scrollBy > elementsCount())
  {
    visibleTo = elementsCount();
  }
  else
  {
    visibleTo += scrollBy;
  }
  int visibleRange = visibleElements;
  if (tableaddr->childCount() < visibleElements)
  {
    visibleRange = tableaddr->childCount();
  }
  visibleFrom = visibleTo - visibleRange;
}

void TextureTable::addElement(Widget widget)
{
}

void TextureTable::addElements(std::vector<Widget> widgets)
{
}

void TextureTable::updateElements(std::vector<Widget> widgets)
{
}

bool TextureTable::removeElement(unsigned int index)
{

  if (index >= tableaddr->childCount())
  {
    return false;
  }
  tableaddr->removeChild(index);
  return true;
}

void TextureTable::clear()
{
  for (int i = 0; i < tableaddr->childCount(); i++)
  {
    tableaddr->removeChild(i);
  }
}

NAMESPACE_END(sdlgui)
