#include "FilterClass.hpp"

#define R64FX_ROOT_IS_POLE          0x8000000000000000UL
#define R64FX_HAS_VALUE             0x4000000000000000UL
#define R64FX_HAS_CONJUGATE         0x2000000000000000UL
#define R64FX_DELETED_WITH_PARENT   0x1000000000000000UL
#define R64FX_DATA_MASK             0x0FFFFFFFFFFFFFFFUL


#include <iostream>

using namespace std;

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


void SysFunRootData::setValue(Complex<float> value)
{
    m_data = complex2data(value);
    m_flags |= R64FX_HAS_VALUE;
}


Complex<float> SysFunRootData::value() const
{
    if(hasValue())
        return data2complex(m_data);
    return Complex<float>(0.0f, 0.0f);
}


void SysFunRootData::setExpression(Expression* expr)
{
    m_data = (unsigned long) expr;
    m_flags &= ~R64FX_HAS_VALUE;
}


Expression* SysFunRootData::expression() const
{
    if(hasExpression())
        return (Expression*) m_data;
    return nullptr;
}


SysFunRoot::SysFunRoot(const SysFunRootData &data)
{
    m_flags = data.m_flags;
    m_data = data.m_data;
}


bool SysFunRootData::hasValue() const
{
    return m_flags & R64FX_HAS_VALUE;
}


bool SysFunRoot::isPole() const
{
    return m_flags & R64FX_ROOT_IS_POLE;
}


void SysFunRoot::enableConjugate()
{
    m_flags |= R64FX_HAS_CONJUGATE;
}


void SysFunRoot::disableConjugate()
{
    m_flags &= ~R64FX_HAS_CONJUGATE;
}


bool SysFunRoot::hasConjugate() const
{
    return m_flags & R64FX_HAS_CONJUGATE;
}


void SysFunRoot::enableDeletionWithParent()
{
    m_flags |= R64FX_DELETED_WITH_PARENT;
}


void SysFunRoot::disableDeletionWithParent()
{
    m_flags &= ~R64FX_DELETED_WITH_PARENT;
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


void SysFunRoot::makeZero()
{
    m_flags &= ~R64FX_ROOT_IS_POLE;
}


void SysFunRoot::makePole()
{
    m_flags |= R64FX_ROOT_IS_POLE;
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
    if(root->isPole())
    {
        m_roots.append(root);
    }
    else
    {
        if(m_last_zero)
        {
            m_roots.insertAfter(m_last_zero, root);
        }
        else
        {
            m_roots.preppend(root);
        }
        m_last_zero = root;
    }
}


void FilterClass::removeRoot(SysFunRoot* root)
{
    if(root == m_last_zero)
    {
        m_last_zero = root->prev();
    }
    m_roots.remove(root);
}


SysFunRootIterators FilterClass::zeros() const
{
    if(isEmpty())
        return {nullptr, nullptr};

    if(m_last_zero)
        return {m_roots.first(), m_last_zero->next()};

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


int FilterClass::rootBufferSize() const
{
    if(isEmpty())
        return 0;
    return m_roots.last()->index() + (m_roots.last()->hasConjugate() ? 2 : 1);
}


int FilterClass::zeroBufferSize() const
{
    if(m_last_zero)
        return m_last_zero->index() + (m_last_zero->hasConjugate() ? 2 : 1);
    return 0;
}


int FilterClass::poleBufferSize() const
{
    if(isEmpty())
        return 0;

    return rootBufferSize() - zeroBufferSize();
}


void FilterClass::updateIndices()
{
    int n = 0;
    for(auto root : m_roots)
    {
        cout << "update: " << n << ", " << (root->isPole() ? "pole" : "zero") << "\n";
        root->setIndex(n);
        n += (root->hasConjugate() ? 2 : 1);
    }
}

}//namespace r64fx
