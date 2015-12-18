#ifndef R64FX_PROGRAM_HPP
#define R64FX_PROGRAM_HPP

namespace r64fx{

class Program{
    bool m_should_be_running = true;
    
public:
    Program(int argc, char* argv[]);
    
    virtual ~Program();
    
    /* Program main loop. */
    virtual int exec();
    
    /* Request program to quit. */
    virtual void quit();
    
    static Program* instance();
    
protected:
    virtual void setup();
    
    virtual void cleanup();
    
};
    
}//namespace r64fx

#endif//R64FX_PROGRAM_HPP