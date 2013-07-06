/* Immediate operands. */
class Imm8{
    friend class CodeBuffer;
    unsigned char byte;
    
public:
    Imm8(unsigned char byte)
    {
        this->byte = byte;
    }
};

class Imm16{
    friend class CodeBuffer;
    union{
        unsigned short word;
        unsigned char byte[2];
    }bytes;
    
public:
    Imm16(unsigned short word)
    {
        bytes.word = word;
    }
};


class Imm32{
    friend class CodeBuffer;
    union{
        unsigned int dword;
        unsigned char byte[4];
    }bytes;
    
public:
    Imm32(unsigned int dword)
    {
        bytes.dword = dword;
    }
};


class Imm64{
    friend class CodeBuffer;
    union{
        unsigned long long qword;
        unsigned char byte[8];
    }bytes;
    
public:
    Imm64(unsigned long long qword)
    {
        bytes.qword = qword;
    }
}; 
