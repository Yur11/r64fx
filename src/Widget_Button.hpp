#ifndef R64FX_WIDGET_BUTTON_HPP
#define R64FX_WIDGET_BUTTON_HPP

#include "Widget.hpp"
#include "Color.hpp"

namespace r64fx{

class ButtonAnimation;
class Font;

class Widget_Button : public Widget{
    ButtonAnimation* m_animation = nullptr;
    unsigned long m_state = 0;
    void (*m_on_state_changed)(void* arg, Widget_Button* button, unsigned long state) = nullptr;
    void* m_on_state_changed_arg = nullptr;

public:
    Widget_Button(ButtonAnimation* animation, Widget* parent = nullptr);

    Widget_Button(ButtonAnimation* animation, bool own_animation, Widget* parent = nullptr);

    virtual ~Widget_Button();

    ButtonAnimation* animation() const;

    bool ownsAnimation() const;

    bool isPressed() const;

    void setFrame(unsigned long frame);

    bool frame() const;

    void onStateChanged(void (*on_state_changed)(void* arg, Widget_Button* button, unsigned long state), void* arg = nullptr);

public:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void mousePressEvent(MousePressEvent* event);

    virtual void mouseReleaseEvent(MouseReleaseEvent* event);
};


class ButtonAnimation : public LinkedList<ButtonAnimation>::Node{
    friend class Widget_Button;
    Size<int>       m_size         = {0, 0};
    unsigned long   m_frame_count  = 0;
    unsigned char*  m_data         = nullptr;

public:
    ButtonAnimation(Size<int> size, int frame_count);

    ~ButtonAnimation();

    Size<int> size() const;

    int width() const;

    int height() const;

    unsigned long frameCount() const;

    bool isGood() const;

    void pickFrame(Image* img, int frame_num);

    static ButtonAnimation* CenteredImageMask(Size<int> size, Image* mask);

    static ButtonAnimation* Text(const std::string &text, Font* font);

    static ButtonAnimation* Text(Size<int> size, const std::string &text, Font* font);

    static ButtonAnimation* Colored(Size<int> size, Color* colors, int ncolors);

    static ButtonAnimation* PlayPause(Size<int> size);

    static unsigned long PlayFrame();

    static unsigned long PauseFrame();
};

}//namespace

#endif//R64FX_WIDGET_BUTTON_HPP
