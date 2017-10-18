#ifndef R64FX_DEBUG_HPP
#define R64FX_DEBUG_HPP

#ifdef R64FX_DEBUG
#include <iostream>
#include <cassert>
#endif//R64FX_DEBUG

#ifdef R64FX_DEBUG
#define R64FX_DEBUG_ASSERT(arg) assert(arg)
#else
#define R64FX_DEBUG_ASSERT(arg)
#endif//R64FX_DEBUG

#endif//R64FX_DEBUG_HPPs
