#ifndef R64FX_GUI_KEY_EVENT_H
#define R64FX_GUI_KEY_EVENT_H


namespace r64fx{
    
class KeyEvent{
    int m_key;

public:
    KeyEvent(int key)
    : m_key(key)
    {}
        
    inline int key() const { return m_key; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_KEY_EVENT_H