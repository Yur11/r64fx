#ifndef R64FX_WIDGET_TIMELINE_HPP
#define R64FX_WIDGET_TIMELINE_HPP

#include "Widget.hpp"
#include "Model.hpp"

namespace r64fx{

class Widget_Sequencer : public Widget{
    Sequencer* m_sequencer = nullptr;

    void* m_impl = nullptr;

    double m_pixel2beat = 1.0;
    double m_beat2pixel = 1.0;

    long m_scroll_vert = 0;
    long m_scroll_hori = 0;

public:
    Widget_Sequencer(Widget* parent = nullptr);

    ~Widget_Sequencer();

    inline void setSequencer(Sequencer* sequencer)
        { m_sequencer = sequencer; }

    inline Sequencer* sequencer() const
        { return m_sequencer; }

    inline void setPixelsPerBeat(int ppb)
        { m_beat2pixel = double(ppb); m_pixel2beat = 1.0 / m_beat2pixel;  }

    inline int pixelsPerBeat() const { return m_beat2pixel; }

    inline void setScrollVert(long s) { m_scroll_vert = s; }
    inline void setScrollHori(long s) { m_scroll_hori = s; }

    inline long scrollVert() const { return m_scroll_vert; }
    inline long scrollHori() const { return m_scroll_hori; }


private:
    virtual void paintEvent(WidgetPaintEvent* event);

    virtual void resizeEvent(WidgetResizeEvent* event);
};

}//namespace r64fx

#endif//R64FX_WIDGET_TIMELINE_HPP
