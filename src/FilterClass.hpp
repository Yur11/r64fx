#ifndef R64FX_FILTER_CLASS_HPP
#define R64FX_FILTER_CLASS_HPP

#include "Complex.hpp"
#include "Expression.hpp"
#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SysFunRoot{
    friend class FilterClass;

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

private:
    void setIndex(int index);

    int index() const;
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
    void addZero(Zero* zero);

    void addPole(Pole* pole);

    void removeZero(Zero* zero);

    void removePole(Pole* pole);

    inline ZeroIterators zeros() const { return {m_zeros.begin(), m_zeros.end()}; }

    inline PoleIterators poles() const { return {m_poles.begin(), m_poles.end()}; }

    inline bool hasZeros() const { return !m_zeros.isEmpty(); }

    inline bool hasPoles() const { return !m_poles.isEmpty(); }
};

}//namespace r64fx

#endif//R64FX_FILTER_CLASS_HPP
