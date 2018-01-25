#ifndef R64FX_TYPE_UTILS_HPP
#define R64FX_TYPE_UTILS_HPP

/* Prevent class instances from being constructed, copied or deleted. */
#define R64FX_OPAQUE_HANDLE(T)\
private: T(){} T(const T&){} inline void operator=(const T&){} ~T(){}

#endif//R64FX_TYPE_UTILS_HPP
