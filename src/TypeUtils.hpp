#ifndef R64FX_TYPE_UTILS_HPP
#define R64FX_TYPE_UTILS_HPP

#define R64FX_VALUE_TYPE(name, type)\
    class name{ type m_value = {}; public: explicit name(type value) : m_value(value) {};\
        inline type value() const{ return m_value; }\
        inline bool operator==(const name &other) { return value() == other.value(); }\
        inline bool operator!=(const name &other) { return value() != other.value(); }\
        inline explicit operator type() { return value(); }\
    }

#endif//R64FX_TYPE_UTILS_HPP
