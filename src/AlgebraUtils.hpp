#ifndef R64FX_ALGEBRA_UTILS_HPP
#define R64FX_ALGEBRA_UTILS_HPP

#include <vector>
#include <algorithm>
#include <functional>

namespace r64fx{

template<typename T> struct Factor{
    T    variable  = T();
    int  power     = 1;

    Factor(){}

    Factor(const T &variable, int power = 1) : variable(variable), power(power) {}
};


/* ==  Comparison operators for Factor.  == */

template<typename T> bool operator==(const Factor<T> &a, const Factor<T> &b)
{
    return a.variable == b.variable && a.power == b.power;
}

template<typename T> bool operator!=(const Factor<T> &a, const Factor<T> &b)
{
    return !operator==(a, b);
}

template<typename T> bool operator>(const Factor<T> &a, const Factor<T> &b)
{
    if(a.power == b.power)
    {
        return a.variable > b.variable;
    }

    return a.power > b.power;
}

template<typename T> bool operator<(const Factor<T> &a, const Factor<T> &b)
{
    return operator>(b, a);
}


template<typename T> bool operator>=(const Factor<T> &a, const Factor<T> &b)
{
    return !operator<(a, b);
}

template<typename T> bool operator<=(const Factor<T> &a, const Factor<T> &b)
{
    return !operator>(a, b);
}


template<typename T> struct Term : public std::vector<Factor<T>>{
    float coeff = 1.0f;

    Term(){}

    Term(const Factor<T> &factor)
    {
        this->push_back(factor);
    }

    Term(float coeff)
    : coeff(coeff)
    {

    }

    Term(T var, int power)
    {
        this->push_back(Factor<T>(var, power));
    }

    Term(float coeff, T var, int power)
    : coeff(coeff)
    {
        this->push_back(Factor<T>(var, power));
    }

    Term(const Term<T> &other, float coeff)
    : coeff(coeff)
    {
        this->assign(other.begin(), other.end());
    }

    int find(T var)
    {
        for(int i=0; i<(int)this->size(); i++)
        {
            if((*this)[i].variable == var)
                return i;
        }

        return -1;
    }

    void remove(int i)
    {
        this->erase(this->begin() + i);
    }
};


/* ==  Comparison operators for Term.  ==
 * ==  Terms must be sorted!           ==
 */

template<typename T> bool operator==(const Term<T> &a, const Term<T> &b)
{
    if(a.coeff != b.coeff)
        return false;

    if(a.size() != b.size())
        return false;

    for(unsigned int i=0; i<a.size(); i++)
    {
        if(a[i] != b[i])
            return false;
    }

    return true;
}

template<typename T> bool operator!=(const Term<T> &a, const Term<T> &b)
{
    return !operator==(a, b);
}

template<typename T> bool operator>(const Term<T> &a, const Term<T> &b)
{
    int s = min(a.size(), b.size());
    for(int i=0; i<s; i++)
    {
        Factor<T> fa = a[i];
        Factor<T> fb = b[i];

        if(fa.power > fb.power)
            return true;
    }
    return false;
}

template<typename T> bool operator<(const Term<T> &a, const Term<T> &b)
{
    return operator>(b, a);
}

template<typename T> bool operator>=(const Term<T> &a, const Term<T> &b)
{
    return !operator<(a, b);
}

template<typename T> bool operator<=(const Term<T> &a, const Term<T> &b)
{
    return !operator>(a, b);
}


template<typename T> class Polynomial : public std::vector<Term<T>>{

};


template<typename T> Term<T> &operator*=(Term<T> &term, const Factor<T> &factor)
{
    for(auto it=term.begin(); it!=term.end(); it++)
    {
        if(it->variable == factor.variable)
        {
            it->power += factor.power;
            if(it->power == 0)
            {
                term.erase(it);
            }
            return term;
        }
    }

    term.push_back(factor);
    return term;
}


template<typename T> Term<T> &operator*=(Term<T> &term, float coeff)
{
    term.coeff *= coeff;
    return term;
}


template<typename T> Term<T> operator*(const Term<T> &term, float coeff)
{
    Term<T> out = term;
    out *= coeff;
    return out;
}


template<typename T> Term<T> &operator*=(Term<T> &a, const Term<T> &b)
{
    a.coeff *= b.coeff;
    for(auto &factor : b)
    {
        a *= factor;
    }
    return a;
}


template<typename T> Term<T> operator*(const Term<T> &a, const Term<T> &b)
{
    Term<T> out = a;
    out *= b;
    return out;
}


template<typename T> Term<T> operator-(const Term<T> &term)
{
    return Term<T>(term, -(term.coeff));
}


template<typename T> Polynomial<T> &operator+=(Polynomial<T> &a, const Term<T> &b)
{
    a.push_back(b);
    return a;
}


template<typename T> Polynomial<T> operator+=(const Term<T> &a, const Term<T> &b)
{
    Polynomial<T> polynomial;
    polynomial.push_back(a);
    polynomial.push_back(b);
    return polynomial;
}


template<typename T> Polynomial<T> &operator+=(Polynomial<T> &a, const Polynomial<T> &b)
{
    for(auto term : b)
    {
        a.push_back(term);
    }
    return a;
}


template<typename T> Polynomial<T> operator+(const Polynomial<T> &a, const Polynomial<T> &b)
{
    Polynomial<T> out = a;
    out += b;
    return out;
}


template<typename T> Polynomial<T> &operator-=(Polynomial<T> &a, const Term<T> &b)
{
    return operator+=(a, -b);
}


template<typename T> Polynomial<T> operator-=(const Term<T> &a, const Term<T> &b)
{
    return operator+=(a, -b);
}


template<typename T> Polynomial<T> &operator-=(Polynomial<T> &a, const Polynomial<T> &b)
{
    for(auto term : b)
    {
        a.push_back(-term);
    }
    return a;
}


template<typename T> Polynomial<T> operator-(const Polynomial<T> &a, const Polynomial<T> &b)
{
    Polynomial<T> out = a;
    out -= b;
    return out;
}


template<typename T> Polynomial<T> &operator*=(Polynomial<T> &polynomial, const Term<T> &term)
{
    for(auto &t : polynomial)
    {
        t *= term;
    }
    return polynomial;
}


/* Bracket multiplication. */
template<typename T> Polynomial<T> operator*(const Polynomial<T> &a, const Polynomial<T> &b)
{
    Polynomial<T> out;
    for(auto &term_a : a)
    {
        for(auto &term_b : b)
        {
            out += term_a * term_b;
        }
    }
    return out;
}


template<typename T> Polynomial<T> operator-(const Polynomial<T> polynomial)
{
    Polynomial<T> out;
    for(auto term : polynomial)
    {
        out.push_back(-term);
    }
    return out;
}


template<typename T> void sort(Term<T> &term)
{
    std::sort(term.begin(), term.end(), std::greater<Factor<T>>());
}

template<typename T> void sort_terms(Polynomial<T> &polynomial)
{
    for(auto &term : polynomial)
    {
        sort(term);
    }
}

template<typename T> void sort(Polynomial<T> &polynomial)
{
    sort_terms(polynomial);
    std::sort(polynomial.begin(), polynomial.end(), std::greater<Term<T>>());
}


template<typename T> void simplify(Polynomial<T> &polynomial)
{
    if(polynomial.empty())
        return;

    Polynomial<T> out;
    while(!polynomial.empty())
    {
        Term<T> a = polynomial.back();
        polynomial.pop_back();

        float a_coeff = a.coeff;
        a.coeff = 1.0f;

        bool ok = false;
        for(unsigned int i=0; i<out.size(); i++)
        {
            Term<T> b = out[i];
            float b_coeff = b.coeff;
            b.coeff = 1.0f;

            if(a == b)
            {
                out[i].coeff = a_coeff + b_coeff;
                ok = true;
                break;
            }
        }

        if(ok)
            continue;

        a.coeff = a_coeff;
        out.push_back(a);
    }

    for(unsigned int i=0; i<out.size(); i++)
    {
        if(out[i].coeff == 0.0f)
        {
            out.erase(out.begin() + i);
        }
        else
        {
            i++;
        }
    }

    polynomial = out;
}


template<typename T> void simplify_imaginary(Term<T> &term, T imagvar)
{
    int i = term.find(imagvar);
    if(i < 0)
        return;

    int imagpower = term[i].power;

    bool is_negative = false;
    if(imagpower < 0)
    {
        is_negative = true;
        imagpower = -imagpower;
    }

    imagpower &= 3;

    if(!(imagpower & 1))// 0 or 2
    {
        term.remove(i);
    }

    if(imagpower & 2)// 2 or 3
    {
        term.coeff = -(term.coeff);
    }

    if(imagpower & 1)// 1 or 3
    {
        term[i].power = (is_negative ? -1 : 1);
    }
}


template<typename T> void simplify_imaginary(Polynomial<T> &polynomial, T imagvar)
{
    for(auto &term : polynomial)
    {
        simplify_imaginary(term, imagvar);
    }
}

template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, const Factor<T> &factor)
{
    stream << factor.variable[0] << "{" << factor.power << "}";
    return stream;
}

template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, const Term<T> &term)
{
    stream << '[' << term.coeff << ']';
    for(auto &factor : term)
    {
        stream << factor;
    }
    return stream;
}

template<typename StreamT, typename T> StreamT &operator<<(StreamT &stream, const Polynomial<T> &polynomial)
{
    for(auto &term : polynomial)
    {
        stream << term << "\n";
    }
    return stream;
}

}//namespace r64fx

#endif//R64FX_ALGEBRA_UTILS_HPP