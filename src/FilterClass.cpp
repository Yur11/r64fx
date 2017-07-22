#include "FilterClass.hpp"

namespace r64fx{

namespace{

union Cast{
    unsigned long  data;
    Complex<float> complex = {0.0, 0.0};
};

inline Complex<float> data2complex(unsigned long data) { Cast cast; cast.data = data; return cast.complex; }

inline unsigned int complex2data(Complex<float> complex) { Cast cast; cast.complex = complex; return cast.data; }

}//namespace


void SysFunRoot::setValue(Complex<float> value)
{
    m_data = complex2data(value);
    m_flags |= 1UL;
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
    m_flags &= ~1UL;
}


Expression* SysFunRoot::expression() const
{
    if(hasExpression())
        return (Expression*) m_data;
    return nullptr;
}


bool SysFunRoot::hasValue() const
{
    return m_flags & 1UL;
}


void SysFunRoot::enableConjugate()
{
    m_flags |= 2UL;
}


void SysFunRoot::disableConjugate()
{
    m_flags &= ~2UL;
}


bool SysFunRoot::hasConjugate() const
{
    return m_flags & 2UL;
}

}//namespace r64fx
