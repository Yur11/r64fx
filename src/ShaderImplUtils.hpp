#ifndef R64FX_SHADER_IMPL_UTILS_HPP
#define R64FX_SHADER_IMPL_UTILS_HPP

#define R64FX_GET_ATTRIB_LOCATION(name) getAttribLocation(attr_##name, #name)
#define R64FX_GET_UNIFORM_LOCATION(name) getUniformLocation(unif_##name, #name)

#endif//R64FX_SHADER_IMPL_UTILS_HPP