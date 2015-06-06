#ifndef R64FX_GUI_KEY_EVENT_H
#define R64FX_GUI_KEY_EVENT_H


namespace r64fx{
    
class KeyEvent{
    unsigned int m_key;

public:
    KeyEvent(unsigned int key) 
    : m_key(key)
    {}
        
    inline unsigned int key() const { return m_key; }
};
    
}//namespace r64fx

#endif//R64FX_GUI_KEY_EVENT_H