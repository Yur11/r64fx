#include "Program.h"
#include "gui/WindowImplementation.h"
#include "DenseWaveformPainter.h"

#include <iostream>
#include <unistd.h>

using namespace std;

namespace r64fx{
    

Program::Program(int argc, char* argv[])
{
    if(!initData(argc, argv))
    {
        _status = 1; 
        return;
    }
    
    if(!initGui())
    {
        _status = 2; 
        return;
    }
    
    initScenes();
}


Program::~Program()
{
    
}

    
void Program::mainThread()
{
    if(_status != 0)
        return;
    
    
    while(Window::count() > 0)
    {       
        Window::mainSequence();
        Program::gcSequence();
        
        usleep(300);
    }
}


void Program::quit()
{
    cerr << "We should really tell the program to quit now!\n";
}


bool Program::initData(int argc, char* argv[])
{
//     if(argc < 2)
//     {
//         cerr << "Give me a path to r64fx data directory!\n";
//         return false;
//     }
//         
//     _data_prefix = argv[1];
//     
//     if(_data_prefix.back() != '/')
//         _data_prefix.push_back('/');
//     
//     if(argc > 2)
//         cerr << "Warning: ignoring extra " << (argc-2) << " command line arguments!\n";
    
    return true;
}


bool Program::initGui()
{
    if(!init_window_implementation())
        return false;
    
    auto window = create_window(800, 600, "r64fx");
    if(!window)
    {
        cerr << "Failed to create a window!\n";
        return false;
    }
    
    window->makeCurrent();
    
    if(!Window::initGlew())
        return false;
    
    SplitView::init();
    
    Mouse::init();
    
    Keyboard::init();
    
    Texture::init();

    if(!Painter::init())
    {
        cerr << "Failed to init Painter!\n";
        return false;
    }
    
    if(!DenseWaveformPainter::init())
    {
        cerr << "Failed to init DenseWaveformPainter!\n";
        return false;
    }
    
    Icon::init();
    
    Socket::init();
    
    Wire::init();
    
    Font::init();
    
    auto font = new Font("./data/fonts/FreeSans.ttf", 20);
    if(!font->isOk())
    {
        delete font;
        return false;
    }
    
    Font::setDefaultFont(font);

    _infs = new InformationScene;
    
    _fms = new FrontMachineScene;
    _bms = new BackMachineScene;
    _fms->counterpart_scene = _bms;
    _bms->counterpart_scene = _fms;
    _fms->wires = &_wires;
    _bms->wires = &_wires;
    
    auto root_view = new View(_fms);
    window->setView(root_view);
    
    return true;
}


void Program::initScenes()
{
    
}


void Program::gcSequence()
{
    if(!_gc_counter)
    {
        _gc_counter = 256;
        gc::deleteGarbage();
    }
    else
    {
        _gc_counter--;
    }
}

bool Program::initJackClient()
{
    _jack_client = jack_client_open("r64fx", JackNullOption, nullptr);
    if(!_jack_client)
    {
        cerr << "Failed to init jack client!\n";
        return false;
    }
    
    int code = jack_set_process_thread(_jack_client, [](void* arg)->void*{ 
        auto* program = (Program*) arg;
        program->jackThread();
        return nullptr;
    }, this);
    
    if(code != 0)
    {
        cerr << "Failed to set jack thread callback!\n";
        return false;
    }
        
    return true;
}


void Program::jackThread()
{
    for(;;)
    {
        /*auto nframes = */jack_cycle_wait(_jack_client);
        
        //Do IO
        
        jack_cycle_signal(_jack_client, 0);
        
        //Do processing
    }
    
    //How do we exit this thing properly?
}
    
}//namespace r64fx