#include <iostream>
#include <unistd.h>
#include "Config.h"
#include "SDL2Window.h"
#include "Scene.h"
#include "Dummy.h"
#include "containers.h"
#include "TextEdit.h"
#include "Keyboard.h"
#include "Icon.h"

using namespace std;

namespace r64fx{
    
string data_prefix = "../data/";

Font* debug_font = nullptr;
    
}//namespace r64fx

using namespace r64fx;


int main()
{
    if(!SDL2Window::init())
    {
        cerr << "Failed to init GUI!\n";
        return 2;
    }
    
    auto window = SDL2Window::create(800, 600, "My window");
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
    window->setView(&view);
    
    while(Window::count() > 0)
    {       
        Window::mainSequence();
    }
    
        
    window->cleanup();    
    
    return 0;
}