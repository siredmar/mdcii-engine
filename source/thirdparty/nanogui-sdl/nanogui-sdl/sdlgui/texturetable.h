/*
    sdl_gui/TextureTable.h -- Table view with TextureButtons
    The TextureTable widget was contributed by Armin Schlegel <armin.schlegel@gmx.de>

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <sdlgui/texturebutton.h>
#include <sdlgui/widget.h>

#include <vector>

NAMESPACE_BEGIN(sdlgui)

class TextureTableBase
{
public:
    virtual void scrollPositive() = 0;
    virtual void scrollNegative() = 0;
    virtual Widget* childAt([[maybe_unused]] int index)
    {
        return nullptr;
    };
};

/**
 * \class TextureTable texturetable.h sdl_gui/texturetable.h
 *
 * \brief Texture view widget.
 *
 * Table view with TextureButtons
 */
template <typename T>
class TextureTable : public TextureTableBase, public Widget
{
public:
    TextureTable(Widget* parent, Vector2i pos, Vector2i size, std::function<Widget*(T)> factory,
        T data, int visibleElements = 10, int scrollBy = 3, int verticalMargin = 2,
        bool mouseScroll = false)
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

    Vector2i preferredSize([[maybe_unused]] SDL_Renderer* ctx) const
    {
        return size;
    }

    bool mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers)
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

    void draw(SDL_Renderer* renderer)
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

    int elementsCount()
    {
        return tableaddr->childCount();
    }

    void scrollPositive()
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

    void scrollNegative()
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

    bool removeElement(unsigned int index)
    {

        if (index >= tableaddr->childCount())
        {
            return false;
        }
        tableaddr->removeChild(index);
        return true;
    }

    void clear()
    {
        for (int i = 0; i < tableaddr->childCount(); i++)
        {
            tableaddr->removeChild(i);
        }
    }

    void setVisible(bool visible)
    {
        mVisible = visible;
        tableaddr->setVisible(visible);
    }

    void setVisibleElements(int visible)
    {
        visibleElements = visible;
    }

    void setScrollBy(int scroll)
    {
        scrollBy = scroll;
    }

    void setMouseScroll(bool state)
    {
        mouseScroll = state;
    }

    void setVerticalMargin(int margin)
    {
        verticalMargin = margin;
    }

    bool scrollEvent(const Vector2i& p, const Vector2f& rel)
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

    void setPosition(const Vector2i& pos)
    {
        _pos = pos;
    }

    void setPosition(int x, int y)
    {
        _pos = Vector2i{ x, y };
    }

protected:
    Vector2i pos;
    Vector2i size;
    int visibleElements;
    int scrollBy;
    int verticalMargin;
    bool mouseScroll;
    int currentScrollIndex;
    int visibleFrom;
    int visibleTo;
    int visibleFromOld;
    int visibleToOld;
    Widget* tableaddr;
    bool setVisibleFlag = false;
};

NAMESPACE_END(sdlgui)
