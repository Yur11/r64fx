#include <iostream>
#include <unistd.h>
#include "Config.h"
#include "Window.h"
#include "Icon.h"
#include "Dummy.h"
#include "Keyboard.h"

using namespace std;


namespace r64fx{
    
    
    
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
     * This creates OpenGL context.
     * 
     * A lot of things in the gui can be done only after this step.
     * This is true for everything that has to do with rendering or texture loading.
     */
    Window window(800, 600, "r64fx");
    window.makeCurrent();
    
    try
    {
        /* Initialize default texture. */
        Texture::init();
        
        /* Initialize default font. */
        Font::init();
        
        /*  */
        Keyboard::init();
        
        /* These should be loaded from a file. */
        Scene scene;
        
        /* Setup root view of the main window. */
        View view(&scene);
        window.setView(&view);
            
        TextLine line("Hello, world! йцукенгшщзхъфывапролджэячсмитьбю");
        scene.appendWidget(&line);
        line.setPosition(100, 100);
        line.font()->setFaceSize(72);
        line.update();
        
//         Dummy dummy(200, 200);
//         scene.appendWidget(&dummy);
//         dummy.setPosition(300, 50);
//                 
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
    }
    catch(Font::Error font_error)
    {
        cerr << "Font::Error " << font_error.message() << "\n";
    }
    
    
    /* Clean things up before exiting. */
    Texture::cleanup();
    Font::cleanup();
    Window::cleanup();
    
    return 0;
}