#include <iostream>
#include <map>
#include <unistd.h>

#include "Config.h"
#include "Window.h"
#include "Icon.h"
#include "Dummy.h"
#include "TextEdit.h"
#include "Keyboard.h"
#include "Font.h"
#include "Json.h"

using namespace std;

#include "data_paths.cpp"
#include "filesystem.cpp"
#include "translator.cpp"
#include "serialize.cpp"

namespace r64fx{
#include "xolonium_regular_font.cpp"
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

    /* Initialize default texture. */
    Texture::init();

    /* Initialize default font. */
    Font font(xolonium_regular_font, xolonium_regular_font_size);
    if(font.isOk())
    {
        Font::initDefaultFont(&font);
    }
    else
    {
        cerr << "Problem creating default font!\n";
        abort();
    }

    tr.loadLanguage("ru");

    /* These should be loaded from a file. */
    Scene scene;

    /* Setup root view of the main window. */
    View view(&scene);
    window.setView(&view);

    Dummy dummy(150, 150);
    scene.appendWidget(&dummy);
    dummy.setPosition(100, 100);

    TextLine line(tr("Session") + " " + tr("Edit") + " " + tr("View") + " " + tr("Graph") + " " + tr("Undo") + " " + tr("Redo"));
    scene.appendWidget(&line);
    line.font()->setFaceSize(16);
    line.update();
    line.setPosition(100, 400);

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