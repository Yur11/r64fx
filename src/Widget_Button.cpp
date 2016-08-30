#include "Widget_Button.hpp"
#include "WidgetFlags.hpp"
#include "Painter.hpp"
#include "ImageAnimation.hpp"
#include "ImageUtils.hpp"
#include "TextPainter.hpp"

#include <iostream>

using namespace std;

namespace r64fx{

namespace{
    void on_state_changed_stub(void* arg, Widget_Button* button, unsigned long state) {}
}//namespace


Widget_Button::Widget_Button(ButtonAnimation* animation, Widget* parent)
: Widget(parent)
, m_animation(animation)
{
    setSize(m_animation->size());
    onStateChanged(nullptr);
}


Widget_Button::Widget_Button(ButtonAnimation* animation, bool own_animation, Widget* parent)
: Widget(parent)
, m_animation(animation)
{
    setSize(m_animation->size());
    if(own_animation)
        m_flags |= R64FX_WIDGET_OWNS_ANIMATION;
    else
        m_flags &= ~R64FX_WIDGET_OWNS_ANIMATION;
    onStateChanged(nullptr);
}


Widget_Button::~Widget_Button()
{
    if(ownsAnimation())
        delete m_animation;
}


ButtonAnimation* Widget_Button::animation() const
{
    return m_animation;
}


bool Widget_Button::ownsAnimation() const
{
    return m_flags & R64FX_WIDGET_OWNS_ANIMATION;
}


bool Widget_Button::isPressed() const
{
    return m_state & 1;
}


void Widget_Button::setFrame(unsigned long frame)
{
    m_state = (m_state & 1) | (frame << 1);
}


bool Widget_Button::frame() const
{
    return m_state >> 1;
}


void Widget_Button::onStateChanged(void (*on_state_changed)(void* arg, Widget_Button* button, unsigned long state), void* arg)
{
    if(on_state_changed)
    {
        m_on_state_changed = on_state_changed;
    }
    else
    {
        m_on_state_changed = on_state_changed_stub;
    }
    m_on_state_changed_arg = arg;
}


void Widget_Button::paintEvent(PaintEvent* event)
{
    auto p = event->painter();
    Image frame;
    m_animation->pickFrame(&frame, m_state);
    p->putImage(&frame, {0, 0});
}


void Widget_Button::mousePressEvent(MousePressEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        grabMouse();
        m_state |= 1;
        m_on_state_changed(m_on_state_changed_arg, this, m_state);
        repaint();
    }
}


void Widget_Button::mouseReleaseEvent(MouseReleaseEvent* event)
{
    if(event->button() == MouseButton::Left())
    {
        if(isMouseGrabber())
            ungrabMouse();
        m_state &= ~1;
        m_on_state_changed(m_on_state_changed_arg, this, m_state);
        repaint();
    }
}


ButtonAnimation::ButtonAnimation(Size<int> size, int frame_count)
: m_size(size)
, m_frame_count(frame_count)
{
    m_data = new(std::nothrow) unsigned char[frame_count * size.width() * size.height() * 4];
}


ButtonAnimation::~ButtonAnimation()
{
    if(m_data)
    {
        delete[] m_data;
    }
}


Size<int> ButtonAnimation::size() const
{
    return m_size;
}


int ButtonAnimation::width() const
{
    return m_size.width();
}


int ButtonAnimation::height() const
{
    return m_size.height();
}


unsigned long ButtonAnimation::frameCount() const
{
    return m_frame_count;
}


bool ButtonAnimation::isGood() const
{
    return m_data != nullptr;
}


void ButtonAnimation::pickFrame(Image* img, int frame_num)
{
    img->load(width(), height(), 4, m_data + (frame_num * width() * height() * 4));
}


void generate_masks(Size<int> size, Image* bg, Image* depressed, Image* pressed)
{
    bg->load(size.width(), size.height(), 1);
    {
        fill(bg, Color(0));
        fill_rounded_rect(bg, Color(255), {0, 0, size.width(), size.height()}, 4);
    }

    depressed->load(size.width(), size.height(), 1);
    {
        fill(depressed, Color(0));
        fill_rounded_rect(depressed, Color(255), {1, 1, size.width() - 2, size.height() - 2}, 4);
    }

    pressed->load(size.width(), size.height(), 1);
    {
        fill(pressed, Color(0));
        fill_rounded_rect(pressed, Color(255), {2, 2, size.width() - 4, size.height() - 4}, 4);
    }
}


ButtonAnimation* ButtonAnimation::CenteredImageMask(Size<int> size, Image* mask)
{
    ButtonAnimation* anim = new(std::nothrow) ButtonAnimation(size, 2);
    if(!anim)
    {
        return nullptr;
    }

    if(!anim->isGood())
    {
        delete anim;
        return nullptr;
    }

    Image bg, depressed, pressed;
    generate_masks(size, &bg, &depressed, &pressed);

    unsigned char black[4] = {0, 0, 0, 0};
    unsigned char c0[4] = {127, 127, 127, 0};

    unsigned char bg_depressed [4] = {200, 200, 200, 0};
    unsigned char bg_pressed   [4] = {150, 150, 150, 0};
    unsigned char fg_depressed [4] = {100, 100, 100, 0};
    unsigned char fg_pressed   [4] = { 50,  50,  50, 0};

    /* Depressed */
    {
        Image img;
        anim->pickFrame(&img, 0);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_depressed), &depressed);
        blend(
            &img,
            Point<int>(img.width()/2 - mask->width()/2, img.height()/2 - mask->height()/2),
            Colors(fg_depressed),
            mask
        );
    }

    /* Pressed */
    {
        Image img;
        anim->pickFrame(&img, 1);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_pressed), &pressed);
        blend(
            &img,
            Point<int>(img.width()/2 - mask->width()/2, img.height()/2 - mask->height()/2),
            Colors(fg_pressed),
            mask
        );
    }

    return anim;
}


ButtonAnimation* ButtonAnimation::Text(const std::string &text, Font* font)
{
    if(text.empty())
        return nullptr;

    Image textimg;
    text2image(text, TextWrap::None, font, &textimg);

    return ButtonAnimation::CenteredImageMask({textimg.width() + 4, textimg.height() + 4}, &textimg);
}


ButtonAnimation* ButtonAnimation::Text(Size<int> size, const std::string &text, Font* font)
{
    if(text.empty())
        return nullptr;

    Image textimg;
    text2image(text, TextWrap::None, font, &textimg);

    return ButtonAnimation::CenteredImageMask(size, &textimg);
}


ButtonAnimation* ButtonAnimation::Colored(Size<int> size, unsigned char** rgbas, int num_rgbas)
{
    ButtonAnimation* anim = new(std::nothrow) ButtonAnimation(size, num_rgbas * 2);
    if(!anim)
    {
        return nullptr;
    }

    if(!anim->isGood())
    {
        delete anim;
        return nullptr;
    }

    unsigned char black[4] = {0, 0, 0, 0};

    Image bg, depressed, pressed;
    generate_masks(size, &bg, &depressed, &pressed);

    for(unsigned long i=0; i<anim->frameCount()/2; i++)
    {
        /* Depressed */
        {
            Image img;
            anim->pickFrame(&img, i*2);
            blend(&img, Point<int>(0, 0), Colors(black), &bg);
            blend(&img, Point<int>(0, 0), rgbas + i, &depressed);
        }

        /* Pressed */
        {
            Image img;
            anim->pickFrame(&img, i*2 + 1);
            blend(&img, Point<int>(0, 0), Colors(black), &bg);
            blend(&img, Point<int>(0, 0), rgbas + i, &pressed);
        }
    }

    return anim;
}


enum PlayPauseStates{
    PlayDepressed   = 0,
    PlayPressed     = 1,
    PauseDepressed  = 2,
    PausePressed    = 3
};


ButtonAnimation* ButtonAnimation::PlayPause(Size<int> size)
{
    ButtonAnimation* anim = new(std::nothrow) ButtonAnimation(size, 4);
    if(!anim)
    {
        return nullptr;
    }

    if(!anim->isGood())
    {
        delete anim;
        return nullptr;
    }

    unsigned char black[4] = {0, 0, 0, 0};
    unsigned char c0[4] = {127, 127, 127, 0};

    unsigned char bg_depressed [4] = {200, 200, 200, 0};
    unsigned char bg_pressed   [4] = {150, 150, 150, 0};
    unsigned char fg_depressed [4] = {100, 100, 100, 0};
    unsigned char fg_pressed   [4] = { 50,  50,  50, 0};

    Image bg, depressed, pressed;
    generate_masks(size, &bg, &depressed, &pressed);

    Image triangle(size.width()/2 + 1, size.height()/2 + 1, 1);
    draw_triangles(size.width()/2 + 1, nullptr, nullptr, nullptr, &triangle);

    Image bars(size.width()/2 + 1 , size.height()/2 + 1, 1);
    {
        fill(&bars, Color(0));
        int w = bars.width() / 3;
        int h = bars.height();
        fill(&bars, 0, 255, {0,   0, w, h});
        fill(&bars, 0, 0,   {w,   0, w, h});
        fill(&bars, 0, 255, {w*2, 0, w, h});
    }

    /* 0 Play Depressed */
    {
        Image img;
        anim->pickFrame(&img, PlayDepressed);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_depressed), &depressed);
        blend(&img, Point<int>(size.width()/4 - size.width()/20, size.height()/4), Colors(fg_depressed), &triangle);
    }

    /* 1 Play Pressed */
    {
        Image img;
        anim->pickFrame(&img, PlayPressed);
        fill(&img, c0);
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_pressed), &pressed);
        blend(&img, Point<int>(size.width()/4 - size.width()/20, size.height()/4), Colors(fg_pressed), &triangle);
    }

    /* 2 Pause Depressed */
    {
        Image img;
        anim->pickFrame(&img, PauseDepressed);
        fill(&img, Color(127, 127, 127, 0));
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_depressed), &depressed);
        blend(&img, Point<int>(
            img.width() / 2 - bars.width() / 2,
            img.height() / 2 - bars.height() / 2
        ), Colors(fg_depressed), &bars);
    }

    /* 3 Pause Pressed */
    {
        Image img;
        anim->pickFrame(&img, PausePressed);
        fill(&img, Color(127, 127, 127, 0));
        blend(&img, Point<int>(0, 0), Colors(black), &bg);
        blend(&img, Point<int>(0, 0), Colors(bg_pressed), &pressed);
        blend(&img, Point<int>(
            img.width() / 2 - bars.width() / 2,
            img.height() / 2 - bars.height() / 2
        ), Colors(fg_pressed), &bars);
    }

    return anim;
}


unsigned long ButtonAnimation::PlayFrame()
{
    return 0;
}


unsigned long ButtonAnimation::PauseFrame()
{
    return 1;
}

}//namespace r64fx