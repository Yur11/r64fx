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

#include "gui/RenderingContext.h"
#include "gui/Translation.h"
#include "gui/Icon.h"
#include "gui/Dummy.h"
#include "gui/TextEdit.h"
#include "gui/Keyboard.h"
#include "gui/RectPainter.h"
#include "gui/Font.h"
#include "gc/gc.h"
#include "gui/TextureAtlas.h"
#include "gui/geometry_io.h"
#include "gui/HorizontalMenu.h"

#include "dsp/Processor.h"
#include "SawtoothOsc.h"
#include "MonophonicMixer.h"

#include "json/Json.h"
#include "Machine.h"
#include "knobs.h"
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
        
        auto window = Window_t::create(800, 600, "r64fx");
        window->makeCurrent();
        
        /* Can be done only after we have obtained the OpenGL context. */
        if(!Window::initGlew())
            return 3;

        Keyboard::init();
        
        Texture::init();

        RectPainter::init();
        
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

        TexturedKnobBackground txkbg(data_prefix + "textures/knob_bg.png");
        KnobHandleTypeA::init();
        KnobHandleTypeA khta;
        
        auto k1 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k1->setPosition(50, 20);
        k1->min_value = 0.1;
        k1->max_value = 880.0;
        k1->update();
        m3->front()->appendWidget(k1);
      
        auto k2 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k2->setPosition(160, 20);
        k2->update();
        m3->front()->appendWidget(k2);
        
        auto k3 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k3->setPosition(280, 20);
        k3->min_value = 0.1;
        k3->max_value = 880.0;
        k3->update();
        m3->front()->appendWidget(k3);
        
        auto k4 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k4->setPosition(390, 20);
        k4->update();
        m3->front()->appendWidget(k4);
        
        auto k5 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k5->setPosition(510, 20);
        k5->min_value = 0.1;
        k5->max_value = 880.0;
        k5->update();
        m3->front()->appendWidget(k5);
      
        auto k6 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k6->setPosition(620, 20);
        k6->update();
        m3->front()->appendWidget(k6);
        
        auto k7 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k7->setPosition(740, 20);
        k7->min_value = 0.01;
        k7->max_value = 100.0;
        k7->update();
        m3->front()->appendWidget(k7);
        
        auto k8 = new Knob<TexturedKnobBackground, KnobHandleTypeA>(&txkbg, &khta);
        k8->setPosition(850, 20);
        k8->update();
        m3->front()->appendWidget(k8);
        
        auto sa = new Socket;
        sa->setPosition(10, 10);
        sa->update();
        m2->addSocket(sa);
        
        auto sb = new Socket;
        sb->setPosition(400, 10);
        sb->update();
        m3->addSocket(sb);
        
//         Wire* wire = new Wire(sa, sb);
//         wire->update();
//         wire->color = { 0.7, 0.7, 0.1, 0.0 };
//         wires.push_back(wire);
        
        /* Setup View icons. */
        View::split_view_vert_icon = Icon({24, 24}, data_prefix + "textures/split_vertically.png");
        View::split_view_hor_icon  = Icon({24, 24}, data_prefix + "textures/split_horizontally.png");
        View::close_view_icon      = Icon({24, 24}, data_prefix + "textures/close_view.png");
        
        /* Setup root view of the main window. */        
        View* view = new View(&fms);
        window->setView(view);
        
        
        HorizontalMenu hm(debug_font);
        hm.setPosition(0, 0);
        fms.appendWidget(&hm);
        
        Menu session_menu(debug_font);
        Menu edit_menu(debug_font);
        Menu view_menu(debug_font);
        Menu help_menu(debug_font);

        auto hello_act = new Action("Hello", {[](void* source, void* data)->void*{
            auto act = (Action*) source;
            cout << act->name().stdstr << "\n";
            return nullptr;
        }, this});
        
        auto doctor_act = new Action("Doctor", {[](void* source, void* data)->void*{
            auto act = (Action*) source;
            cout << act->name().stdstr << "\n";
            return nullptr;
        }, this});
        
        auto name_act = new Action("Name", {[](void* source, void* data)->void*{
            auto act = (Action*) source;
            cout << act->name().stdstr << "\n";
            return nullptr;
        }, this});
        
        auto continue_act = new Action("Continue", {[](void* source, void* data)->void*{
            auto act = (Action*) source;
            cout << act->name().stdstr << "\n";
            return nullptr;
        }, this});
        
        auto yesterday_act = new Action("Yesterday", {[](void* source, void* data)->void*{
            auto act = (Action*) source;
            cout << act->name().stdstr << "\n";
            return nullptr;
        }, this});
        
        auto tommorow_act = new Action("Tommorow", {[](void* source, void* data)->void*{
            auto act = (Action*) source;
            cout << act->name().stdstr << "\n";
            return nullptr;
        }, this});
        
        session_menu.appendAction(hello_act);
        session_menu.appendAction(doctor_act);
        session_menu.appendAction(name_act);
        session_menu.appendAction(continue_act);
        session_menu.appendAction(yesterday_act);
        session_menu.appendAction(tommorow_act);
        session_menu.update();
        
        hm.appendMenu("Session", &session_menu);
        hm.appendMenu("Edit", &edit_menu);
        hm.appendMenu("View", &view_menu);
        hm.appendMenu("Help", &help_menu);
        hm.update();
                
//         auto window2 = SDL2Window::create(640, 480, "window2");
//         Scene scene2;
//         View* view2 = new View(&fms);
//         window2->setView(view2);
//         
//         auto window3 = SDL2Window::create(640, 480, "window3");
//         window3->makeCurrent();
//         Scene scene3;
//         View* view3 = new View(&bms);
//         window3->setView(view3);

//         
//         int max_texture_size;
//         glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
        
//         jack_status_t jack_status;
//         jack_client = jack_client_open("r64fx", JackNullOption, &jack_status);
//         if(!jack_client)
//         {
//             cerr << "Failed to create jack client!\n";
//             abort();
//         }
//         
//         input_port = jack_port_register(jack_client, "in", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//         if(!input_port)
//         {
//             cerr << "Failed to create input port!\n";
//             abort();
//         }
//         
//         output_port = jack_port_register(jack_client, "out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//         if(!output_port)
//         {
//             cerr << "Failed to create output port!\n";
//             abort();
//         }
//         
//         midi_input_port = jack_port_register(jack_client, "midi_in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
//         if(!midi_input_port)
//         {
//             cerr << "Failed to create midi input port!\n";
//             abort();
//         }
        
//         audio_buffer_size = jack_get_buffer_size(jack_client);
//         
//         input_buffer = new float[audio_buffer_size];
//         output_buffer = new float[audio_buffer_size];
//         for(auto i=0; i<audio_buffer_size; i++)
//         {
//             input_buffer[i] = output_buffer[i] = 0.0;
//         }
//         
//         jack_set_process_thread(jack_client, [](void* data)->void*{
//             auto program = (Program*) data;
//             program->jack_thread();
//             return nullptr;
//         }, this);
//         
        
//         SawtoothOsc osc;
//         auto osc_slot1 = osc.getSlot();
//         assert(osc_slot1 != nullptr);
//         auto osc_slot2 = osc.getSlot();
//         assert(osc_slot2 != nullptr);
//         auto osc_slot3 = osc.getSlot();
//         assert(osc_slot3 != nullptr);
//         auto osc_slot4 = osc.getSlot();
//         assert(osc_slot4 != nullptr);
//         
//         freq1_addr = osc.frequencyPort()->at(osc_slot1);
//         freq2_addr = osc.frequencyPort()->at(osc_slot2);
//         freq3_addr = osc.frequencyPort()->at(osc_slot3);
//         freq4_addr = osc.frequencyPort()->at(osc_slot4);
        
//         MonophonicMixer mix;
//         auto mix_slot1 = mix.getSlot();
//         assert(mix_slot1 != nullptr);
//         auto mix_slot2 = mix.getSlot();
//         assert(mix_slot2 != nullptr);
//         auto mix_slot3 = mix.getSlot();
//         assert(mix_slot3 != nullptr);
//         auto mix_slot4 = mix.getSlot();
//         assert(mix_slot4 != nullptr);
//         
//         level1_addr = mix.levelPort()->at(mix_slot1);
//         level2_addr = mix.levelPort()->at(mix_slot2);
//         level3_addr = mix.levelPort()->at(mix_slot3);
//         level4_addr = mix.levelPort()->at(mix_slot4);
//         
//         k1->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* freq = (float*) data;
//             float f = knob->value();
//             *freq = f;
//             return nullptr;
//         }, freq1_addr);
//         
//         k2->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* level = (float*) data;
//             float l = knob->value();
//             *level = l;
//             return nullptr;
//         }, level1_addr);
//         
//         k3->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* freq = (float*) data;
//             float f = knob->value();
//             *freq = f;
//             return nullptr;
//         }, freq2_addr);
//         
//         k4->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* level = (float*) data;
//             float l = knob->value();
//             *level = l;
//             return nullptr;
//         }, level2_addr);
//         
//         k5->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* freq = (float*) data;
//             float f = knob->value();
//             *freq = f;
//             return nullptr;
//         }, freq3_addr);
//         
//         k6->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* level = (float*) data;
//             float l = knob->value();
//             *level = l;
//             return nullptr;
//         }, level3_addr);
//         
//         k7->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* freq = (float*) data;
//             float f = knob->value();
//             *freq = f;
//             return nullptr;
//         }, freq4_addr);
//         
//         k8->value_changed = Message([](void* source, void* data) -> void*{
//             BasicKnob* knob = (BasicKnob*) source;
//             float* level = (float*) data;
//             float l = knob->value();
//             *level = l;
//             return nullptr;
//         }, level4_addr);
//         
//         auto c1 = make_connection(osc.normalizedPort(), osc_slot1, mix.inputPort(), mix_slot1);
//         auto c2 = make_connection(osc.normalizedPort(), osc_slot2, mix.inputPort(), mix_slot2);
//         auto c3 = make_connection(osc.normalizedPort(), osc_slot3, mix.inputPort(), mix_slot3);
//         auto c4 = make_connection(osc.valuePort(), osc_slot4, osc.fmPort(), osc_slot1);
//         auto c5 = make_connection(osc.valuePort(), osc_slot4, osc.fmPort(), osc_slot2);
//         auto c6 = make_connection(osc.valuePort(), osc_slot4, osc.fmPort(), osc_slot3);
//         
//         graph->processors.push_back(&osc);
//         graph->processors.push_back(&mix);
//         
//         graph->writeToBuffer(output_buffer, mix.mix());
//         
//         graph->render(audio_buffer_size);
//         
//         cout << graph->debug() << "\n";

//         jack_activate(jack_client);

        glEnable(GL_MULTISAMPLE);

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
        
//         jack_deactivate(jack_client);
        
//         delete[] input_buffer;
//         delete[] output_buffer;
// 
//         while(!cycle_waiting)
//             usleep(100);

        /* Cleanup things up before exiting. */
//         jack_client_close(jack_client);
//         delete graph;

        RectPainter::cleanup();
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
        }
    }
    
} program;

}//namespace r64fx


int main(int argc, char* argv[])
{
    return r64fx::program.main_thread(argc, argv);
}