// #ifndef R64FX_IMAGE_PAINTER_HPP
// #define R64FX_IMAGE_PAINTER_HPP
//
// #include "Image.hpp"
// #include "Transform2D.hpp"
// #include "Rect.hpp"
//
// namespace r64fx{
//
// class ImagePainter{
//     Image*          m_img  = nullptr;
//     unsigned char*  m_fg;
//     unsigned char*  m_bg;
//
// public:
//     ImagePainter();
//
//     ImagePainter(Image* img);
//
//     ImagePainter(Image* img, unsigned char* fg, unsigned char* bg);
//
//     void setImage(Image* img);
//
//     Image* image() const;
//
//     void setForegroundComponents(unsigned char* fg);
//
//     unsigned char* foregroundComponents() const;
//
//     void setBackgroundComponents(unsigned char* bg);
//
//     unsigned char* backgroundComponents() const;
//
//     void fillForeground(Rect<int> rect);
//
//     void fillForeground();
//
//     void fillBackground(Rect<int> rect);
//
//     void fillBackground();
//
//     void fillComponent(int component, unsigned char value, Rect<int> rect);
//
//     void fillComponent(int component, unsigned char value);
//
//     void fill(unsigned char* components, Rect<int> rect);
//
//     void fill(unsigned char* components);
//
//     void implant(Point<int> pos, Image* img);
//
//     void implant(Transform2D<float> transform, Image* img, Rect<int> rect);
//
//     void implant(Transform2D<float> transform, Image* img);
//
//     void blend(Point<int> pos, unsigned char** colors, Image* mask);
//
//     void drawArc(Point<float> center, float radius, float arca, float arcb, float thickness, Rect<int> rect);
//
//     void drawArc(Point<float> center, float radius, float arca, float arcb, float thickness);
//
//     void drawLine(Point<float> a, Point<float> b, float thickness);
//
//     void drawRadius(Point<float> center, float angle, float outer, float inner, float thickness);
// };
//
// }//namespace r64fx
//
// #endif//R64FX_IMAGE_PAINTER_HPP