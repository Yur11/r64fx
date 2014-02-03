#ifndef R64FX_GUI_RENDERING_CONTEXT_H
#define R64FX_GUI_RENDERING_CONTEXT_H

#include "shared_sources/LinkedItem.h"

namespace r64fx{
   
typedef int RenderingContextId_t;
    
const RenderingContextId_t max_rendering_context_count = 4;

const RenderingContextId_t BadRenderingContextId = -1;


/** @brief Base class for rendering contexts.
 *  
 *  Each window has it's own rendering context 
 *  that encapsulates all the things that are specific to that context
 *  and can't be shared with other contexts.
 *  I.e. opengl vertex array objects.
 * 
 *  Each context has an integer id with a value in the range 0 .. max_rendering_context_count-1.
 *  
 *  When rendering for a specific window, and id of that window's context shall be passed dowm the rendering tree
 *  to tell the RenderingContextAware objects which vbo-s to use.
 * 
 *  Inherit from this class and reimplement the makeCurrent() method.
 *  You must use the getFreeId() method to get an id for the new instance.
 */
class RenderingContext{
    static RenderingContext* all_rendering_contexts[max_rendering_context_count];
    RenderingContextId_t _id;
    
public:
    /** @brief Mark all instances as being non setup. 
     
        Call this once at the start of the program.
     */
    static void init();
    
    /** @brief Get a free id for the new rendering context. 
     
        Use to construct a new RenderingContext instance.
     
        Returns BadRenderingContextId if no free ids are avilable.
     */
    static RenderingContextId_t getFreeId();
    
    /** @brief Construct a new instances using a given id. 
     
        The id shall be obtained by calling getFreeId() method.
     */
    RenderingContext(RenderingContextId_t id);

    virtual ~RenderingContext();
    
    /** @brief Get an instance by it's id. */
    inline static RenderingContext* context(RenderingContextId_t id) { return all_rendering_contexts[id]; }
    
    inline RenderingContextId_t id() const { return _id; };
        
    /** @brief Make this context current. */
    virtual void makeCurrent() = 0;
    
    /** @brief Teach all RenderingContextAware objects about this context. */
    void setup();
    
    /** @brief Make all RenderingContextAware object forgets about this context. */
    void cleanup();    
};


/** @brief Base class for all objects aware of the rendering context. */
class RenderingContextAware : public LinkedItem<RenderingContextAware>{
    friend class RenderingContext;
    static RenderingContextAware* first_item;
    
    bool is_setup_for[max_rendering_context_count];
   
protected:
    virtual void setupForContext(RenderingContextId_t context_id) = 0;
    
    virtual void cleanupForContext(RenderingContextId_t context_id) = 0;
    
public:
    RenderingContextAware();
    
    virtual ~RenderingContextAware();
};

    
}//namespace r64fx

#endif//R64FX_GUI_RENDERING_CONTEXT_H