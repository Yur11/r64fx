#ifndef R64FX_PROGRAM_HPP
#define R64FX_PROGRAM_HPP

namespace r64fx{
    
class ProgramPrivate;
    
class Program{
    ProgramPrivate* m = nullptr;
    
public:
    Program(int argc, char** argv);
    
    ~Program();
    
    int exec();
};
    
}//namespace r64fx

#endif//R64FX_PROGRAM_HPP
