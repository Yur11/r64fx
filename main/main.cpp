#include <iostream>
#include <map>
#include <unistd.h>
#include <cstring>
#include <jack/jack.h>
#include <jack/midiport.h>

#ifdef USE_SDL2
#include "gui/SDL2Window.h"
#else
#error "No valid Window implementation present!"
#endif//USE_SDL2

#include "gui/Error.h"
#include "gui/RenderingContext.h"
#include "gui/Translation.h"
#include "gui/Icon.h"
#include "gui/Dummy.h"
#include "gui/TextEdit.h"
#include "gui/Keyboard.h"
#include "gui/Painter.h"
#include "gui/Font.h"
#include "gc/gc.h"
#include "gui/TextureAtlas.h"
#include "gui/geometry_io.h"
#include "gui/HorizontalMenu.h"
#include "gui/Dummy.h"

#include "dsp/Processor.h"
#include "SawtoothOsc.h"
#include "MonophonicMixer.h"

#include "json/Json.h"
#include "Machine.h"
#include "Knob.h"
#include "Wire.h"

using namespace std;

#include "serialize.cxx"

namespace r64fx{

    
#ifdef USE_SDL2
typedef SDL2Window Window_t;
#endif//USE_SDL2


string data_prefix;

/** Removeme */
Font* debug_font = nullptr;

/** @brief Main program class. 
 
    Just to avoid doing a lot of function forward-decalarations.
 */
struct Program{
    jack_client_t* jack_client;
    bool cycle_waiting;
    int audio_buffer_size;

    /* removethis */
    jack_port_t* input_port;
    jack_port_t* output_port;
    float* input_buffer;
    float* output_buffer;
    jack_port_t* midi_input_port;
    float* freq1_addr;
    float* freq2_addr;
    float* freq3_addr;
    float* freq4_addr;
    
    float* level1_addr;
    float* level2_addr;
    float* level3_addr;
    float* level4_addr;
    
    Graph* graph;
    
    
    float num1 = 1.0;
    float num2 = 1.0;
    
    
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
         */
        if(!SDL2Window::init())
        {
            cerr << "Failed to init GUI!\n";
            return 2;
        }
        
        auto window = Window_t::create(800, 600, "r64fx");
        window->makeCurrent();
        
        /* Can be done only after we have obtained the OpenGL context. */
        if(!Window::initGlew())
            return 3;
        
        SplitView::init();
        
        Mouse::init();
        
        Keyboard::init();
        
        Texture::init();
        
        if(!Painter::init())
        {
            cerr << "Failed to init painter!\n";
            return 4;
        }
        
        Icon::init();
        
        Socket::init();
        
        Wire::init();
        
        Font::init();
        debug_font = new Font(data_prefix + "fonts/FreeSans.ttf", 13);
        assert(debug_font->isOk());

        /* Set the UI language. */
        tr.loadLanguage("en");
        
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
        
        graph = new Graph;
        
        /* removeme */
        Machine* m1 = new Machine(&fms, &bms);
        m1->setPosition(100, 100);
        Machine* m2 = new Machine(&fms, &bms);
        m2->setPosition(100, 300);
        Machine* m3 = new Machine(&fms, &bms);
        m3->setPosition(100, 500);
        
        Texture fg(data_prefix + "textures/knob_bg.png");
        Texture bg(data_prefix + "textures/knob_a.png");
        Texture shiny(data_prefix + "textures/brushed_metal_knob_top.png");
        
        auto k1 = new ShinyKnob(fg, bg, shiny);
        k1->setPosition(50, 20);
        k1->min_value = 0.1;
        k1->max_value = 880.0;
        k1->update();
        m3->front()->appendWidget(k1);
      
        auto sa = new Socket;
        sa->setPosition(10, 10);
        sa->update();
        m2->addSocket(sa);
        
        auto sb = new Socket;
        sb->setPosition(400, 10);
        sb->update();
        m3->addSocket(sb);
        
        Wire* wire = new Wire(sa, sb);
        wire->update();
        wire->color = { 0.7, 0.7, 0.1, 0.0 };
        wires.push_back(wire);
        
        /* Setup View icons. */
        View::split_view_vert_icon = Icon({24, 24}, data_prefix + "textures/split_vertically.png");
        View::split_view_hor_icon  = Icon({24, 24}, data_prefix + "textures/split_horizontally.png");
        View::close_view_icon      = Icon({24, 24}, data_prefix + "textures/close_view.png");
        
        /* Setup root view of the main window. */        
        View* view = new View(&fms);
        window->setView(view);
        
        glEnable(GL_MULTISAMPLE);
        CHECK_FOR_GL_ERRORS;

        Painter::enable();

        /* Main event loop. */
        int gc_counter = 256;
        while(Window::count() > 0)
        {       
            Window::mainSequence();
            
            if(!gc_counter)
            {
                gc_counter = 256;
                gc::deleteGarbage();
            }
            else
            {
                gc_counter--;
            }
            
            usleep(300);
        }

        Mouse::cleanup();
        Icon::cleanup();
        Texture::cleanup();
        Window_t::cleanup();
        
        gc::deleteGarbage();
        
        return 0;
    }


    void jack_thread()
    {
        static int i = 0;
        for(;;)
        {
//             cout << i << "\n";
            i++;
            cycle_waiting = true;
            auto nframes = jack_cycle_wait(jack_client);
            cycle_waiting = false;
//             cout << "_\n";
            
            auto jack_input_buffer = jack_port_get_buffer(input_port, audio_buffer_size);
            auto jack_output_buffer = jack_port_get_buffer(output_port, audio_buffer_size);
            
            memcpy(input_buffer, jack_input_buffer, sizeof(float) * audio_buffer_size);
            memcpy(jack_output_buffer, output_buffer, sizeof(float) * audio_buffer_size);
            
            /* Should we use audio_buffer_size for midi too? */
            auto midi_input_buffer = jack_port_get_buffer(midi_input_port, audio_buffer_size);
            auto event_count = jack_midi_get_event_count(midi_input_buffer);
            for(int i=0; i<(int)event_count; i++)
            {
                jack_midi_event_t event;
                jack_midi_event_get(&event, midi_input_buffer, 0);
                
                union{
                    unsigned char value;
                    struct{
                        unsigned char high:4;
                        unsigned char low:4;
                    } half;
                } byte;
                
                if(event.size == 3)
                {
                    byte.value = event.buffer[0];
                    
                    if(byte.half.low == b1001)
                    {
                        cout << "Note On: Chan:  " << (int)byte.half.high << "\n";
                        unsigned char key = event.buffer[1];
                        unsigned char vel = event.buffer[2];
                        cout << "key: " << (int)key << ", vel: " << (int)vel << "\n";
                        float freq = 440.0 * pow(2, float(key - 69) / 12);
                        cout << "freq: " << freq << "\n";
                    }
                    else if(byte.half.low == b1000)
                    {
                        cout << "Note Off: Chan: " << (int)byte.half.high << "\n";
                        unsigned char key = event.buffer[1];
                        unsigned char vel = event.buffer[2];
                        cout << "key: " << (int)key << ", vel: " << (int)vel << "\n";
                    }
                    else if(byte.half.low == b1011)
                    {
                        cout << "Control Change: Chan: " << (int)byte.half.high << "\n";
                        unsigned char key = event.buffer[1];
                        unsigned char vel = event.buffer[2];
                        cout << "key: " << (int)key << ", vel: " << (int)vel << "\n";
                    }
                    else if(byte.half.low == b1010)
                    {
                        cout << "Polyphonic Aftertouch: Chan: " << (int)byte.half.high << "\n";
                        unsigned char key = event.buffer[1];
                        unsigned char val = event.buffer[2];
                        cout << "key: " << (int)key << ", val: " << (int)val << "\n";
                    }
                    else if(byte.half.low == b1110)
                    {
                        cout << "Pitch Wheel: Chan: " << (int)byte.half.high << "\n";
                        unsigned int low = event.buffer[1] & b01111111;
                        unsigned int high  = event.buffer[2] & b01111111;
                        int val = (high << 7) + low - 8192;
                    }
                }
                else if(event.size == 2)
                {
                    byte.value = event.buffer[0];
                    
                    if(byte.half.low == b1101)
                    {
                        cout << "Channel Aftertouch: Chan: " << (int)byte.half.high << "\n";
                        unsigned char val = event.buffer[1];
                        cout << "val: " << (int)val << "\n";
                    }
                    
                }
            }
            
            jack_cycle_signal(jack_client, 0);

            graph->run();
            
//             for(int i=0; i<1000*500; i++)
//             {
//                 num1 += num2;
//                 num1 *= 0.5;
//                 num1 *= 0.3;
//             }
        }
    }
    
} program;

}//namespace r64fx


int main(int argc, char* argv[])
{
    return r64fx::program.main_thread(argc, argv);
}