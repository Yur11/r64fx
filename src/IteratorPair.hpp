#ifndef R64FX_ITERATOR_PAIR_HPP
#define R64FX_ITERATOR_PAIR_HPP

namespace r64fx{

template<typename IteratorT> class IteratorPair{
    IteratorT m_begin;
    IteratorT m_end;

public:
    IteratorPair(const IteratorT &begin, const IteratorT &end)
    : m_begin(begin)
    , m_end(end)
    {

    }

    inline IteratorT begin() const
    {
        return m_begin;
    }

    inline IteratorT end() const
    {
        return m_end;
    }
};

}//namespace r64fx

#endif//R64FX_ITERATOR_PAIR_HPP