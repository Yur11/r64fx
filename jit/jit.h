#ifndef X86_64_JIT_ASSEMBLER_H
#define X86_64_JIT_ASSEMBLER_H

#include <unistd.h>
#include <malloc.h>
#include <sys/mman.h>

namespace r64fx{

/* Usefull binary constants. */
const unsigned int b00 = 0;
const unsigned int b01 = 1;
const unsigned int b10 = 2;
const unsigned int b11 = 3;
const unsigned int b000 = 0;
const unsigned int b001 = 1;
const unsigned int b010 = 2;
const unsigned int b011 = 3;
const unsigned int b100 = 4;
const unsigned int b101 = 5;
const unsigned int b110 = 6;
const unsigned int b111 = 7;
const unsigned int b0000 = 0;
const unsigned int b0001 = 1;
const unsigned int b0010 = 2;
const unsigned int b0011 = 3;
const unsigned int b0100 = 4;
const unsigned int b0101 = 5;
const unsigned int b0110 = 6;
const unsigned int b0111 = 7;
const unsigned int b1000 = 8;
const unsigned int b1001 = 9;
const unsigned int b1010 = 10;
const unsigned int b1011 = 11;
const unsigned int b1100 = 12;
const unsigned int b1101 = 13;
const unsigned int b1110 = 14;
const unsigned int b1111 = 15;
const unsigned int b00000 = 0;
const unsigned int b00001 = 1;
const unsigned int b00010 = 2;
const unsigned int b00011 = 3;
const unsigned int b00100 = 4;
const unsigned int b00101 = 5;
const unsigned int b00110 = 6;
const unsigned int b00111 = 7;
const unsigned int b01000 = 8;
const unsigned int b01001 = 9;
const unsigned int b01010 = 10;
const unsigned int b01011 = 11;
const unsigned int b01100 = 12;
const unsigned int b01101 = 13;
const unsigned int b01110 = 14;
const unsigned int b01111 = 15;
const unsigned int b10000 = 16;
const unsigned int b10001 = 17;
const unsigned int b10010 = 18;
const unsigned int b10011 = 19;
const unsigned int b10100 = 20;
const unsigned int b10101 = 21;
const unsigned int b10110 = 22;
const unsigned int b10111 = 23;
const unsigned int b11000 = 24;
const unsigned int b11001 = 25;
const unsigned int b11010 = 26;
const unsigned int b11011 = 27;
const unsigned int b11100 = 28;
const unsigned int b11101 = 29;
const unsigned int b11110 = 30;
const unsigned int b11111 = 31;
const unsigned int b000000 = 0;
const unsigned int b000001 = 1;
const unsigned int b000010 = 2;
const unsigned int b000011 = 3;
const unsigned int b000100 = 4;
const unsigned int b000101 = 5;
const unsigned int b000110 = 6;
const unsigned int b000111 = 7;
const unsigned int b001000 = 8;
const unsigned int b001001 = 9;
const unsigned int b001010 = 10;
const unsigned int b001011 = 11;
const unsigned int b001100 = 12;
const unsigned int b001101 = 13;
const unsigned int b001110 = 14;
const unsigned int b001111 = 15;
const unsigned int b010000 = 16;
const unsigned int b010001 = 17;
const unsigned int b010010 = 18;
const unsigned int b010011 = 19;
const unsigned int b010100 = 20;
const unsigned int b010101 = 21;
const unsigned int b010110 = 22;
const unsigned int b010111 = 23;
const unsigned int b011000 = 24;
const unsigned int b011001 = 25;
const unsigned int b011010 = 26;
const unsigned int b011011 = 27;
const unsigned int b011100 = 28;
const unsigned int b011101 = 29;
const unsigned int b011110 = 30;
const unsigned int b011111 = 31;
const unsigned int b100000 = 32;
const unsigned int b100001 = 33;
const unsigned int b100010 = 34;
const unsigned int b100011 = 35;
const unsigned int b100100 = 36;
const unsigned int b100101 = 37;
const unsigned int b100110 = 38;
const unsigned int b100111 = 39;
const unsigned int b101000 = 40;
const unsigned int b101001 = 41;
const unsigned int b101010 = 42;
const unsigned int b101011 = 43;
const unsigned int b101100 = 44;
const unsigned int b101101 = 45;
const unsigned int b101110 = 46;
const unsigned int b101111 = 47;
const unsigned int b110000 = 48;
const unsigned int b110001 = 49;
const unsigned int b110010 = 50;
const unsigned int b110011 = 51;
const unsigned int b110100 = 52;
const unsigned int b110101 = 53;
const unsigned int b110110 = 54;
const unsigned int b110111 = 55;
const unsigned int b111000 = 56;
const unsigned int b111001 = 57;
const unsigned int b111010 = 58;
const unsigned int b111011 = 59;
const unsigned int b111100 = 60;
const unsigned int b111101 = 61;
const unsigned int b111110 = 62;
const unsigned int b111111 = 63;
const unsigned int b0000000 = 0;
const unsigned int b0000001 = 1;
const unsigned int b0000010 = 2;
const unsigned int b0000011 = 3;
const unsigned int b0000100 = 4;
const unsigned int b0000101 = 5;
const unsigned int b0000110 = 6;
const unsigned int b0000111 = 7;
const unsigned int b0001000 = 8;
const unsigned int b0001001 = 9;
const unsigned int b0001010 = 10;
const unsigned int b0001011 = 11;
const unsigned int b0001100 = 12;
const unsigned int b0001101 = 13;
const unsigned int b0001110 = 14;
const unsigned int b0001111 = 15;
const unsigned int b0010000 = 16;
const unsigned int b0010001 = 17;
const unsigned int b0010010 = 18;
const unsigned int b0010011 = 19;
const unsigned int b0010100 = 20;
const unsigned int b0010101 = 21;
const unsigned int b0010110 = 22;
const unsigned int b0010111 = 23;
const unsigned int b0011000 = 24;
const unsigned int b0011001 = 25;
const unsigned int b0011010 = 26;
const unsigned int b0011011 = 27;
const unsigned int b0011100 = 28;
const unsigned int b0011101 = 29;
const unsigned int b0011110 = 30;
const unsigned int b0011111 = 31;
const unsigned int b0100000 = 32;
const unsigned int b0100001 = 33;
const unsigned int b0100010 = 34;
const unsigned int b0100011 = 35;
const unsigned int b0100100 = 36;
const unsigned int b0100101 = 37;
const unsigned int b0100110 = 38;
const unsigned int b0100111 = 39;
const unsigned int b0101000 = 40;
const unsigned int b0101001 = 41;
const unsigned int b0101010 = 42;
const unsigned int b0101011 = 43;
const unsigned int b0101100 = 44;
const unsigned int b0101101 = 45;
const unsigned int b0101110 = 46;
const unsigned int b0101111 = 47;
const unsigned int b0110000 = 48;
const unsigned int b0110001 = 49;
const unsigned int b0110010 = 50;
const unsigned int b0110011 = 51;
const unsigned int b0110100 = 52;
const unsigned int b0110101 = 53;
const unsigned int b0110110 = 54;
const unsigned int b0110111 = 55;
const unsigned int b0111000 = 56;
const unsigned int b0111001 = 57;
const unsigned int b0111010 = 58;
const unsigned int b0111011 = 59;
const unsigned int b0111100 = 60;
const unsigned int b0111101 = 61;
const unsigned int b0111110 = 62;
const unsigned int b0111111 = 63;
const unsigned int b1000000 = 64;
const unsigned int b1000001 = 65;
const unsigned int b1000010 = 66;
const unsigned int b1000011 = 67;
const unsigned int b1000100 = 68;
const unsigned int b1000101 = 69;
const unsigned int b1000110 = 70;
const unsigned int b1000111 = 71;
const unsigned int b1001000 = 72;
const unsigned int b1001001 = 73;
const unsigned int b1001010 = 74;
const unsigned int b1001011 = 75;
const unsigned int b1001100 = 76;
const unsigned int b1001101 = 77;
const unsigned int b1001110 = 78;
const unsigned int b1001111 = 79;
const unsigned int b1010000 = 80;
const unsigned int b1010001 = 81;
const unsigned int b1010010 = 82;
const unsigned int b1010011 = 83;
const unsigned int b1010100 = 84;
const unsigned int b1010101 = 85;
const unsigned int b1010110 = 86;
const unsigned int b1010111 = 87;
const unsigned int b1011000 = 88;
const unsigned int b1011001 = 89;
const unsigned int b1011010 = 90;
const unsigned int b1011011 = 91;
const unsigned int b1011100 = 92;
const unsigned int b1011101 = 93;
const unsigned int b1011110 = 94;
const unsigned int b1011111 = 95;
const unsigned int b1100000 = 96;
const unsigned int b1100001 = 97;
const unsigned int b1100010 = 98;
const unsigned int b1100011 = 99;
const unsigned int b1100100 = 100;
const unsigned int b1100101 = 101;
const unsigned int b1100110 = 102;
const unsigned int b1100111 = 103;
const unsigned int b1101000 = 104;
const unsigned int b1101001 = 105;
const unsigned int b1101010 = 106;
const unsigned int b1101011 = 107;
const unsigned int b1101100 = 108;
const unsigned int b1101101 = 109;
const unsigned int b1101110 = 110;
const unsigned int b1101111 = 111;
const unsigned int b1110000 = 112;
const unsigned int b1110001 = 113;
const unsigned int b1110010 = 114;
const unsigned int b1110011 = 115;
const unsigned int b1110100 = 116;
const unsigned int b1110101 = 117;
const unsigned int b1110110 = 118;
const unsigned int b1110111 = 119;
const unsigned int b1111000 = 120;
const unsigned int b1111001 = 121;
const unsigned int b1111010 = 122;
const unsigned int b1111011 = 123;
const unsigned int b1111100 = 124;
const unsigned int b1111101 = 125;
const unsigned int b1111110 = 126;
const unsigned int b1111111 = 127;
const unsigned int b00000000 = 0;
const unsigned int b00000001 = 1;
const unsigned int b00000010 = 2;
const unsigned int b00000011 = 3;
const unsigned int b00000100 = 4;
const unsigned int b00000101 = 5;
const unsigned int b00000110 = 6;
const unsigned int b00000111 = 7;
const unsigned int b00001000 = 8;
const unsigned int b00001001 = 9;
const unsigned int b00001010 = 10;
const unsigned int b00001011 = 11;
const unsigned int b00001100 = 12;
const unsigned int b00001101 = 13;
const unsigned int b00001110 = 14;
const unsigned int b00001111 = 15;
const unsigned int b00010000 = 16;
const unsigned int b00010001 = 17;
const unsigned int b00010010 = 18;
const unsigned int b00010011 = 19;
const unsigned int b00010100 = 20;
const unsigned int b00010101 = 21;
const unsigned int b00010110 = 22;
const unsigned int b00010111 = 23;
const unsigned int b00011000 = 24;
const unsigned int b00011001 = 25;
const unsigned int b00011010 = 26;
const unsigned int b00011011 = 27;
const unsigned int b00011100 = 28;
const unsigned int b00011101 = 29;
const unsigned int b00011110 = 30;
const unsigned int b00011111 = 31;
const unsigned int b00100000 = 32;
const unsigned int b00100001 = 33;
const unsigned int b00100010 = 34;
const unsigned int b00100011 = 35;
const unsigned int b00100100 = 36;
const unsigned int b00100101 = 37;
const unsigned int b00100110 = 38;
const unsigned int b00100111 = 39;
const unsigned int b00101000 = 40;
const unsigned int b00101001 = 41;
const unsigned int b00101010 = 42;
const unsigned int b00101011 = 43;
const unsigned int b00101100 = 44;
const unsigned int b00101101 = 45;
const unsigned int b00101110 = 46;
const unsigned int b00101111 = 47;
const unsigned int b00110000 = 48;
const unsigned int b00110001 = 49;
const unsigned int b00110010 = 50;
const unsigned int b00110011 = 51;
const unsigned int b00110100 = 52;
const unsigned int b00110101 = 53;
const unsigned int b00110110 = 54;
const unsigned int b00110111 = 55;
const unsigned int b00111000 = 56;
const unsigned int b00111001 = 57;
const unsigned int b00111010 = 58;
const unsigned int b00111011 = 59;
const unsigned int b00111100 = 60;
const unsigned int b00111101 = 61;
const unsigned int b00111110 = 62;
const unsigned int b00111111 = 63;
const unsigned int b01000000 = 64;
const unsigned int b01000001 = 65;
const unsigned int b01000010 = 66;
const unsigned int b01000011 = 67;
const unsigned int b01000100 = 68;
const unsigned int b01000101 = 69;
const unsigned int b01000110 = 70;
const unsigned int b01000111 = 71;
const unsigned int b01001000 = 72;
const unsigned int b01001001 = 73;
const unsigned int b01001010 = 74;
const unsigned int b01001011 = 75;
const unsigned int b01001100 = 76;
const unsigned int b01001101 = 77;
const unsigned int b01001110 = 78;
const unsigned int b01001111 = 79;
const unsigned int b01010000 = 80;
const unsigned int b01010001 = 81;
const unsigned int b01010010 = 82;
const unsigned int b01010011 = 83;
const unsigned int b01010100 = 84;
const unsigned int b01010101 = 85;
const unsigned int b01010110 = 86;
const unsigned int b01010111 = 87;
const unsigned int b01011000 = 88;
const unsigned int b01011001 = 89;
const unsigned int b01011010 = 90;
const unsigned int b01011011 = 91;
const unsigned int b01011100 = 92;
const unsigned int b01011101 = 93;
const unsigned int b01011110 = 94;
const unsigned int b01011111 = 95;
const unsigned int b01100000 = 96;
const unsigned int b01100001 = 97;
const unsigned int b01100010 = 98;
const unsigned int b01100011 = 99;
const unsigned int b01100100 = 100;
const unsigned int b01100101 = 101;
const unsigned int b01100110 = 102;
const unsigned int b01100111 = 103;
const unsigned int b01101000 = 104;
const unsigned int b01101001 = 105;
const unsigned int b01101010 = 106;
const unsigned int b01101011 = 107;
const unsigned int b01101100 = 108;
const unsigned int b01101101 = 109;
const unsigned int b01101110 = 110;
const unsigned int b01101111 = 111;
const unsigned int b01110000 = 112;
const unsigned int b01110001 = 113;
const unsigned int b01110010 = 114;
const unsigned int b01110011 = 115;
const unsigned int b01110100 = 116;
const unsigned int b01110101 = 117;
const unsigned int b01110110 = 118;
const unsigned int b01110111 = 119;
const unsigned int b01111000 = 120;
const unsigned int b01111001 = 121;
const unsigned int b01111010 = 122;
const unsigned int b01111011 = 123;
const unsigned int b01111100 = 124;
const unsigned int b01111101 = 125;
const unsigned int b01111110 = 126;
const unsigned int b01111111 = 127;
const unsigned int b10000000 = 128;
const unsigned int b10000001 = 129;
const unsigned int b10000010 = 130;
const unsigned int b10000011 = 131;
const unsigned int b10000100 = 132;
const unsigned int b10000101 = 133;
const unsigned int b10000110 = 134;
const unsigned int b10000111 = 135;
const unsigned int b10001000 = 136;
const unsigned int b10001001 = 137;
const unsigned int b10001010 = 138;
const unsigned int b10001011 = 139;
const unsigned int b10001100 = 140;
const unsigned int b10001101 = 141;
const unsigned int b10001110 = 142;
const unsigned int b10001111 = 143;
const unsigned int b10010000 = 144;
const unsigned int b10010001 = 145;
const unsigned int b10010010 = 146;
const unsigned int b10010011 = 147;
const unsigned int b10010100 = 148;
const unsigned int b10010101 = 149;
const unsigned int b10010110 = 150;
const unsigned int b10010111 = 151;
const unsigned int b10011000 = 152;
const unsigned int b10011001 = 153;
const unsigned int b10011010 = 154;
const unsigned int b10011011 = 155;
const unsigned int b10011100 = 156;
const unsigned int b10011101 = 157;
const unsigned int b10011110 = 158;
const unsigned int b10011111 = 159;
const unsigned int b10100000 = 160;
const unsigned int b10100001 = 161;
const unsigned int b10100010 = 162;
const unsigned int b10100011 = 163;
const unsigned int b10100100 = 164;
const unsigned int b10100101 = 165;
const unsigned int b10100110 = 166;
const unsigned int b10100111 = 167;
const unsigned int b10101000 = 168;
const unsigned int b10101001 = 169;
const unsigned int b10101010 = 170;
const unsigned int b10101011 = 171;
const unsigned int b10101100 = 172;
const unsigned int b10101101 = 173;
const unsigned int b10101110 = 174;
const unsigned int b10101111 = 175;
const unsigned int b10110000 = 176;
const unsigned int b10110001 = 177;
const unsigned int b10110010 = 178;
const unsigned int b10110011 = 179;
const unsigned int b10110100 = 180;
const unsigned int b10110101 = 181;
const unsigned int b10110110 = 182;
const unsigned int b10110111 = 183;
const unsigned int b10111000 = 184;
const unsigned int b10111001 = 185;
const unsigned int b10111010 = 186;
const unsigned int b10111011 = 187;
const unsigned int b10111100 = 188;
const unsigned int b10111101 = 189;
const unsigned int b10111110 = 190;
const unsigned int b10111111 = 191;
const unsigned int b11000000 = 192;
const unsigned int b11000001 = 193;
const unsigned int b11000010 = 194;
const unsigned int b11000011 = 195;
const unsigned int b11000100 = 196;
const unsigned int b11000101 = 197;
const unsigned int b11000110 = 198;
const unsigned int b11000111 = 199;
const unsigned int b11001000 = 200;
const unsigned int b11001001 = 201;
const unsigned int b11001010 = 202;
const unsigned int b11001011 = 203;
const unsigned int b11001100 = 204;
const unsigned int b11001101 = 205;
const unsigned int b11001110 = 206;
const unsigned int b11001111 = 207;
const unsigned int b11010000 = 208;
const unsigned int b11010001 = 209;
const unsigned int b11010010 = 210;
const unsigned int b11010011 = 211;
const unsigned int b11010100 = 212;
const unsigned int b11010101 = 213;
const unsigned int b11010110 = 214;
const unsigned int b11010111 = 215;
const unsigned int b11011000 = 216;
const unsigned int b11011001 = 217;
const unsigned int b11011010 = 218;
const unsigned int b11011011 = 219;
const unsigned int b11011100 = 220;
const unsigned int b11011101 = 221;
const unsigned int b11011110 = 222;
const unsigned int b11011111 = 223;
const unsigned int b11100000 = 224;
const unsigned int b11100001 = 225;
const unsigned int b11100010 = 226;
const unsigned int b11100011 = 227;
const unsigned int b11100100 = 228;
const unsigned int b11100101 = 229;
const unsigned int b11100110 = 230;
const unsigned int b11100111 = 231;
const unsigned int b11101000 = 232;
const unsigned int b11101001 = 233;
const unsigned int b11101010 = 234;
const unsigned int b11101011 = 235;
const unsigned int b11101100 = 236;
const unsigned int b11101101 = 237;
const unsigned int b11101110 = 238;
const unsigned int b11101111 = 239;
const unsigned int b11110000 = 240;
const unsigned int b11110001 = 241;
const unsigned int b11110010 = 242;
const unsigned int b11110011 = 243;
const unsigned int b11110100 = 244;
const unsigned int b11110101 = 245;
const unsigned int b11110110 = 246;
const unsigned int b11110111 = 247;
const unsigned int b11111000 = 248;
const unsigned int b11111001 = 249;
const unsigned int b11111010 = 250;
const unsigned int b11111011 = 251;
const unsigned int b11111100 = 252;
const unsigned int b11111101 = 253;
const unsigned int b11111110 = 254;
const unsigned int b11111111 = 255;


/* Immediate operands. */
class Imm8{
    friend class CodeBuffer;
    unsigned char byte;
    
public:
    explicit Imm8(unsigned char byte)
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
    explicit Imm16(unsigned short word)
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
    explicit Imm32(unsigned int dword)
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
    explicit Imm64(unsigned long long qword)
    {
        bytes.qword = qword;
    }

    explicit Imm64(void* ptr)
    {
        bytes.qword = (long int) ptr;
    }
}; 


class Register{
    const unsigned char _bits;

public:
    Register(const unsigned char bits) : _bits(bits) {}
    
    inline unsigned char bits() const { return _bits; }
    
    inline unsigned char code() const { return _bits; }
    
    /** @brief R or B bit of the REX prefix.*/
    inline bool prefix_bit() const { return _bits & b1000; }
};


class GPR64 : public Register{
    
public:
    explicit GPR64(const unsigned char bits) : Register(bits) {}
};

const GPR64
    rax(b0000),
    rcx(b0001),
    rdx(b0010),
    rbx(b0011),
    rsp(b0100),
    rbp(b0101),
    rsi(b0110),
    rdi(b0111),
    r8 (b1000),
    r9 (b1001),
    r10(b1010),
    r11(b1011),
    r12(b1100),
    r13(b1101),
    r14(b1110),
    r15(b1111)
;


class GPR32 : public Register{
    
public:
    explicit GPR32(const unsigned char bits) : Register(bits) {}
};

const GPR32
    eax(b000),
    ecx(b001),
    edx(b010),
    ebx(b011),
    esp(b100),
    ebp(b101),
    esi(b110),
    edi(b111),
    r8d (b1000),
    r9d (b1001),
    r10d(b1010),
    r11d(b1011),
    r12d(b1100),
    r13d(b1101),
    r14d(b1110),
    r15d(b1111)
;


class GPR16 : public Register{
    
public:
    explicit GPR16(const unsigned char bits) : Register(bits) {}
};

const GPR16
    ax(b000),
    cx(b001),
    dx(b010),
    bx(b011),
    sp(b100),
    bp(b101),
    si(b110),
    di(b111)
;


class GPR8 : public Register{

public:
    explicit GPR8(const unsigned char bits) : Register(bits) {}
};

const GPR8
    al(b000),
    cl(b001),
    dl(b010),
    bl(b011),
    ah(b100),
    ch(b101),
    dh(b110),
    bh(b111)
;


class Xmm : public Register{

public:
    explicit Xmm(const unsigned char bits) : Register(bits) {}
};

const Xmm
    xmm0(b0000),
    xmm1(b0001),
    xmm2(b0010),
    xmm3(b0011),
    xmm4(b0100),
    xmm5(b0101),
    xmm6(b0110),
    xmm7(b0111),
    xmm8(b1000),
    xmm9(b1001),
    xmm10(b1010),
    xmm11(b1011),
    xmm12(b1100),
    xmm13(b1101),
    xmm14(b1110),
    xmm15(b1111)
;


class Ymm : public Register{

public:
    explicit Ymm(const unsigned char bits) : Register(bits) {}
};

const Ymm
    ymm0(b0000),
    ymm1(b0001),
    ymm2(b0010),
    ymm3(b0011),
    ymm4(b0100),
    ymm5(b0101),
    ymm6(b0110),
    ymm7(b0111),
    ymm8(b1000),
    ymm9(b1001),
    ymm10(b1010),
    ymm11(b1011),
    ymm12(b1100),
    ymm13(b1101),
    ymm14(b1110),
    ymm15(b1111)
;

struct Mem8{
    long int addr;
    
    explicit Mem8(void* addr)
    {
        this->addr = (long int) addr;
    }
};

struct Mem16{
    long int addr;
    
    explicit Mem16(void* addr)
    {
        this->addr = (long int) addr;
    }
};

struct Mem32{
    long int addr;
    
    explicit Mem32(void* addr)
    {
        this->addr = (long int) addr;
    }
};

struct Mem64{
    long int addr;
    
    explicit Mem64(void* addr)
    {
        this->addr = (long int) addr;
    }
};

struct Mem128{
    long int addr;
    
    explicit Mem128(void* addr)
    {
        this->addr = (long int) addr;
    }
};

struct Mem256{
    long int addr;
    
    explicit Mem256(void* addr)
    {
        this->addr = (long int) addr;
    }
};


struct Base{
    GPR64 reg;

    explicit Base(GPR64 reg) : reg(reg) {}
};


struct Disp8{
    unsigned char byte;

    explicit Disp8(unsigned char byte) : byte(byte) {}
};


const unsigned char Scale1 = b00;
const unsigned char Scale2 = b01;
const unsigned char Scale4 = b10;
const unsigned char Scale8 = b11;


class CmpCode{
    unsigned int _code;

public:
    CmpCode(unsigned int code) : _code(code) {}

    inline unsigned int code() const { return _code; }
};

const CmpCode
    EQ(0),
    LT(1),
    LE(2),
    UNORD(3),
    NEQ(4),
    NLT(5),
    NLE(6),
    ORD(7)
;


class CodeBuffer{
    unsigned char* begin;
    unsigned char* end;
    int _npages;
    
public:
    CodeBuffer(int npages = 1);
    
    ~CodeBuffer()
    {

    }
    
    void cleanup()
    {
        free(begin);
    }
    
    /** @brief Pointer to the beginning of the buffer. */
    inline unsigned char* codeBegin() const { return begin; }
    
    /** @brief Pointer to the byte past the end of the written bytes. 
     
        This can be used to obtain memory locations for doing branching to lower addresses.
     */
    inline unsigned char* codeEnd() const { return end; }
    
    inline void setEnd(void* addr) { end = (unsigned char*) addr; }
    
    inline int npages() const { return _npages; }
    
    void allowExecution();
    
    CodeBuffer &operator<<(unsigned char byte);

    inline CodeBuffer &operator<<(Imm8 imm)
    {
        return operator<<(imm.byte);
    }

    CodeBuffer &operator<<(Imm16 imm);
    
    CodeBuffer &operator<<(Imm32 imm);
    
    CodeBuffer &operator<<(Imm64 imm);
};


class Assembler{
    CodeBuffer bytes;
    bool must_free;

public:
    Assembler(int npages = 1) : bytes(npages), must_free(true)
    {

    }

    /** Use an external CodeBuffer. Do not free it! */
    Assembler(CodeBuffer cb) : bytes(cb), must_free(false)
    {

    }

    ~Assembler()
    {
        if(must_free) bytes.cleanup();
    }

    inline CodeBuffer &buffer() { return bytes; }

    inline void* getFun()
    {
        bytes.allowExecution();
        return bytes.codeBegin();
    }

    inline unsigned char* ip()
    {
        return bytes.codeEnd();
    }

    /** @brief Insert one or more nop instructions. */
    inline void nop(int count = 1)
    {
        while(count--) bytes << 0x90;
    }

    inline void ret()
    {
        bytes << 0xC3;
    }

    void add(GPR32 reg, Mem32 mem);
    void add(Mem32 mem, GPR32 reg);
    void add(GPR32 dst, GPR32 src);
    void add(GPR32 reg, unsigned int imm);

    void add(GPR64 reg, Mem64 mem);
    void add(Mem64 mem, GPR64 reg);
    void add(GPR64 dst, GPR64 src);
    void add(GPR64 reg, unsigned int imm);

    void add(GPR64 reg, Base base);
    void add(Base base, GPR64 reg);

    void mov(GPR32 reg, Mem32 mem);
    void mov(Mem32 mem, GPR32 reg);
    void mov(GPR32 dst, GPR32 src);
    void mov(GPR32 reg, unsigned int imm);

    void sub(GPR32 reg, Mem32 mem);
    void sub(Mem32 mem, GPR32 reg);
    void sub(GPR32 dst, GPR32 src);
    void sub(GPR32 reg, unsigned int imm);

    void sub(GPR64 reg, Mem64 mem);
    void sub(Mem64 mem, GPR64 reg);
    void sub(GPR64 dst, GPR64 src);
    void sub(GPR64 reg, unsigned int imm);

    void sub(GPR64 reg, Base base);
    void sub(Base base, GPR64 reg);


    void mov(GPR64 reg, Mem64 mem);
    void mov(Mem64 mem, GPR64 reg);
    void mov(GPR64 dst, GPR64 src);
    void mov(GPR64 reg, Imm32 imm);
    void mov(GPR64 reg, Imm64 imm);
    inline void mov(GPR64 reg, unsigned int imm)      { mov(reg, Imm32(imm)); }

    void mov(GPR64 reg, Base base);
    void mov(Base base, GPR64 reg);


    void push(GPR64 reg);
    void push(unsigned int imm);

    void pop(GPR64 reg);

    /* SSE */
    void addps(Xmm dst, Xmm src);
    void addps(Xmm reg, Mem128 mem);
    void addps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void addss(Xmm dst, Xmm src);
    void addss(Xmm reg, Mem32 mem);
    void addss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void subps(Xmm dst, Xmm src);
    void subps(Xmm reg, Mem128 mem);
    void subps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void subss(Xmm dst, Xmm src);
    void subss(Xmm reg, Mem32 mem);
    void subss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void mulps(Xmm dst, Xmm src);
    void mulps(Xmm reg, Mem128 mem);
    void mulps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void mulss(Xmm dst, Xmm src);
    void mulss(Xmm reg, Mem32 mem);
    void mulss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void divps(Xmm dst, Xmm src);
    void divps(Xmm reg, Mem128 mem);
    void divps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void divss(Xmm dst, Xmm src);
    void divss(Xmm reg, Mem32 mem);
    void divss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rcpps(Xmm dst, Xmm src);
    void rcpps(Xmm reg, Mem128 mem);
    void rcpps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rcpss(Xmm dst, Xmm src);
    void rcpss(Xmm reg, Mem32 mem);
    void rcpss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void sqrtps(Xmm dst, Xmm src);
    void sqrtps(Xmm reg, Mem128 mem);
    void sqrtps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void sqrtss(Xmm dst, Xmm src);
    void sqrtss(Xmm reg, Mem32 mem);
    void sqrtss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rsqrtps(Xmm dst, Xmm src);
    void rsqrtps(Xmm reg, Mem128 mem);
    void rsqrtps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void rsqrtss(Xmm dst, Xmm src);
    void rsqrtss(Xmm reg, Mem32 mem);
    void rsqrtss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void maxps(Xmm dst, Xmm src);
    void maxps(Xmm reg, Mem128 mem);
    void maxps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void maxss(Xmm dst, Xmm src);
    void maxss(Xmm reg, Mem32 mem);
    void maxss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void minps(Xmm dst, Xmm src);
    void minps(Xmm reg, Mem128 mem);
    void minps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void minss(Xmm dst, Xmm src);
    void minss(Xmm reg, Mem32 mem);
    void minss(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void andps(Xmm dst, Xmm src);
    void andps(Xmm reg, Mem128 mem);
    void andps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void andnps(Xmm dst, Xmm src);
    void andnps(Xmm reg, Mem128 mem);
    void andnps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void orps(Xmm dst, Xmm src);
    void orps(Xmm reg, Mem128 mem);
    void orps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void xorps(Xmm dst, Xmm src);
    void xorps(Xmm reg, Mem128 mem);
    void xorps(Xmm reg, Base base, Disp8 disp = Disp8(0));

    void cmpps(CmpCode kind, Xmm dst, Xmm src);
    void cmpps(CmpCode kind, Xmm reg, Mem128 mem);
    void cmpps(CmpCode kind, Xmm reg, Base base, Disp8 disp = Disp8(0));

    void cmpss(CmpCode kind, Xmm dst, Xmm src);
    void cmpss(CmpCode kind, Xmm reg, Mem128 mem);
    void cmpss(CmpCode kind, Xmm reg, Base base, Disp8 disp = Disp8(0));

    void movups(Xmm dst, Xmm src);
    void movups(Xmm reg, Mem128 mem);
    void movups(Xmm reg, Base base, Disp8 disp = Disp8(0));
    void movups(Mem128, Xmm reg);
    void movups(Base base, Disp8 disp, Xmm reg);
    inline void movups(Base base, Xmm reg) { movups(base, Disp8(0), reg); }

    void movaps(Xmm dst, Xmm src);
    void movaps(Xmm reg, Mem128 mem);
    void movaps(Xmm reg, Base base, Disp8 disp = Disp8(0));
    void movaps(Mem128, Xmm reg);
    void movaps(Base base, Disp8 disp, Xmm reg);
    inline void movaps(Base base, Xmm reg) { movaps(base, Disp8(0), reg); }

};//Assembler

}//namespace r64fx



#endif//X86_64_JIT_ASSEMBLER_H