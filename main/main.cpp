#include <iostream>
#include <map>
#include <unistd.h>
#include <cstring>
#include <jack/jack.h>

#ifdef USE_SDL2
#include "gui/SDL2Window.h"
#else
#error "No valid Window implementation present!"
#endif//USE_SDL2

#include "gui/Translation.h"
#include "gui/Icon.h"
#include "gui/Dummy.h"
#include "gui/TextEdit.h"
#include "gui/Keyboard.h"
#include "gui/Font.h"
#include "gc/gc.h"
#include "json/Json.h"
#include "Machine.h"
#include "gui/TextureAtlas.h"

#include "gui/geometry_io.h"

#include "knobs.h"
#include "sockets_and_wires.h"

using namespace std;

#include "serialize.cxx"

namespace r64fx{

    
#ifdef USE_SDL2
typedef SDL2Window Window_t;
#endif//USE_SDL2


string data_prefix;

    
/* Default font is built into the binary. 
 * Just in case the data paths are really messed up
 * and we want to render text.
 */    

const vector<unsigned char> jura_book_font = {
    #include "jura_book_font.ttf.h"
};


/** For widgets that render shadows and stuff.
 *  Import it with extern.
 *  Use only in the main module. 
 */
float light_angle = 0.75 * M_PI;


/** @brief Main program class. 
 
    Just to avoid doing a lot of function forward-decalarations.
 */
struct Program{
    jack_client_t* jack_client;
    jack_status_t jack_status;
    bool jack_thread_should_be_running;
    
    int main_thread(int argc, char* argv[])
    {
        using namespace r64fx;
        
        /* Set data paths */
        if(argc < 2)
        {
            cerr << "Give me a path to r64fx data directory!\n";
            return 1;
        }
            
        r64fx::data_prefix = argv[1];
        
        if(data_prefix.back() != '/')
            data_prefix.push_back('/');
        
        if(argc > 2)
            cerr << "Warning: ignoring extra " << (argc-2) << " command line arguments!\n";
        
        /*
         * Main window opened by default. 
         * This creates an OpenGL context.
         * 
         * Most of the things in the gui can be done only after this step.
         * This is true for everything that has to do with rendering or texture loading.
         */
        if(!SDL2Window::init())
        {
            cerr << "Failed to init GUI!\n";
            return 2;
        }
        
        Window_t window(800, 600, "r64fx");

        window.makeCurrent();
        
        /* Can be done only after we have obtained the OpenGL context. */
        if(!Window::initGlew())
            return 3;
        
        Texture::init();
        
        Keyboard::init();
        
        Wire::init();
        
        /* Initialize default font. */
        Font font(jura_book_font.data(), jura_book_font.size());
        font.setFaceSize(16);
        if(font.isOk())
        {
            Font::initDefaultFont(&font);
        }
        else
        {
            cerr << "Problem creating default font!\n";
            abort();
        }

        /* Set the UI language. */
        tr.loadLanguage("ru");
        
        /* removeme. */
        Dummy::initDebugMenu();
        
        /* These two scenes show the machines. */
        FrontMachineScene fms;
        BackMachineScene bms;
        
        /* Allows us to flip between front and back machine_scenes in a view. */
        fms.counterpart_scene = &bms;
        bms.counterpart_scene = &fms;
        
        vector<Wire*> wires;
        fms.wires = &wires;
        bms.wires = &wires;
        
        /* removeme */
        Machine* m1 = new Machine(&fms, &bms);
        m1->setPosition(100, 100);
        Machine* m2 = new Machine(&fms, &bms);
        m2->setPosition(100, 300);
        Machine* m3 = new Machine(&fms, &bms);
        m3->setPosition(100, 500);

        TexturedKnobBackground txkbg("textures/knob_bg.png");
        KnobHandleTypeA::init();
        KnobHandleTypeA khta;
        
        auto k1 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k1->setPosition(50, 20);
        k1->update();
        m3->front()->appendWidget(k1);
      
        auto k2 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k2->setPosition(250, 20);
        k2->update();
        m3->front()->appendWidget(k2);
        
        auto k3 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k3->setPosition(400, 20);
        k3->update();
        m3->front()->appendWidget(k3);
        
        auto sa = new Socket;
        sa->setPosition(10, 10);
        sa->update();
        sa->is_plugged = true;
        m2->back()->appendWidget(sa);
        
        auto sb = new Socket;
        sb->setPosition(400, 10);
        sb->update();
        sb->is_plugged = true;
        m3->back()->appendWidget(sb);
        
        Wire* wire = new Wire;
        wire->setA(sa);
        wire->setB(sb);
        wire->update();
        wire->color = { 0.7, 0.7, 0.1, 0.0 };
        wires.push_back(wire);
        
        /* Setup View icons. */
        View::split_view_vert_icon = Icon({24, 24}, "textures/split_vertically.png");
        View::split_view_hor_icon  = Icon({24, 24}, "textures/split_horizontally.png");
        View::close_view_icon      = Icon({24, 24}, "textures/close_view.png");
        
        /* Setup root view of the main window. */        
        View* view = new View(&fms);
        window.setView(view);

        
        int max_texture_size;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        
//         start_jack_thread();
        
        glEnable(GL_MULTISAMPLE_ARB);
        
        /* Main event loop. */
        int gc_counter = 256;
        for(;;)
        {
            Window_t::processEvents();

            //Process other stuff here.

            window.render();            
            window.swapBuffers();
            
            if(!gc_counter)
            {
                gc_counter = 256;
                gc::deleteGarbage();
            }
            else
            {
                gc_counter--;
            }
            if(Window_t::shouldQuit()) break;
            usleep(300);
        }
        
//         stop_jack_thread();

        /* Cleanup things up before exiting. */
        Texture::cleanup();
        Window_t::cleanup();
        
        return 0;
    }


    void jack_thread()
    {
        static int i = 0;
        while(jack_thread_should_be_running)
        {
            cout << i++ << "\n";
            auto nframes = jack_cycle_wait(jack_client);
            //Do IO.
            jack_cycle_signal(jack_client, 0);
            //Do processing.
        }
    }
    

    void start_jack_thread()
    {
        jack_thread_should_be_running = true;
        jack_set_process_thread(jack_client, [](void* data)->void*{
            auto program = (Program*) data;
            program->jack_thread();
            return nullptr;
        }, this);
        
        jack_activate(jack_client);
    }


    void stop_jack_thread()
    {
        jack_deactivate(jack_client);
        jack_thread_should_be_running = false;
    }
    
} program;

}//namespace r64fx


int main(int argc, char* argv[])
{
    return r64fx::program.main_thread(argc, argv);
}