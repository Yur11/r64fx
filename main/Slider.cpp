#include "Slider.hpp"
#include "gui/MouseEvent.hpp"

#include <iostream>
using namespace std;

namespace r64fx{
 
float Slider::coord2value(float coord)
{
    coord += coord2value_offset;
    coord *= coord2value_coeff;
    
    if(coord < 0.0)
    {
        coord = 0.0;
    }
    else if(coord > 1.0)
    {
       coord = 1.0;
    }
    
    return coord;
}


float Slider::pos2value(Point<float> p)
{
    switch(orientation)
    {
        case Orientation::Vertical:
            return coord2value(p.y);
        break;
        
        case Orientation::Horizontal:
            return coord2value(p.x);
        break;
    }
    
    return 0.0;
}
    
    
Slider::Slider(Texture2D* backround_tex, Texture2D* handle_tex, Widget* parent)
: Widget(parent)
, p(8)
, backround_tex(backround_tex)
, handle_tex(handle_tex)
{
}

    
void Slider::appearanceChangeEvent()
{
    if(value < 0.0)
    {
        value = 0.0;
    }
    else if(value > 1.0)
    {
       value = 1.0;
    }
    
    auto r = projectedRect();
    
    float data[32];
    
    data[0] = r.left;
    data[1] = r.top;
    
    data[2] = r.right;
    data[3] = r.top;
    
    data[4] = r.left;
    data[5] = r.bottom;
    
    data[6] = r.right;
    data[7] = r.bottom;
    
    
    float handle_width = r.width() * handle_rect_w_ratio;
    float handle_height = r.height() * handle_rect_h_ratio;
    float handle_half_width = handle_width * 0.5;
    float handle_half_height = handle_height * 0.5;
    
    switch(orientation)
    {
        case Orientation::Vertical:
        {
            float handle_left  = r.center().x - handle_half_width; 
            float handle_right = r.center().x + handle_half_width;
            float handle_min_y = r.top + handle_half_height;
            float handle_max_y = r.bottom - handle_half_height;
            float movement_height= handle_max_y - handle_min_y;
            float handle_center_y = movement_height * value + handle_min_y;
            float handle_top = handle_center_y - handle_half_height;
            float handle_bottom = handle_center_y + handle_half_height;

            coord2value_coeff = 1.0 / movement_height;
            coord2value_offset = -handle_min_y;
            
            /* Handle positions */
            data[8]  = handle_left;
            data[9]  = handle_top;
            
            data[10] = handle_right;
            data[11] = handle_top;
            
            data[12] = handle_left;
            data[13] = handle_bottom;
            
            data[14] = handle_right;
            data[15] = handle_bottom;
            
            
            /* ============== Tex coords ============ */
            /* Background */
            data[16] = 0.0;
            data[17] = 0.0;

            data[18] = 1.0;
            data[19] = 0.0;

            data[20] = 0.0;
            data[21] = 1.0;

            data[22] = 1.0;
            data[23] = 1.0;


            /* Handle */
            data[24] = 0.0;
            data[25] = 0.0;

            data[26] = 1.0;
            data[27] = 0.0;

            data[28] = 0.0;
            data[29] = 1.0;

            data[30] = 1.0;
            data[31] = 1.0;
            
            break;
        }
        
        case Orientation::Horizontal:
        {
            float handle_top    = r.center().y - handle_half_height;
            float handle_bottom = r.center().y + handle_half_height;
            float handle_min_x  = r.left + handle_half_width;
            float handle_max_x  = r.right - handle_half_width;
            float movement_width = handle_max_x - handle_min_x;
            float handle_center_x = movement_width * value + handle_min_x;
            float handle_left  = handle_center_x - handle_half_width;
            float handle_right = handle_center_x + handle_half_width;
            
            coord2value_coeff = 1.0 / movement_width;
            coord2value_offset = -handle_min_x;
            
            
            /* Handle positions */
            data[8]  = handle_left;
            data[9]  = handle_top;
            
            data[10] = handle_right;
            data[11] = handle_top;
            
            data[12] = handle_left;
            data[13] = handle_bottom;
            
            data[14] = handle_right;
            data[15] = handle_bottom;
            
            
            /* ============== Tex coords ============ */
            /* Background */
            data[16] = 0.0;
            data[17] = 0.0;
            
            data[18] = 0.0;
            data[19] = 1.0;
            
            data[20] = 1.0;
            data[21] = 0.0;
            
            data[22] = 1.0;
            data[23] = 1.0;
            
            
            /* Handle */
            data[24] = 0.0;
            data[25] = 0.0;
            
            data[26] = 0.0;
            data[27] = 1.0;
            
            data[28] = 1.0;
            data[29] = 0.0;
            
            data[30] = 1.0;
            data[31] = 1.0;
            
            break;
        }
    }
    
    p.bindBuffer();
    p.setData(data);
    p.unbindBuffer();
}


void Slider::render()
{
    p.bindArray();
    
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    p.setTexture(backround_tex->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 0);
   
    Painter::setColor(1.0, 1.0, 1.0, 1.0);
    p.setTexture(handle_tex->glName());
    p.render(GL_TRIANGLE_STRIP, 4, 4);
    
    p.unbindArray();
}


void Slider::mousePressEvent(MouseEvent* event)
{
    prev_value = value;
    value = pos2value(event->position());
    if(prev_value != value)
    {
        cout << value << "\n";
    }
    mouse_is_down = true;
    grabMouseInput();
    appearanceChangeEvent();
    event->has_been_handled = true;
}


void Slider::mouseReleaseEvent(MouseEvent* event)
{
    ungrabMouseInput();
    mouse_is_down = false;
    event->has_been_handled = true;
}


void Slider::mouseMoveEvent(MouseEvent* event)
{
    if(mouse_is_down)
    {
        prev_value = value;
        value = pos2value(event->position());
        if(prev_value != value)
        {
            cout << value << "\n";
        }
    }
    appearanceChangeEvent();
    event->has_been_handled = true;
}
    
}//namespace r64fx