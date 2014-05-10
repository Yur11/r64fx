#include "Program.h"
#include "gui/Translation.h"
#include "gui/WindowImplementation.h"

#include "gui/LinearContainer.h"
#include "gui/Dummy.h"
#include "gui/Keyboard.h"
#include "gui/Mouse.h"
#include "gui/Font.h"
#include "gui/Icon.h"
#include "gui/View.h"

#include "DenseWaveformPainter.h"
#include "Slider.h"
#include "Knob.h"
#include "AudioClipWidget.h"
#include "AudioData.h"

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
    
    if(!DenseWaveformPainter::init())
    {
        cerr << "Failed to init DenseWaveformPainter!\n";
        return false;
    }

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
    
    LinearContainer* lc1 = new LinearContainer;
    lc1->setPadding(10.0);

    Dummy* dummy1 = new Dummy(100.0, 100.0);
    Dummy* dummy2 = new Dummy(100.0, 100.0);
    Dummy* dummy3 = new Dummy(100.0, 100.0);
    Dummy* dummy4 = new Dummy(100.0, 100.0);
    
    
    AudioData* ad1 = new AudioData((_data_prefix + "drum_loop_mono.wav").c_str());
    ad1->calculateLinear();
    
    AudioClipWidget* acw1 = new AudioClipWidget(ad1);
    acw1->updateWaveform();
    acw1->setSize(200, 100);
    
    lc1->appendWidget(dummy1);
    lc1->appendWidget(dummy2);
    lc1->appendWidget(dummy3);
    lc1->appendWidget(dummy4);
    lc1->appendWidget(acw1);
    lc1->alignHorizontally();
    
    
    LinearContainer* lc2 = new LinearContainer;
    lc2->setPadding(10.0);

    Dummy* dummy5 = new Dummy(100.0, 100.0);
    Dummy* dummy6 = new Dummy(100.0, 100.0);
    Dummy* dummy7 = new Dummy(100.0, 100.0);
    Dummy* dummy8 = new Dummy(100.0, 100.0);
    
    lc2->appendWidget(dummy5);
    lc2->appendWidget(dummy6);
    lc2->appendWidget(dummy7);
    lc2->appendWidget(dummy8);
    lc2->alignHorizontally();
    
    Size<float> slider_size(50, 100);
    LinearContainer* lc3 = new LinearContainer;
    lc3->setPadding(10.0);
    Slider* slider1 = new Slider("slider1/background", "slider1/handle");
    slider1->setSize(slider_size);
    Slider* slider2 = new Slider("slider1/background", "slider1/handle");
    slider2->setSize(slider_size);
    Slider* slider3 = new Slider("slider1/background", "slider1/handle");
    slider3->setSize(slider_size);
    Slider* slider4 = new Slider("slider1/background", "slider1/handle");
    slider4->setSize(slider_size);
    
    Slider* slider5 = new Slider("slider1/background", "slider1/handle");
    slider5->setSize(slider_size);
    Slider* slider6 = new Slider("slider1/background", "slider1/handle");
    slider6->setSize(slider_size);
    Slider* slider7 = new Slider("slider1/background", "slider1/handle");
    slider7->setSize(slider_size);
    Slider* slider8 = new Slider("slider1/background", "slider1/handle");
    slider8->setSize(slider_size);
    
    lc3->appendWidget(slider1);
    lc3->appendWidget(slider2);
    lc3->appendWidget(slider3);
    lc3->appendWidget(slider4);
    lc3->appendWidget(slider5);
    lc3->appendWidget(slider6);
    lc3->appendWidget(slider7);
    lc3->appendWidget(slider8);
    lc3->alignHorizontally();
    
    Size<float> knob_size(64, 64);
    LinearContainer* lc4 = new LinearContainer;
    lc4->setPadding(10.0);
    Knob* knob1 = new Knob("knob1/background", "knob1/rotating", "knob1/shine");
    knob1->setSize(knob_size);
    Knob* knob2 = new Knob("knob1/background", "knob1/rotating", "knob1/shine");
    knob2->setSize(knob_size);
    Knob* knob3 = new Knob("knob1/background", "knob1/rotating", "knob1/shine");
    knob3->setSize(knob_size);
    Knob* knob4 = new Knob("knob1/background", "knob1/rotating", "knob1/shine");
    knob4->setSize(knob_size);
    lc4->appendWidget(knob1);
    lc4->appendWidget(knob2);
    lc4->appendWidget(knob3);
    lc4->appendWidget(knob4);
    lc4->alignVertically();
    
    
    LinearContainer* lc = new LinearContainer;
    lc->appendWidget(lc1);
    lc->appendWidget(lc2);
    lc->appendWidget(lc3);
    lc->appendWidget(lc4);
    lc->alignVertically();
    
    View* view = new View;
    view->appendWidget(lc);
    view->setOffset(10, 10);
    
    window->setRootWidget(view);
    
    cout << "view:    " << view << "\n";
    cout << "lc:      " << lc << "\n";
    cout << "lc1:     " << lc1 << "\n";
    cout << "lc2:     " << lc2 << "\n";
    cout << "dummy1:  " << dummy1 << "\n";
    cout << "dummy2:  " << dummy2 << "\n";
    cout << "dummy3:  " << dummy3 << "\n";
    cout << "dummy4:  " << dummy4 << "\n";
    cout << "dummy5:  " << dummy5 << "\n";
    cout << "dummy6:  " << dummy6 << "\n";
    cout << "dummy7:  " << dummy7 << "\n";
    cout << "dummy8:  " << dummy8 << "\n";
    
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
    
    gl::Enable(GL_MULTISAMPLE);

    gl::Enable(GL_BLEND);
    gl::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    gl::ClearColor(0.84, 0.82, 0.81, 1.0);
    
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
    
    initCommonTexture2D("knob1/background");
    initCommonTexture2D("knob1/rotating");
    initCommonTexture2D("knob1/shine");
    
    initCommonTexture2D("slider1/background");
    initCommonTexture2D("slider1/handle");
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