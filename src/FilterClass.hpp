#ifndef R64FX_FILTER_CLASS_HPP
#define R64FX_FILTER_CLASS_HPP

#include "Complex.hpp"
#include "Expression.hpp"
#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SysFunRoot{
    unsigned long m_flags = 0;
    unsigned long m_data  = 0;

public:
    SysFunRoot() { setValue(Complex<float>(0.0f, 0.0f)); }

    SysFunRoot(Complex<float> value) { setValue(value); }

    SysFunRoot(Expression* expr) { setExpression(expr); }

    void setValue(Complex<float> value);

    Complex<float> value() const;

    void setExpression(Expression* expr);

    Expression* expression() const;

    bool hasValue() const;

    inline bool hasExpression() const { return !hasValue(); }

    void enableConjugate();

    void disableConjugate();

    bool hasConjugate() const;
};

class Zero : public LinkedList<Zero>::Node, public SysFunRoot{
public:
    using SysFunRoot::SysFunRoot;
};

class Pole : public LinkedList<Pole>::Node, public SysFunRoot{
public:
    using SysFunRoot::SysFunRoot;
};

typedef IteratorPair<LinkedList<Zero>::Iterator> ZeroIterators;
typedef IteratorPair<LinkedList<Pole>::Iterator> PoleIterators;


class FilterClass{
    LinkedList<Zero> m_zeros;
    LinkedList<Pole> m_poles;

public:
    inline void addZero(Zero* zero) { m_zeros.append(zero); }

    inline void addPole(Pole* pole) { m_poles.append(pole); }

    inline void removeZero(Zero* zero) { m_zeros.remove(zero); }

    inline void removePole(Pole* pole) { m_poles.remove(pole); }

    inline ZeroIterators zeros() const { return {m_zeros.begin(), m_zeros.end()}; }

    inline PoleIterators poles() const { return {m_poles.begin(), m_poles.end()}; }
};

}//namespace r64fx

#endif//R64FX_FILTER_CLASS_HPP
