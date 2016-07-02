#ifndef R64FX_WIDGET_LABEL_HPP
#define R64FX_WIDGET_LABEL_HPP

#include "Widget.hpp"
#include "Image.hpp"

namespace r64fx{

class Widget_Label : public Widget{
    Image* m_image;

public:
    Widget_Label(Widget* parent = nullptr);

    Widget_Label(Image* image, bool copy = true, Widget* parent = nullptr);

    Widget_Label(const std::string &text, Widget* parent = nullptr);

    virtual ~Widget_Label();

    void setImage(Image* image, bool copy = true);

    void setText(const std::string &text);

    void clear();

    Image* image() const;

    bool ownsImage();

protected:
    virtual void paintEvent(PaintEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_LABEL_HPP