#ifndef R64FX_FLIP_FLAGS_HPP
#define R64FX_FLIP_FLAGS_HPP

#include "FlagBits.hpp"

namespace r64fx{

class FlipFlags : public FlagBits<FlipFlags>{
public:
    using FlagBits<FlipFlags>::FlagBits;

    inline static FlipFlags Vert() { return FlipFlags(1); }
    inline static FlipFlags Hori() { return FlipFlags(2); }
    inline static FlipFlags Diag() { return FlipFlags(4); }
};

}//namespace r64fx

#endif//R64FX_FLIP_FLAGS_HPP
