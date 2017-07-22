#ifndef R64FX_EXPRESSION_HPP
#define R64FX_EXPRESSION_HPP

namespace r64fx{

class Expression{
    unsigned long m_flags = 0;

public:
    Expression() {}
};


class BinaryExpression : public Expression{
    Expression* m_left = nullptr;
    Expression* m_right = nullptr;

public:
    BinaryExpression() {}

    BinaryExpression(Expression* left, Expression* right)
    : m_left(left)
    , m_right(right)
    {}

    inline Expression* left() const { return m_left; }

    inline Expression* right() const { return m_right; }
};


class Addition : public BinaryExpression{
public:
    using BinaryExpression::BinaryExpression;
};


class Multiplication : public BinaryExpression{
public:
    using BinaryExpression::BinaryExpression;
};

}//namespace r64fx

#endif//R64FX_EXPRESSION_HPP
