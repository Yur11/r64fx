#ifndef R64FX_GUI_ITERATOR_PAIR_H
#define R64FX_GUI_ITERATOR_PAIR_H

namespace r64fx{
  
template<typename IteratorT> class IteratorPair{
    IteratorT _begin, _end;
    
public:
    IteratorPair(IteratorT begin = IteratorT(), IteratorT end = IteratorT())
    : _begin(begin)
    , _end(end)
    {}
    
    inline IteratorT begin() const { return _begin; }
    inline IteratorT end() const { return _end; }
    
    inline void setBeginIterator(IteratorT it) { _begin = it; }
    inline void setEndIterator(IteratorT it) { _end = it; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_ITERATOR_PAIR_H