#ifndef R64FX_MIXIN_HPP
#define R64FX_MIXIN_HPP

namespace r64fx{

class Empty{};

template<typename Parent=Empty>                                          class MixinStub                          {};
template<template<class> class H=MixinStub, template<class> class... T>  class Mixin      : public H<Mixin<T...>> {};
template<template<class> class... T>                                     class Mix        : public Mixin<T...>    {};

}//namespace r64fx

#endif//R64FX_MIXIN_HPP
