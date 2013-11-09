#include <iostream>
#include <unistd.h>
#include "Config.h"
#include "Window.h"
#include "Scene.h"
#include "Dummy.h"
#include "containers.h"
#include "TextEdit.h"
#include "Keyboard.h"
#include "Icon.h"
#include "MenuItem.h"
#include "menus.h"

using namespace std;
using namespace r64fx;


int main()
{
    if(!Config::init())
    {
        cerr << "Failed to init configuration!\n";
        return 1;
    }
    
    if(!Window::init())
    {
        cerr << "Failed to init GUI!\n";
        return 2;
    }
    
    Window window(800, 600, "My window");
    Scene scene;
    View view(&scene);
    
    Dummy dummy1(100, 100);
    Dummy dummy2(100, 100);
    Dummy dummy3(200, 100);
    
    HorizontalContainer c;
    c.appendWidget(&dummy1);
    c.appendWidget(&dummy2);
    c.appendWidget(&dummy3);
    c.setPadding(10);
    c.setSpacing(5);
    c.setPosition(50, 50);
    c.update();
    
    scene.appendWidget(&c);
    
    window.setView(&view);
    for(;;)
    {
        Window::processEvents();
        
        //Process other stuff here.
        
        window.render();
        window.swapBuffers();
        if(Window::shouldQuit()) break;
        usleep(3000);
    }
    
    Keyboard::endTextInput();
        
    Window::cleanup();    
    
    return 0;
}