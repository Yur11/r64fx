#include "Widget_Dummy.hpp"
#include "Painter.hpp"
#include "Mouse.hpp"
#include "ImageUtils.hpp"

#include <iostream>
using namespace std;

namespace r64fx{

Widget_Dummy::Widget_Dummy(Widget* parent)
: Widget(parent)
{
    changeColor();
}


Widget_Dummy::~Widget_Dummy()
{

}


void Widget_Dummy::paintEvent(WidgetPaintEvent* event)
{
    auto p = event->painter();
    p->fillRect({0, 0, width(), height()}, m_color);
    if(m_img.isGood())
    {
//         p->blendColors({0, 0}, Colors(Color(0, 0, 0, 0)), &m_img);
    }

    childrenPaintEvent(event);
}


struct PlotPoint{
    float minval   = 0.0f;
    float maxval   = 0.0f;
    float minblend = 0.0f;
    float maxblend = 0.0f;
};


void Widget_Dummy::resizeEvent(WidgetResizeEvent* event)
{
    m_img.load(width(), height(), 1);
    fill(&m_img, Color(0));

    vector<float> vec(width());

    float width_rcp = 1.0f / float(width() - 1);
    for(int i=0; i<width(); i++)
    {
        float val = sin(i * width_rcp * 8.0f * M_PI) * 0.7 * float(height()/2) + float(height()/2);
        vec[i] = val;
    }

    stroke_plot(&m_img, m_color, {0, 0, width(), height()}, vec.data(), 3, 0.5, height() * 0.25);

//     /* Find extrema. */
//     struct Extremum{
//         int index = 0;
//         bool is_minimum = true;
//
//         Extremum(int index, bool is_minimum) : index(index), is_minimum(is_minimum) {}
//     };
//
//     vector<Extremum> extrema;
//     extrema.push_back({0, true});
//     for(int i=0; i<width(); i++)
//     {
//         if(i > 0 && vec[i - 1] >= vec[i] && (i + 1) < width() && vec[i] <= vec[i + 1])
//         {
//             extrema.push_back({i, true});
//         }
//         else if(i > 0 && vec[i - 1] <= vec[i] && (i + 1) < width() && vec[i] >= vec[i + 1])
//         {
//             extrema.push_back({i, false});
//         }
//     }
//     if(extrema.size() >= 2)
//     {
//         extrema[0].is_minimum = !extrema[1].is_minimum;
//     }
//     extrema.push_back({int(extrema.size()) - 1, !extrema.back().is_minimum});
//
//
//     /* Generate line. */
//     vector<PlotPoint> pvec(width());
//     int curr_extremum = 0;
//     float minimum = 0.0f;
//     float maximum = 0.0f;
//     for(int i=0; i<width(); i++)
//     {
//         if(i == extrema[curr_extremum].index)
//         {
//             if(extrema[curr_extremum].is_minimum)
//             {
//                 minimum = vec[extrema[curr_extremum + 0].index] - half_thickness;
//                 maximum = vec[extrema[curr_extremum + 1].index] + half_thickness;
//             }
//             else
//             {
//                 minimum = vec[extrema[curr_extremum + 1].index] - half_thickness;
//                 maximum = vec[extrema[curr_extremum + 0].index] + half_thickness;
//             }
//             curr_extremum++;
//         }
//
//         float dy1;
//         float extra1;
//         {
//             dy1 = 0.0f;
//             if(i > 0)
//             {
//                 dy1 = vec[i] - vec[i - 1];
//             }
//             extra1 = sqrt(dy1 * dy1 + 1) * half_thickness;
//         }
//
//         float dy2;
//         float extra2;
//         {
//             dy2 = 0.0f;
//             if((i + 1) < width())
//             {
//                 dy2 = vec[i + 1] - vec[i];
//             }
//             extra2 = sqrt(dy2 * dy2 + 1) * half_thickness;
//         }
//
//         PlotPoint pp;
//         if(dy1 < 0)
//         {
//             if(dy2 < 0)
//             {
//                 pp.minval = vec[i] - extra2;
//                 pp.maxval = vec[i] + extra1;
//             }
//             else
//             {
//                 pp.minval = vec[i] - (extra1 + extra2) * 0.5f;
//                 pp.maxval = vec[i] + (extra1 + extra2) * 0.5f;
//             }
//         }
//         else
//         {
//             if(dy2 >=0)
//             {
//                 pp.minval = vec[i] - extra1;
//                 pp.maxval = vec[i] + extra2;
//             }
//             else
//             {
//                 pp.minval = vec[i] - (extra1 + extra2) * 0.5f;
//                 pp.maxval = vec[i] + (extra1 + extra2) * 0.5f;
//             }
//         }
//
//         pp.minval = int(max(pp.minval, minimum));
//         pp.maxval = int(min(pp.maxval, maximum));
//         pvec[i] = pp;
//     }
//
//     for(int i=0; i<width(); i++)
//     {
//         pvec[i].minblend = pvec[i].minval;
//         pvec[i].maxblend = pvec[i].maxval;
//     }
//
//
//     /* Blend top part. */
//     int segment_length = 0;
//     int curr_y = pvec[0].minval;
//     int prev_y = curr_y - 1;
//     for(int i=0; i<width(); i++)
//     {
//         segment_length++;
//         if((i + 1) == width() || int(pvec[i].minval) != int(pvec[i + 1].minval))
//         {
//             int curr_y = pvec[i].minval;
//
//             int next_y;
//             if((i + 1) < width())
//             {
//                 next_y = pvec[i + 1].minval;
//             }
//             else
//             {
//                 next_y = curr_y - 1;
//             }
//
//             int prev_h = curr_y - prev_y;
//             int next_h = curr_y - next_y;
//
//             if(segment_length > 1)
//             {
//                 float rcp = 1.0f / float(segment_length + 1);
//
//                 if(next_y < curr_y)
//                 {
//                     if(prev_y > curr_y)
//                     {
//                         for(int j=0; j<segment_length; j++)
//                         {
//                             int x = j + i - segment_length + 1;
//                             pvec[x].minblend = curr_y - (j + 1) * rcp;
//                         }
//                     }
//                     else
//                     {
//                         if(segment_length >= 3)
//                         {
//                             for(int j=0; j<(segment_length>>1); j++)
//                             {
//                                 int x1 = j + i - segment_length + 1;
//                                 int x2 = i - j;
//                                 pvec[x1].minblend = pvec[x2].minblend
//                                     = curr_y - (segment_length - j) * rcp;
//                             }
//
//                             if(segment_length & 1)
//                             {
//                                 int x = i - (segment_length >> 1);
//                                 pvec[x].minblend = pvec[x + 1].minblend;
//                             }
//                         }
//                         else
//                         {
//                             for(int j=0; j<segment_length; j++)
//                             {
//                                 int x = j + i - segment_length + 1;
//                                 pvec[x].minblend = curr_y - 0.5f;
//                             }
//                         }
//                     }
//                 }
//                 else if(prev_y < curr_y)
//                 {
//                     for(int j=0; j<segment_length; j++)
//                     {
//                         int x = j + i - segment_length + 1;
//                         pvec[x].minblend = curr_y - (segment_length - j) * rcp;
//                     }
//                 }
//             }
//             else // segment_length == 1
//             {
//                 if(next_y < curr_y)
//                 {
//                     if(prev_y > curr_y)
//                     {
//                         if((curr_y - next_y) == 1)
//                         {
//                             pvec[i].minblend = curr_y - 0.5f;
//                         }
//                         else
//                         {
//                             pvec[i].minblend = next_y;
//                         }
//                     }
//                 }
//                 else if(prev_y < curr_y)
//                 {
//                     if((curr_y - prev_y) == 1)
//                     {
//                         pvec[i].minblend = curr_y - 0.5f;
//                     }
//                     else
//                     {
//                         pvec[i].minblend = prev_y;
//                     }
//                 }
//             }
//
//             segment_length = 0;
//             prev_y = curr_y;
//         }
//     }
//
//
//     /* Blend bottom part. */
//     segment_length = 0;
//     curr_y = pvec[0].minval;
//     prev_y = curr_y - 1;
//     for(int i=0; i<width(); i++)
//     {
//         segment_length++;
//         if((i + 1) == width() || int(pvec[i].maxval) != int(pvec[i + 1].maxval))
//         {
//             int curr_y = pvec[i].maxval;
//
//             int next_y;
//             if((i + 1) < width())
//             {
//                 next_y = pvec[i + 1].maxval;
//             }
//             else
//             {
//                 next_y = curr_y - 1;
//             }
//
//             int prev_h = curr_y - prev_y;
//             int next_h = curr_y - next_y;
//
//             if(segment_length > 1)
//             {
//                 float rcp = 1.0f / (segment_length + 1);
//
//                 if(next_y > curr_y)
//                 {
//                     if(prev_y < curr_y)
//                     {
//                         for(int j=0; j<segment_length; j++)
//                         {
//                             int x = j + i - segment_length + 1;
//                             pvec[x].maxblend = curr_y + (j + 1) * rcp;
//                         }
//                     }
//                     else
//                     {
//                         if(segment_length >= 3)
//                         {
//                             for(int j=0; j<(segment_length>>1); j++)
//                             {
//                                 int x1 = j + i - segment_length + 1;
//                                 int x2 = i - j;
//                                 pvec[x1].maxblend = pvec[x2].maxblend
//                                     = curr_y + (segment_length - j) * rcp;
//                             }
//
//                             if(segment_length & 1)
//                             {
//                                 int x = i - (segment_length >> 1);
//                                 pvec[x].maxblend = pvec[x + 1].maxblend;
//                             }
//                         }
//                         else
//                         {
//                             for(int j=0; j<segment_length; j++)
//                             {
//                                 int x = j + i - segment_length + 1;
//                                 pvec[x].maxblend = curr_y + 0.5f;
//                             }
//                         }
//                     }
//                 }
//                 else if(prev_y > curr_y)
//                 {
//                     for(int j=0; j<segment_length; j++)
//                     {
//                         int x = j + i - segment_length + 1;
//                         pvec[x].maxblend = curr_y + (segment_length - j) * rcp;
//                     }
//                 }
//             }
//             else // segment_length == 1
//             {
//                 if(next_y > curr_y)
//                 {
//                     if(prev_y < curr_y)
//                     {
//                         if((next_y - curr_y) == 1)
//                         {
//                             pvec[i].maxblend = curr_y + 0.5f;
//                         }
//                         else
//                         {
//                             pvec[i].maxblend = next_y;
//                         }
//                     }
//                 }
//                 else if(prev_y > curr_y)
//                 {
//                     if((prev_y - curr_y) == 1)
//                     {
//                         pvec[i].maxblend = curr_y + 0.5f;
//                     }
//                     else
//                     {
//                         pvec[i].maxblend = prev_y;
//                     }
//                 }
//             }
//
//             segment_length = 0;
//             prev_y = curr_y;
//         }
//     }
//
//
//     for(int x=0; x<width(); x++)
//     {
//         auto &pp = pvec[x];
//         for(int y=0; y<height(); y++)
//         {
//             if(y < pp.minval)
//             {
//                 float diff = pp.minval - pp.minblend;
//                 if(diff > 0.0f)
//                 {
//                     if(diff <= 1.0f)
//                     {
//                         if((y + 1) == pp.minval)
//                         {
//                             m_img(x, y)[0] = (unsigned char)(255.0f * diff);
//                         }
//                     }
//                     else
//                     {
//                         float rcp = 1.0f / (diff + 1);
//                         float val = 1.0f - (pp.minblend - y + diff) * rcp;
//                         if(val > 0.0f && val < 1.0f)
//                         {
//                             m_img(x, y)[0] = (unsigned char)(255.0f * val);
//                         }
//                     }
//                 }
//             }
//             else if(y <= pp.maxval)
//             {
//                 m_img(x, y)[0] = 255;
//             }
//             else
//             {
//                 float diff = pp.maxblend - pp.maxval;
//                 if(diff >= 0.0f)
//                 {
//                     if(diff <= 1.0f)
//                     {
//                         if((y - 1) == pp.maxval)
//                         {
//                             m_img(x, y)[0] = (unsigned char)(255.0f * diff);
//                         }
//                     }
//                     else
//                     {
//                         float rcp = 1.0f / (diff + 1);
//                         float val = 1.0f - (y - pp.maxblend + diff) * rcp;
//                         if(val > 0.0f && val < 1.0f)
//                         {
//                             m_img(x, y)[0] = (unsigned char)(255.0f * val);
//                         }
//                     }
//                 }
//             }
//         }
//     }
}


void Widget_Dummy::mousePressEvent(MousePressEvent* event)
{
    changeColor();
    repaint();
}


void Widget_Dummy::mouseReleaseEvent(MouseReleaseEvent* event)
{

}


void Widget_Dummy::mouseMoveEvent(MouseMoveEvent* event)
{

}


void Widget_Dummy::keyPressEvent(KeyPressEvent* event)
{

}


void Widget_Dummy::keyReleaseEvent(KeyReleaseEvent* event)
{

}


void Widget_Dummy::closeEvent()
{
    cout << "Close!\n";
}


void Widget_Dummy::changeColor()
{
    for(int i=0; i<3; i++)
    {
        m_color[i] = (rand() % 127) + 128;
    }
}

}//namespace r64fx
