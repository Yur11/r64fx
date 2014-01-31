#ifndef R64FX_GUI_TEXT_EDIT_H
#define R64FX_GUI_TEXT_EDIT_H

#include "TextLine.h"
#include <iostream>

namespace r64fx{
    
/** @brief An editable line of text. */
class TextEdit : public TextLine{
    int cursor_position = 0;
    Widget* prev_keyboard_grabber = nullptr;
    Widget* prev_mouse_grabber = nullptr;
    bool left_down = false;
    Color background_color;
    
    
    inline void move_cursor_forward()
    {
        if(cursor_position < (int)text.size()) cursor_position++;
    }
    
    inline void move_cursor_back()
    {
        if(cursor_position > 0) cursor_position--;
    }

    inline void home_cursor()
    {
        cursor_position = 0;
    }
    
    inline void end_cursor()
    {
        cursor_position = text.size();
    }
    
    inline void erase_forward()
    {
        if(cursor_position < (int)text.size())
        {
            text.erase(cursor_position, 1);
        }
    }
    
    
    inline void erase_backwards()
    {
        if(cursor_position > 0)
        {
            text.erase(cursor_position - 1, 1);
            cursor_position--;
        }
    }

    
    /* Text selection. */
    class{
        int _index_a = 0; //Char index where drag has started.
        int _index_b = 0; //Char index where drag has ended.
        
        float _selection_a = 0.0;
        float _selection_b = 0.0;
        
        bool _drag_active = false;
        bool _drag_occured = false;
        
    public:
        inline void drag_start(int index, float x)
        {
            _drag_active = true;
            _index_a = index;
            _selection_a = x;
        }
        
        inline void drag(int index, float x)
        {
            if(index != _index_b) _selection_b = x;
            _index_b = index;
            _drag_occured = true;
        }
        
        inline void drag_end()
        {
            _drag_active = false;
            if(!drag_occured()) clear();
        }
        
        inline int left_index()  { return _index_a < _index_b ? _index_a : _index_b; }
        inline int right_index() { return _index_a > _index_b ? _index_a : _index_b; }
        
        inline float left()  { return _selection_a < _selection_b ? _selection_a : _selection_b; } 
        inline float right() { return _selection_a > _selection_b ? _selection_a : _selection_b; } 
        
        inline void clear() { _index_a = 0; _index_b = 0; _drag_occured = false; _drag_active = false; }
        
        inline bool index_in_selection(int index) { return (index >= left_index() && index <= right_index()); }
        
        inline bool drag_is_actve() const { return _drag_active; }
        
        inline bool drag_occured() { return _drag_occured; }
        
    } text_selection;
    
public:    
    TextEdit(Utf8String inital_text, Font* font, Widget* parent = nullptr) 
    : TextLine(inital_text, font, parent)
    , background_color(1.0, 1.0, 1.0)
    {
    }
    
    inline void setBackgroundColor(Color color) { background_color = color; }
    
    inline void setBackgroundColor(float r, float g, float b) { background_color = Color(r, g, b); }
    
    inline Color backgroundColor() const { return background_color; }
    
    virtual void render(RenderingContextId_t context_id);
    
    virtual void mousePressEvent(MouseEvent* event);
    
    virtual void mouseReleaseEvent(MouseEvent* event);
    
    virtual void mouseMoveEvent(MouseEvent* event);
    
    virtual void keyPressEvent(KeyEvent* event);
    
    virtual void textInputEvent(Utf8String text);
    
    /** @brief Char index at x coord. Starting with 1. */
    int charAt(float x);
};
    
}//namespace r64fx

#endif//R64FX_GUI_TEXT_EDIT_H

