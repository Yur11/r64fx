#ifndef R64FX_RING_SECTOR_PAINTER_HPP
#define R64FX_RING_SECTOR_PAINTER_HPP

#include "Image.hpp"
#include "ImageArgs.hpp"

namespace r64fx{

class RingSectorPainter{
    Image m_table;
    Image m_radius;

public:
    RingSectorPainter(int size);

    void paint(ImgPos dst, float min_angle, float max_angle, float outer_radius, float inner_radius = 0);

    inline int size() const { return m_table.width() << 1; }
};

}//namespace

#endif//R64FX_RING_SECTOR_PAINTER_HPP
