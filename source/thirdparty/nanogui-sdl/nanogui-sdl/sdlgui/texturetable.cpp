/*
    sdlgui/TextureTable.cpp -- Table view with TextureButtons

    The TextureTable widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/texturetable.h>
#include <sdlgui/theme.h>

NAMESPACE_BEGIN(sdlgui)

TextureTable::TextureTable(Widget* parent, Vector2i pos, Vector2i size,
    std::function<Widget*(const std::vector<std::tuple<std::string, std::string, int>>&)> factory,
    const std::vector<std::tuple<std::string, std::string, int>>& data, unsigned int visibleElements, int scrollBy, int verticalMargin, bool mouseScroll)
  : Widget(parent)
  , pos(pos)
  , size(size)
  , visibleElements(visibleElements)
  , scrollBy(scrollBy)
  , verticalMargin(verticalMargin)
  , mouseScroll(mouseScroll)
  , visibleFrom(0)
  , visibleTo(visibleFrom + visibleElements)
  , visibleFromOld(visibleFrom)
  , visibleToOld(visibleTo)
  , tableaddr(factory(data))
{
  tableaddr->setSize(size);
  addChild(tableaddr);
  if (tableaddr->childCount() < visibleElements)
  {
    visibleTo = tableaddr->childCount();
    visibleToOld = visibleTo;
  }
}

Vector2i TextureTable::preferredSize(SDL_Renderer* ctx) const
{
  return size;
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
  if (mouseScroll)
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

void TextureTable::setMouseScroll(bool state)
{
  mouseScroll = state;
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

void TextureTable::setPosition(const Vector2i& pos)
{
  _pos = pos;
}

void TextureTable::setPosition(int x, int y)
{
  _pos = Vector2i{x, y};
}

void TextureTable::setVisible(bool visible)
{
  mVisible = visible;
  tableaddr->setVisible(visible);
}

NAMESPACE_END(sdlgui)
