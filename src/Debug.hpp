#ifndef R64FX_DEBUG_HPP
#define R64FX_DEBUG_HPP

#ifdef R64FX_DEBUG
#include <iostream>
#include <iomanip>
#include <cassert>
#endif//R64FX_DEBUG

#ifdef R64FX_DEBUG
#define R64FX_DEBUG_ASSERT(arg) { assert(arg); }
#define R64FX_DEBUG_ABORT(msg) { std::cerr << msg; abort(); }

#else
#define R64FX_DEBUG_ASSERT(arg)
#define R64FX_DEBUG_ABORT(msg)
#endif//R64FX_DEBUG

#endif//R64FX_DEBUG_HPP
