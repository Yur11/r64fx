#include <iostream>
#include <map>
#include <unistd.h>
#include <cstring>
#include <jack/jack.h>

#include "Config.h"
#include "Window.h"
#include "Icon.h"
#include "Dummy.h"
#include "TextEdit.h"
#include "Keyboard.h"
#include "Font.h"
#include "gc.h"
#include "Json.h"
#include "Machine.h"

using namespace std;

#include "data_paths.cpp"
#include "filesystem.cpp"
#include "translator.cpp"
#include "serialize.cpp"

namespace r64fx{
#include "jura_book_font.cpp"
}//namespace r64fx


struct Program{
    jack_client_t* jack_client;
    jack_status_t jack_status;
    bool jack_thread_should_be_running;
    
    
    int main_thread()
    {
        using namespace r64fx;
        
//         jack_client = jack_client_open("r64fx", JackNullOption, &jack_status);
//         if(!jack_client)
//         {
//             cerr << "Failed to create jack client!\n";
//             return 1;
//         }
//         
        
        if(!Window::init())
        {
            cerr << "Failed to init GUI!\n";
            return 2;
        }
        
        /*
        * Main window opened by default. 
        * This creates an OpenGL context.
        * 
        * A lot of things in the gui can be done only after this step.
        * This is true for everything that has to do with rendering or texture loading.
        */
        Window window(800, 600, "r64fx");
        window.makeCurrent();

        /*  */
        Keyboard::init();

        /* Initialize default texture and data paths for texture file lookup. */
        Texture::init(&data_paths);

        /* Initialize default font. */
        Font font(jura_book_font, jura_book_font_size);
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

        tr.loadLanguage("en");

        Dummy::initDebugMenu();
        
        FrontMachineScene fms;
        BackMachineScene bms;
        
        /* Allow to flip between front and back machine_scenes in a view. */
        fms.counterpart_scene = &bms;
        bms.counterpart_scene = &fms;
        
        Machine* m = new Machine(&fms, &bms);

        /* Setup root view of the main window. */        
        View* view = new View(&fms);
        window.setView(view);

        
//         start_jack_thread();
        
        /* Main event loop. */
        int gc_counter = 256;
        for(;;)
        {
            Window::processEvents();

            //Process other stuff here.

            window.render();
            window.swapBuffers();
            glFinish();
            if(!gc_counter)
            {
                gc_counter = 256;
                gc::deleteGarbage();
            }
            else
            {
                gc_counter--;
            }
            if(Window::shouldQuit()) break;
            usleep(300);
        }
        
//         stop_jack_thread();

        /* Cleanup things up before exiting. */
        Texture::cleanup();
        Window::cleanup();
        
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


int main()
{
    return program.main_thread();
}