#ifndef PROGRAM_IMPL_IFACE_HPP
#define PROGRAM_IMPL_IFACE_HPP

namespace r64fx{

class WindowImplIface;

class ProgramImplEventIface{

public:
    void initMousePressEvent(WindowImplIface* window, float x, float y, unsigned int button);

    void initMouseReleaseEvent(WindowImplIface* window, float x, float y, unsigned int button);

    void initMouseMoveEvent(WindowImplIface* window, float x, float y, unsigned int buttons);

    void initKeyPressEvent(WindowImplIface* window, unsigned int key);

    void initKeyReleaseEvent(WindowImplIface* window, unsigned int key);

    void initResizeEvent(WindowImplIface* window, int w, int h);

    void initCloseEvent(WindowImplIface* window);
};

}//namespace r64fx

#endif//PROGRAM_IMPL_IFACE_HPP