#include "Program.h"
#include "gui/Translation.h"
#include "gui/WindowImplementation.h"
#include "DenseWaveformPainter.h"

#include "gui/Dummy.h"
#include "gui/Keyboard.h"
#include "gui/Font.h"
#include "gui/Icon.h"

#include <iostream>
#include <unistd.h>

using namespace std;

namespace r64fx{
    
    
bool Program::initData(int argc, char* argv[])
{
    if(argc < 2)
    {
        cerr << "Give me a path to r64fx data directory!\n";
        return false;
    }
        
    _data_prefix = argv[1];
    
    if(_data_prefix.back() != '/')
        _data_prefix.push_back('/');
    
    if(argc > 2)
        cerr << "Warning: ignoring extra " << (argc-2) << " command line arguments!\n";
    
    return true;
}


bool Program::initGui()
{
    tr.loadLanguage(_data_prefix + "translations/en/");
    
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
    
//     SplitView::init();
    
    Mouse::init();
    
    Keyboard::init();
    
    if(!Painter::init())
    {
        cerr << "Failed to init Painter!\n";
        return false;
    }
    
//     if(!DenseWaveformPainter::init())
//     {
//         cerr << "Failed to init DenseWaveformPainter!\n";
//         return false;
//     }

    Font::init();
    
    auto font = new Font( _data_prefix + "fonts/FreeSans.ttf", 10);
    if(!font->isOk())
    {
        delete font;
        return false;
    }
    
    Font::setDefaultFont(font);
        
    initTextures();
    
    Icon::default_size = { 14, 14 };
    Icon::init();
    
    initActions();
    
    Dummy* dummy = new Dummy(10.0, 10.0);
    
    window->setRootWidget(dummy);

//     _infs = new InformationScene;
//     for(auto act : {_hello_act, _doctor_act, _name_act, _continue_act, _yesterday_act, _tommorow_act})
//     {
//         auto aw = new ActionWidget(act);
//         aw->showing_icon = false;
//         aw->update();
//         _infs->panel->appendWidget(aw);
//     }
    
//     _infs->panel->update();
//     
//     _fms = new FrontMachineScene;
//     _bms = new BackMachineScene;
//     _fms->counterpart_scene = _bms;
//     _bms->counterpart_scene = _fms;
//     _fms->wires = &_wires;
//     _bms->wires = &_wires;
//     
//     auto root_view = new View(_infs);
//     window->setView(root_view);
//     
//     root_view->split_vert_act = _split_view_vert_act;
//     root_view->split_hor_act = _split_view_hor_act;
//     root_view->close_act = _close_view_act;
//     
//     root_view->updateContextMenu();
    
    return true;
}
    

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
}


Program::~Program()
{
    
}

    
void Program::mainThread()
{
    if(_status != 0)
        return;

//     gl::Enable(GL_SCISSOR_TEST);
//     gl::Enable(GL_BLEND);
//     gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.78, 0.78, 0.78, 1.0);
    
    Painter::enable();
    Painter::setTexturingMode(Painter::RGBA);
    
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


void Program::initTextures()
{
    initCommonTexture2D("close_view");
    initCommonTexture2D("split_horizontally");
    initCommonTexture2D("split_vertically");
}


void Program::initCommonTexture2D(std::string name, GLenum internal_format, int expected_chan_count)
{
    GLenum format;
    if(expected_chan_count == 4)
        format = GL_RGBA;
    else if(expected_chan_count == 3)
        format = GL_RGB;
    else if(expected_chan_count == 1)
        format = GL_RED;
    else
    {
        cerr << "Program::initCommonTexture2D(): Bad channel count !\n" << expected_chan_count << "\n";
        return;
    }
        
    auto tex = Texture2D::loadMipmaps(_data_prefix + "textures/" + name, internal_format, format);
    if(!tex)
    {
        cerr << "Failed to load texture " << name << " !\n";
        return;
    }
    
    Texture::addCommonTexture(name, tex);
}


void Program::initActions()
{
    _hello_act = new Action(
        "hello",
        "Hello",
        [](void*) -> void*
        {
            cout << "Hello\n";
            return nullptr;
        }
    );
    
    _doctor_act = new Action(
        "doctor",
        "Doctor",
        [](void*) -> void*
        {
            cout << "Doctor\n";
            return nullptr;
        }
    );
    
    _name_act = new Action(
        "name",
        "Name",
        [](void*) -> void*
        {
            cout << "Name\n";
            return nullptr;
        }
    );
    
    _continue_act = new Action(
        "continue",
        "Continue",
        [](void*) -> void*
        {
            cout << "Continue\n";
            return nullptr;
        }
    );
    
    _yesterday_act = new Action(
        "yesterday",
        "Yesterday",
        [](void*) -> void*
        {
            cout << "Yesterday\n";
            return nullptr;
        }
    );
    
    _tommorow_act = new Action(
        "tommorow",
        "Tommorow",
        [](void*) -> void*
        {
            cout << "Tommorow\n";
            return nullptr;
        }
    );
    
//     _split_view_vert_act = new Action(
//         "split_vertically",
//         tr("split_vertically"),
//         [](void*) -> void*
//         {
//             View::splitViewVertically(View::activeView());
//             return nullptr;
//         }
//     );
//     
//     _split_view_hor_act = new Action(
//         "split_horizontally",
//         tr("split_horizontally"),
//         [](void*) -> void*
//         {
//             View::splitViewHorizontally(View::activeView());
//             return nullptr;
//         }
//     );
//     
//     _close_view_act = new Action(
//         "close_view",
//         tr("close_view"),
//         [](void*) -> void*
//         {
//             View::closeView(View::activeView());
//             return nullptr;
//         }
//     );
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