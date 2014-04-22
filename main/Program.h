#ifndef R64FX_MAIN_PROGRAM_H
#define R64FX_MAIN_PROGRAM_H

#include <string>
#include <vector>
#include <jack/jack.h>

#include "InformationScene.h"
#include "gui/Action.h"
#include "gui/gl.h"

namespace r64fx{
    
class Program{
    std::string _data_prefix;
    
    int _status = 0;
    
    int _gc_counter = 256;
    
    Action* _hello_act;
    Action* _doctor_act;
    Action* _name_act;
    Action* _continue_act;
    Action* _yesterday_act;
    Action* _tommorow_act;
    
    jack_client_t* _jack_client;
    
    /** @brief Parse command-line arguments and initialize data paths. */
    bool initData(int argc, char* argv[]);
    
    /** @brief Initialize gui functionality including OpenGL context. */
    bool initGui();
    
    /** */
    void initTextures();
    
    void initCommonTexture2D(std::string name, GLenum internal_format = GL_RGBA8, int expected_chan_count = 4);
            
    void initActions();
        
    /** @brief Run garbage collector if needed. */
    void gcSequence();

    /**  */
    bool initJackClient();
    
    /** */
    void jackThread();
    
public:
    Program(int argc, char* argv[]);
    
    virtual ~Program();
    
    void mainThread();

    /** @brief Return status of the program. */
    inline int status() const { return _status; }
    
    /** @brief Tell the program to quit. */
    void quit();
};
    
}//namespace r64fx

#endif//R64FX_MAIN_PROGRAM_H