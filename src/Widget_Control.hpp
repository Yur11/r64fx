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

    Size<int> size() const;

    int width() const;

    int height() const;

    int frameCount() const;

    virtual void paint(int frame, Painter* painter) = 0;
};


class ControlAnimation_Image : public ControlAnimation{
    unsigned char* m_data = nullptr;

protected:
    enum class FrameFormat{
        FullRGBA,
        BlendedRG
    };

    ControlAnimation_Image(FrameFormat ff, Size<int> size, int frame_count);

    virtual ~ControlAnimation_Image();

    FrameFormat frameFormat() const;

    int frameComponentCount() const;

    int frameSize() const;

    unsigned char* data() const;

    void pickFrame(int frame, Image* img);

public:
    virtual void paint(int frame, Painter* painter);

private:
    ControlAnimation_Image::FrameFormat m_frame_format;
};


enum class KnobType{
    Unipolar,
    Bipolar
};

class ControlAnimation_Knob : public ControlAnimation_Image{
public:
    ControlAnimation_Knob(int size, int frame_count, KnobType kt = KnobType::Unipolar);
};


class ControlAnimation_Button : public ControlAnimation_Image{

public:
    ControlAnimation_Button(Size<int> size, int frame_count);

    virtual int mousePress(int current_frame) = 0;

    virtual int mouseRelease(int current_frame) = 0;

protected:
    void generateMasks(Image* bg, Image* depressed, Image* pressed);
};


class ControlAnimation_ColouredButton : public ControlAnimation_Button{
public:
    ControlAnimation_ColouredButton(int size, unsigned char** rgbas, int num_rgbas);

private:
    virtual int mousePress(int current_frame);

    virtual int mouseRelease(int current_frame);
};


class ControlAnimation_PlayPauseButton : public ControlAnimation_Button{
public:
    ControlAnimation_PlayPauseButton(int size);

private:
    virtual int mousePress(int current_frame);

    virtual int mouseRelease(int current_frame);
};


class Widget_ValueControl : public Widget{
    Font* m_font = nullptr;
    float m_min_value = 0.0f;
    float m_max_value = 1.0f;
    float m_value_step = 0.05;
    float m_value = 0.0f;
    ControlAnimation* m_animation = nullptr;

public:
    Widget_ValueControl(Widget* parent = nullptr);

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

    bool showsText(bool yes);

    bool showsText() const;

    void resizeAndRealign();

    int textWidth() const;

    int textHeight() const;

    int animationWidth() const;

    int animationHeight() const;

protected:
    virtual void paintEvent(PaintEvent* event);

    void paintAnimation(Painter* painter, Point<int> position);

    void paintText(Painter* painter, Point<int> position);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);

    virtual void mouseMoveEvent(MouseMoveEvent* event);
};


class Widget_ButtonControl : public Widget{
    ControlAnimation_Button* m_animation = nullptr;
    int m_frame = 0;

public:
    Widget_ButtonControl(ControlAnimation_Button* animation, Widget* parent = nullptr);

    void setAnimation(ControlAnimation_Button* animation);

protected:
    virtual void paintEvent(PaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_CONTROL_HPP