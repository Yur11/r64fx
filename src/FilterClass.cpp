#include "FilterClass.hpp"

#define R64FX_HAS_VALUE             0x8000000000000000UL
#define R64FX_HAS_CONJUGATE         0x4000000000000000UL
#define R64FX_DELETED_WITH_PARENT   0x2000000000000000UL
#define R64FX_DATA_MASK             0x1FFFFFFFFFFFFFFFUL

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


void SysFunRoot::setValue(Complex<float> value)
{
    m_data = complex2data(value);
    m_flags |= R64FX_HAS_VALUE;
}


Complex<float> SysFunRoot::value() const
{
    if(hasValue())
        return data2complex(m_data);
    return Complex<float>(0.0f, 0.0f);
}


void SysFunRoot::setExpression(Expression* expr)
{
    m_data = (unsigned long) expr;
    m_flags &= ~R64FX_HAS_VALUE;
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


FilterClass::~FilterClass()
{
    auto next_zero = m_zeros.first();
    while(next_zero)
    {
        auto zero = next_zero;
        next_zero = zero->next();
        m_zeros.remove(zero);
        if(zero->isDeletedWithParent())
            delete zero;
    }

    auto next_pole = m_poles.first();
    while(next_pole)
    {
        auto pole = next_pole;
        next_pole = pole->next();
        m_poles.remove(pole);
        if(pole->isDeletedWithParent())
            delete pole;
    }
}


void FilterClass::addZero(Zero* zero)
{
    int index = 0;
    if(!m_zeros.isEmpty())
        index = m_zeros.last()->index() + 1;
    zero->setIndex(index);

    m_zeros.append(zero);
}


void FilterClass::addPole(Pole* pole)
{
    int index = 0;
    if(!m_poles.isEmpty())
        index = m_poles.last()->index() + 1;
    pole->setIndex(index);

    m_poles.append(pole);
}


Zero* FilterClass::newZero(Complex<float> value)
{
    auto zero = new Zero(value);
    zero->enableDeletionWithParent();
    if(value.im != 0.0f)
        zero->enableConjugate();
    addZero(zero);
    return zero;
}


Pole* FilterClass::newPole(Complex<float> value)
{
    auto pole = new Pole(value);
    pole->enableDeletionWithParent();
    if(value.im != 0.0f)
        pole->enableConjugate();
    addPole(pole);
    return pole;
}


void FilterClass::removeZero(Zero* zero)
{
    Zero* next_zero = zero->next();
    m_zeros.remove(zero);

    while(next_zero)
    {
        next_zero->setIndex(next_zero->index() - 1);
        next_zero = next_zero->next();
    }
}


void FilterClass::removePole(Pole* pole)
{
    Pole* next_pole = pole->next();
    m_poles.remove(pole);

    while(next_pole)
    {
        next_pole->setIndex(next_pole->index() - 1);
        next_pole = next_pole->next();
    }
}

}//namespace r64fx
