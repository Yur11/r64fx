#include <iostream>
#include <unistd.h>
#include "Config.h"
#include "Window.h"
#include "Icon.h"
#include "Dummy.h"
#include "Keyboard.h"
#include <Json.h>

using namespace std;


namespace r64fx{

#include "data_paths.cpp"
#include "serialize.cpp"

    
}//namespace r64fx


int main()
{
    using namespace r64fx;
    
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

    /* Initializ default texture. */
    Texture::init();

    /* Find an initialize the default font. */
    string default_font_path = find_path("fonts/Xolonium-Regular.otf");
    if(default_font_path.empty())
    {
        cerr << "Failed to find default font file!\n";
        abort();
    }

    Font font(default_font_path);
    if(font.isOk())
    {
        Font::initDefaultFont(&font);
    }
    else
    {
        cerr << "Problem creating default font!\n";
        abort();
    }


    /* These should be loaded from a file. */
    Scene scene;

    /* Setup root view of the main window. */
    View view(&scene);
    window.setView(&view);

    Dummy dummy(150, 150);
    scene.appendWidget(&dummy);
    dummy.setPosition(100, 100);

    /* Main event loop. */
    for(;;)
    {
        Window::processEvents();

        //Process other stuff here.

        window.render();
        window.swapBuffers();
        if(Window::shouldQuit()) break;
        usleep(3000);
    }
    

    /* Cleanup things up before exiting. */
    Texture::cleanup();
    Window::cleanup();
    
    return 0;
}