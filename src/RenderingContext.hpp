#ifndef R64FX_GUI_RENDERING_CONTEXT_H
#define R64FX_GUI_RENDERING_CONTEXT_H

#include <vector>

namespace r64fx{
   
typedef int RenderingContextId_t;
    
const RenderingContextId_t max_rendering_context_count = 4;

const RenderingContextId_t BadRenderingContextId = -1;

class RenderingContextAware;

/** @brief Base class for rendering contexts.
 *  
 *  Each window has it's own rendering context 
 *  that encapsulates all the things that are specific to that context
 *  and can't be shared with other contexts.
 *  I.e. opengl vertex array objects.
 * 
 *  Each context has an integer id with a value in the range 0 .. max_rendering_context_count-1.
 * 
 *  Inherit from this class and reimplement the makeCurrent() method.
 *  You must use the getFreeId() method to get an id for the new instance.
 */
class RenderingContext{
    friend class RenderingContextAware;
    
    static RenderingContext* all_rendering_contexts[max_rendering_context_count];
    static RenderingContext* current_context;
    RenderingContextId_t _id;
    
    std::vector<RenderingContextAware*> new_items;    
    std::vector<RenderingContextAware*> active_items;
    std::vector<RenderingContextAware*> discarded_items;
    
    static void registerItem(RenderingContextAware* item);
    
    static void discardItem(RenderingContextAware* item);
    
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
        
    /** @brief Make this context current. 
     
        Reimplementations must explicitly call this base class method!
     */
    virtual void makeCurrent();
        
    /** @brief Currently bound context. */
    inline static RenderingContext* current() { return current_context; }
    
    /** @brief Perform opengl context specific operation for this contexts. 
     
        This includes vertex array object allocations and deallocations.
        makeCurrent() must be called pror to calling this function.
     */
    void update();
    
    /** @brief */
    void cleanup();
        
protected:
    virtual void beforeDestruction();
};


/** @brief Base class for all objects aware of the rendering context. 
 
    All RenderingContextAwares instances must be allocated on the heap
    and shall be discarded by calling discard().
    Never delete them directly.
 */
class RenderingContextAware : public Disposable{
    friend class RenderingContext;
   
protected:
    virtual void setupForContext(RenderingContextId_t context_id) = 0;
    
    virtual void cleanupForContext(RenderingContextId_t context_id) = 0;
    
public:
    RenderingContextAware();
    
    virtual ~RenderingContextAware() {};

    void discard();
};

    
}//namespace r64fx

#endif//R64FX_GUI_RENDERING_CONTEXT_H