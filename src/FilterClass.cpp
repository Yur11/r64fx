#include "FilterClass.hpp"

#define R64FX_ROOT_IS_POLE          0x8000000000000000UL
#define R64FX_HAS_VALUE             0x4000000000000000UL
#define R64FX_HAS_CONJUGATE         0x2000000000000000UL
#define R64FX_DELETED_WITH_PARENT   0x1000000000000000UL
#define R64FX_DATA_MASK             0x0FFFFFFFFFFFFFFFUL

namespace r64fx{

namespace{

static_assert(sizeof(unsigned long) == sizeof(Complex<float>), "Expecting: sizeof(unsigned long) == sizeof(Complex<float>) !");

union Cast{
    unsigned long  data;
    Complex<float> complex;

    Cast() {}
};

inline Complex<float> data2complex(unsigned long data) { Cast cast; cast.data = data; return cast.complex; }

inline unsigned long complex2data(Complex<float> complex) { Cast cast; cast.complex = complex; return cast.data; }

}//namespace


SysFunRoot* SysFunRoot::setValue(Complex<float> value)
{
    m_data = complex2data(value);
    m_flags |= R64FX_HAS_VALUE;
    return this;
}


Complex<float> SysFunRoot::value() const
{
    if(hasValue())
        return data2complex(m_data);
    return Complex<float>(0.0f, 0.0f);
}


SysFunRoot* SysFunRoot::setExpression(Expression* expr)
{
    m_data = (unsigned long) expr;
    m_flags &= ~R64FX_HAS_VALUE;
    return this;
}


Expression* SysFunRoot::expression() const
{
    if(hasExpression())
        return (Expression*) m_data;
    return nullptr;
}


bool SysFunRoot::hasValue() const
{
    return m_flags & R64FX_HAS_VALUE;
}


SysFunRoot* SysFunRoot::makeZero()
{
    m_flags &= ~R64FX_ROOT_IS_POLE;
    return this;
}


SysFunRoot* SysFunRoot::makePole()
{
    m_flags |= R64FX_ROOT_IS_POLE;
    return this;
}


bool SysFunRoot::isPole() const
{
    return m_flags & R64FX_ROOT_IS_POLE;
}


SysFunRoot* SysFunRoot::enableConjugate()
{
    m_flags |= R64FX_HAS_CONJUGATE;
    return this;
}


SysFunRoot* SysFunRoot::disableConjugate()
{
    m_flags &= ~R64FX_HAS_CONJUGATE;
    return this;
}


bool SysFunRoot::hasConjugate() const
{
    return m_flags & R64FX_HAS_CONJUGATE;
}


SysFunRoot* SysFunRoot::enableDeletionWithParent()
{
    m_flags |= R64FX_DELETED_WITH_PARENT;
    return this;
}


SysFunRoot* SysFunRoot::disableDeletionWithParent()
{
    m_flags &= ~R64FX_DELETED_WITH_PARENT;
    return this;
}


bool SysFunRoot::isDeletedWithParent() const
{
    return m_flags & R64FX_DELETED_WITH_PARENT;
}


void SysFunRoot::setIndex(int index)
{
    m_flags &= ~R64FX_DATA_MASK;
    m_flags |= index;
}


int SysFunRoot::index() const
{
    return m_flags & R64FX_DATA_MASK;
}


FilterClass::~FilterClass()
{
    auto next_root = m_roots.first();
    while(next_root)
    {
        auto root = next_root;
        next_root = root->next();
        m_roots.remove(root);
        if(root->isDeletedWithParent())
            delete root;
    }
}


void FilterClass::addRoot(SysFunRoot* root)
{
    auto existing_root = (root->isPole() ? m_last_zero : m_roots.last());
    if(existing_root)
    {
        root->setIndex(existing_root->index() + (root->hasConjugate() ? 2 : 1));
        m_roots.insertAfter(existing_root, root);
    }
    else
    {
        root->setIndex(0);
        m_roots.append(root);
    }
}


void FilterClass::removeRoot(SysFunRoot* root)
{
    if(root == m_last_zero)
    {
        m_last_zero = root->prev();
    }
    m_roots.remove(root);
    root->setIndex(0);
}


SysFunRootIterators FilterClass::zeros() const
{
    if(isEmpty())
        return {nullptr, nullptr};

    if(m_last_zero)
        return {m_roots.first(), m_last_zero};

    return {nullptr, nullptr};
}


SysFunRootIterators FilterClass::poles() const
{
    if(isEmpty())
        return {nullptr, nullptr};

    if(m_last_zero)
        return {m_last_zero->next(), m_roots.end()};

    return roots();
}


int FilterClass::rootCount() const
{
    if(isEmpty())
        return 0;
    return m_roots.last()->index() + 1;
}


int FilterClass::zeroCount() const
{
    if(m_last_zero)
        return m_last_zero->index() + 1;
    return 0;
}


int FilterClass::poleCount() const
{
    if(isEmpty())
        return 0;

    return rootCount() - zeroCount();
}


int FilterClass::firstPoleIndex() const
{
    if(isEmpty())
        return -1;

    if(m_last_zero && m_last_zero->next())
    {
        return m_last_zero->next()->index();
    }

    return 0;
}


void FilterClass::updateIndices()
{
    int n = 0;
    for(auto root : m_roots)
    {
        root->setIndex(n);
        n += (root->hasConjugate() ? 2 : 1);
    }
}

}//namespace r64fx
