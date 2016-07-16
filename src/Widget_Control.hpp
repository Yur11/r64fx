#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"
#include "ImageAnimation.hpp"

namespace r64fx{

class Image;
class Font;

class ControlAnimation{
    Size<int> m_size = {0, 0};
    int m_frame_count = 0;

public:
    ControlAnimation(Size<int> size, int frame_count);

    virtual ~ControlAnimation();

    void setSize(Size<int> size);

    Size<int> size() const;

    int width() const;

    int height() const;

    int frameCount() const;

    virtual void paint(int frame, Painter* painter) = 0;
};


class ControlAnimation_Knob : public ControlAnimation{
    unsigned char* m_data = nullptr;

public:
    ControlAnimation_Knob(int size, int frame_count);

    virtual ~ControlAnimation_Knob();

private:
    virtual void paint(int frame, Painter* painter);
};


// class ControlAnimation_PlayPauseButton : public ControlAnimation{
//     unsigned char* m_frames = nullptr;
//
// public:
//     ControlAnimation_PlayPauseButton(int size);
//
//     virtual ~ControlAnimation_PlayPauseButton();
//
// private:
//     virtual void paint(ControlAnimationState state, Painter* painter);
//
//     virtual ControlAnimationState mousePress(ControlAnimationState state, Point<int> position);
//
//     virtual ControlAnimationState mouseRelease(ControlAnimationState state, Point<int> position);
//
//     virtual float value(ControlAnimationState state, float minval, float maxval);
//
//     virtual int frameCount();
// };


class Widget_ValueControl : public Widget{
    Font* m_font = nullptr;
    float m_min_value = 0.0f;
    float m_max_value = 1.0f;
    float m_value_step = 0.05;
    float m_value = 0.0f;
    ControlAnimation* m_animation = nullptr;

public:
    Widget_ValueControl(int char_count, Font* font, Widget* parent = nullptr);

    Widget_ValueControl(ControlAnimation* animation, Widget* parent = nullptr);

    virtual ~Widget_ValueControl();

    void setMinValue(float val);

    float minValue() const;

    void setMaxValue(float val);

    float maxValue() const;

    float valueRange() const;

    void setValueStep(float step);

    float valueStep() const;

    void setValue(float val);

    float value() const;

    void setFont(Font* font);

    Font* font() const;

    void setAnimation(ControlAnimation* animation);

    ControlAnimation* animation() const;

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP