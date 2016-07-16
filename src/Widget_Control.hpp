#ifndef R64FX_WIDGET_CONTROL_HPP
#define R64FX_WIDGET_CONTROL_HPP

#include "Widget.hpp"
#include "ImageAnimation.hpp"

namespace r64fx{

class Image;
class Font;

struct ControlAnimationState{
    unsigned long bits = 0;

    ControlAnimationState();

    explicit ControlAnimationState(unsigned long bits);
};


class ControlAnimation{
    Size<int> m_size = {0, 0};

public:
    virtual ~ControlAnimation() {}

    void setSize(Size<int> size);

    Size<int> size() const;

    int width() const;

    int height() const;

    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mousePress(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseRelease(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta);

    virtual ControlAnimationState mouseEnter(ControlAnimationState state);

    virtual ControlAnimationState mouseLeave(ControlAnimationState state);

    virtual float value(ControlAnimationState state, float minval, float maxval) = 0;

    virtual int frameCount() = 0;
};


class ControlAnimation_Knob : public ControlAnimation{
protected:
    unsigned char* m_data = nullptr;
    int m_frame_count = 16;

public:
    ControlAnimation_Knob(int knob_radius, int frame_count);

    virtual ~ControlAnimation_Knob();

private:
    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mouseMove(ControlAnimationState state, Point<int> position, Point<int> delta);

    virtual float value(ControlAnimationState state, float minval, float maxval);

    virtual int frameCount();
};


class ControlAnimation_PlayPauseButton : public ControlAnimation{
    unsigned char* m_frames = nullptr;

public:
    ControlAnimation_PlayPauseButton(int size);

    virtual ~ControlAnimation_PlayPauseButton();

private:
    virtual void paint(ControlAnimationState state, Painter* painter);

    virtual ControlAnimationState mousePress(ControlAnimationState state, Point<int> position);

    virtual ControlAnimationState mouseRelease(ControlAnimationState state, Point<int> position);

    virtual float value(ControlAnimationState state, float minval, float maxval);

    virtual int frameCount();
};


class Widget_Control : public Widget{
    ControlAnimation*      m_animation = nullptr;
    ControlAnimationState  m_state;

public:
    Widget_Control(ControlAnimation* animation, Widget* parent = nullptr);

    ~Widget_Control();

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);

    virtual void mouseEnterEvent();

    virtual void mouseLeaveEvent();

    virtual void stateChanged();
};


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