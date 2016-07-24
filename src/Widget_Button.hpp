#ifndef R64FX_WIDGET_BUTTON_HPP
#define R64FX_WIDGET_BUTTON_HPP

#include "Widget.hpp"

namespace r64fx{

class ButtonAnimation;

class Widget_Button : public Widget{
    ButtonAnimation* m_animation = nullptr;
    int m_state = 0;

public:
    Widget_Button(ButtonAnimation* animation, Widget* parent = nullptr);

    virtual ~Widget_Button();

    ButtonAnimation* animation() const;

public:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);
};


class ButtonAnimation{
    friend class Widget_Button;
    unsigned int user_count = 0;
    Size<int> m_size = {0, 0};
    int m_frame_count = 0;
    unsigned char* m_data = nullptr;

public:
    ButtonAnimation(Size<int> size, int frame_count);

    ~ButtonAnimation();

    Size<int> size() const;

    int width() const;

    int height() const;

    int frameCount() const;

    bool isGood() const;

    void pickFrame(Image* img, int frame_num);

    static ButtonAnimation* Colored(Size<int> size, unsigned char** rgbas, int num_rgbas);

    static ButtonAnimation* PlayPause(Size<int> size);
};

}//namespace

#endif//R64FX_WIDGET_BUTTON_HPP