#include <iostream>
#include <unistd.h>
#include "Config.h"
#include "Window.h"
#include "Dummy.h"

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
    
    /* Main window opened by default. */
    Window window(800, 600, "My window");
    window.makeCurrent();
    
    /* These should be loaded from a file. */
    Scene scene;
    
    /* Setup root view of the main window. */
    View view(&scene);
    window.setView(&view);
        
    /* Remove this.*/
    Dummy dummy1(100, 100);
    Dummy dummy2(100, 100);
    Dummy dummy3(200, 100);
    
    HorizontalContainer c;
    c.appendWidget(&dummy1);
    c.appendWidget(&dummy2);
    c.appendWidget(&dummy3);
    c.setPadding(10);
    c.setSpacing(5);
    c.setPosition(650, 150);
    c.update();
    
    scene.appendWidget(&c);
    
    
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
    
    /* Clean things up before exiting. */
    Window::cleanup(); 
    
    return 0;
}