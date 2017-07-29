#ifndef R64FX_FILTER_CLASS_HPP
#define R64FX_FILTER_CLASS_HPP

#include "Complex.hpp"
#include "Expression.hpp"
#include "LinkedList.hpp"
#include "IteratorPair.hpp"

namespace r64fx{


// Base class for SysFunRoot and its ctor. parameter.
class SysFunRootData{
    friend class SysFunRoot;

    unsigned long m_data   = 0;
    unsigned long m_flags  = 0;

public:
    SysFunRootData() { setValue(Complex<float>(0.0f, 0.0f)); }

    SysFunRootData(Complex<float> value) { setValue(value); }

    SysFunRootData(float re, float im) { setValue({re, im}); }

    SysFunRootData(Expression* expr) { setExpression(expr); }

    void setValue(Complex<float> value);

    Complex<float> value() const;

    void setExpression(Expression* expr);

    Expression* expression() const;

    bool hasValue() const;

    inline bool hasExpression() const { return !hasValue(); }
};


// Pole or Zero.
class SysFunRoot : public LinkedList<SysFunRoot>::Node, public SysFunRootData{
    friend class FilterClass;
    friend class Zero;
    friend class Pole;

    SysFunRoot(const SysFunRootData &data); // Using implicit call of SysFunRootData params.

public:
    bool isPole() const;

    inline bool isZero() const { return !isPole(); }

    void enableConjugate();

    void disableConjugate();

    bool hasConjugate() const;

    void enableDeletionWithParent();

    void disableDeletionWithParent();

    bool isDeletedWithParent() const;

private:
    void setIndex(int index);

    int index() const;

    void makeZero();

    void makePole();
};

typedef IteratorPair<LinkedList<SysFunRoot>::Iterator> SysFunRootIterators;

class Zero : public SysFunRoot{
public:
    Zero(const SysFunRootData &data) : SysFunRoot(data) { makeZero(); }
};

class Pole : public SysFunRoot{
public:
    Pole(const SysFunRootData &data) : SysFunRoot(data) { makePole(); }
};


class FilterClass{
    LinkedList<SysFunRoot> m_roots; // Zeros and Poles
    SysFunRoot* m_last_zero = nullptr;

public:
    ~FilterClass();

    void addRoot(SysFunRoot* root);

    template<class T> inline T* newRoot(const SysFunRootData &data)
    {
        auto root = new T(data);
        root->enableDeletionWithParent();
        if(root->hasValue())
        {
            if(root->value().im != 0.0f)
                root->enableConjugate();
        }
        addRoot(root);
        return root;
    }

    void removeRoot(SysFunRoot* root);

    inline SysFunRootIterators roots() const { return {m_roots.begin(), m_roots.end()}; }

    SysFunRootIterators zeros() const;

    SysFunRootIterators poles() const;

    inline bool isEmpty() const { return m_roots.isEmpty(); }

    int rootBufferSize() const; // Roots with enabled conjugates count as two!

    int zeroBufferSize() const;

    int poleBufferSize() const;

    void updateIndices();

    Complex<float> evalAt(Complex<float> z);
};

}//namespace r64fx

#endif//R64FX_FILTER_CLASS_HPP
