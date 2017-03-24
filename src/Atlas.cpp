#include "Atlas.hpp"
#include <assert.h>
#include <iostream>


namespace r64fx{


bool AtlasItem::isEmpty() const
{
    return m_children == nullptr;
}

bool AtlasItem::isFull() const
{
    return m_children == this;
}


Rect<short> AtlasItem::allocRect(Size<short> size)
{
    if(isFull() || size.width() > width() || size.height() > height())
        return {0, 0, 0, 0};

    if(!m_children)
    {
        if(size.width() == width() && size.height() == height())
        {
            m_children = this;
            return *this;
        }
        else
        {
            m_children = new AtlasItem[2];
            if(size.height() == height())
            {
                m_children[0] = AtlasItem(x(), y(), size.width(), size.height());
                m_children[1] = AtlasItem(x() + size.width(), y(), width() - size.width(), height());
            }
            else if(size.width() == width())
            {
                m_children[0] = AtlasItem(x(), y(), size.width(), size.height());
                m_children[1] = AtlasItem(x(), y() + size.height(), width(), height() - size.height());
            }
            else
            {
                m_children[0] = AtlasItem(x(), y(), width(), size.height());
                m_children[1] = AtlasItem(x(), y() + size.height(), width(), height() - size.height());
            }
            return m_children[0].allocRect(size);
        }
    }

    for(int i=0; i<2; i++)
    {
        auto rect = m_children[i].allocRect(size);
        if(rect.width() !=0 && rect.height() != 0)
            return rect;
    }

    return {0, 0, 0, 0};
}


bool AtlasItem::freeRect(Rect<short> rect)
{
    if(!m_children)
        return false;

    if(isFull())
    {
        if(rect == Rect<short>(*this))
        {
            m_children = nullptr;
            return true;
        }
        else
        {
            return false;
        }
    }

    for(int i=0; i<2; i++)
    {
        if(m_children[i].isEmpty())
            continue;
        if(rect.y() > m_children[i].bottom() || rect.x() > m_children[i].right())
            continue;
        if(m_children[i].freeRect(rect))
        {
            if(m_children[0].isEmpty() && m_children[1].isEmpty())
            {
                delete[] m_children;
                m_children = nullptr;
            }
            return true;
        }
    }

    return false;
}


Rect<short> Atlas::allocRect(Size<short> size)
{
    return AtlasItem::allocRect(size);
}


bool Atlas::freeRect(Rect<short> rect)
{
    return AtlasItem::freeRect(rect);
}

}//namespace r64fx