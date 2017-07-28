#ifndef R64FX_FILTER_CLASS_HPP
#define R64FX_FILTER_CLASS_HPP

#include "Complex.hpp"
#include "Expression.hpp"
#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{

class SysFunRoot : public LinkedList<SysFunRoot>::Node{
    friend class FilterClass;

    unsigned long m_data  = 0;

protected:
    unsigned long m_flags = 0;

    SysFunRoot() { setValue(Complex<float>(0.0f, 0.0f)); }

    SysFunRoot(Complex<float> value) { setValue(value); }

    SysFunRoot(Expression* expr) { setExpression(expr); }

public:
    SysFunRoot* setValue(Complex<float> value);

    Complex<float> value() const;

    SysFunRoot* setExpression(Expression* expr);

    Expression* expression() const;

    bool hasValue() const;

    inline bool hasExpression() const { return !hasValue(); }

    SysFunRoot* makeZero();

    SysFunRoot* makePole();

    bool isPole() const;

    inline bool isZero() const { return !isPole(); }

    SysFunRoot* enableConjugate();

    SysFunRoot* disableConjugate();

    bool hasConjugate() const;

    SysFunRoot* enableDeletionWithParent();

    SysFunRoot* disableDeletionWithParent();

    bool isDeletedWithParent() const;

private:
    void setIndex(int index);

    int index() const;
};

typedef IteratorPair<LinkedList<SysFunRoot>::Iterator> SysFunRootIterators;


class FilterClass{
    LinkedList<SysFunRoot> m_roots; // Zeros and Poles
    SysFunRoot* m_last_zero = nullptr;

public:
    ~FilterClass();

    void addRoot(SysFunRoot* root);

    void removeRoot(SysFunRoot* root);

    inline SysFunRootIterators roots() const { return {m_roots.begin(), m_roots.end()}; }

    SysFunRootIterators zeros() const;

    SysFunRootIterators poles() const;

    inline bool isEmpty() const { return m_roots.isEmpty(); }

    int rootCount() const; // Roots with enabled conjugates count as two!

    int zeroCount() const;

    int poleCount() const;

    int firstPoleIndex() const;

    void updateIndices();
};

}//namespace r64fx

#endif//R64FX_FILTER_CLASS_HPP
