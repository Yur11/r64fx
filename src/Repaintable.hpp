#ifndef R64FX_GUI_REPAINTABLE_H
#define R64FX_GUI_REPAINTABLE_H

namespace r64fx{
    
/** @brief Usefull mixin for things that may be asked to get repainted. */
struct Repaintable{
    bool needs_repainting = false;
    
    inline void getRepainted() { needs_repainting = true; }    
};
    
}//namespace r64fx

#endif//R64FX_GUI_REPAINTABLE_H

