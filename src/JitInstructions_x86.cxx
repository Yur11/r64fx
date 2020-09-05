inline void RET       ()                                        { A(1); W(0xC3); R64FXDBG; }
inline void NOP       ()                                        { A(1); W(0x90); R64FXDBG; }
inline void RDTSC     ()                                        { A(2); W(0x0F, 0x31); R64FXDBG; }

inline void PUSH      (Imm32 i)                                 { A(5); W(0x68); IMM(i); R64FXDBG; }
inline void PUSH      (Mem64 m)                                 { A(6); W(0xFF, ModRM(0,6,5)); RIP(m.addr); R64FXDBG; }
inline void POP       (Mem64 m)                                 { A(6); W(0x8F, ModRM(0,0,5)); RIP(m.addr); R64FXDBG; }

inline void JNO       (JumpLabel8 &j)                           { A(2); W(0x71); LABEL(j); R64FXDBG; }
inline void JB        (JumpLabel8 &j)                           { A(2); W(0x72); LABEL(j); R64FXDBG; }
inline void JNAE      (JumpLabel8 &j)                           { A(2); W(0x72); LABEL(j); R64FXDBG; }
inline void JAE       (JumpLabel8 &j)                           { A(2); W(0x73); LABEL(j); R64FXDBG; }
inline void JNB       (JumpLabel8 &j)                           { A(2); W(0x73); LABEL(j); R64FXDBG; }
inline void JNC       (JumpLabel8 &j)                           { A(2); W(0x73); LABEL(j); R64FXDBG; }
inline void JE        (JumpLabel8 &j)                           { A(2); W(0x74); LABEL(j); R64FXDBG; }
inline void JZ        (JumpLabel8 &j)                           { A(2); W(0x74); LABEL(j); R64FXDBG; }
inline void JNE       (JumpLabel8 &j)                           { A(2); W(0x75); LABEL(j); R64FXDBG; }
inline void JNZ       (JumpLabel8 &j)                           { A(2); W(0x75); LABEL(j); R64FXDBG; }
inline void JBE       (JumpLabel8 &j)                           { A(2); W(0x76); LABEL(j); R64FXDBG; }
inline void JNA       (JumpLabel8 &j)                           { A(2); W(0x76); LABEL(j); R64FXDBG; }
inline void JS        (JumpLabel8 &j)                           { A(2); W(0x78); LABEL(j); R64FXDBG; }
inline void JA        (JumpLabel8 &j)                           { A(2); W(0x77); LABEL(j); R64FXDBG; }
inline void JNBE      (JumpLabel8 &j)                           { A(2); W(0x77); LABEL(j); R64FXDBG; }
inline void JP        (JumpLabel8 &j)                           { A(2); W(0x7A); LABEL(j); R64FXDBG; }
inline void JPE       (JumpLabel8 &j)                           { A(2); W(0x7A); LABEL(j); R64FXDBG; }
inline void JNP       (JumpLabel8 &j)                           { A(2); W(0x7B); LABEL(j); R64FXDBG; }
inline void JPO       (JumpLabel8 &j)                           { A(2); W(0x7B); LABEL(j); R64FXDBG; }
inline void JL        (JumpLabel8 &j)                           { A(2); W(0x7C); LABEL(j); R64FXDBG; }
inline void JNGE      (JumpLabel8 &j)                           { A(2); W(0x7C); LABEL(j); R64FXDBG; }
inline void JGE       (JumpLabel8 &j)                           { A(2); W(0x7D); LABEL(j); R64FXDBG; }
inline void JNL       (JumpLabel8 &j)                           { A(2); W(0x7D); LABEL(j); R64FXDBG; }
inline void JLE       (JumpLabel8 &j)                           { A(2); W(0x7E); LABEL(j); R64FXDBG; }
inline void JNG       (JumpLabel8 &j)                           { A(2); W(0x7E); LABEL(j); R64FXDBG; }
inline void JG        (JumpLabel8 &j)                           { A(2); W(0x7F); LABEL(j); R64FXDBG; }
inline void JNLE      (JumpLabel8 &j)                           { A(2); W(0x7F); LABEL(j); R64FXDBG; }
inline void JCXZ      (JumpLabel8 &j)                           { A(2); W(0xE3); LABEL(j); R64FXDBG; }

inline void JNE       (JumpLabel32 &j)                          { A(6); W(0x0F, 0x85); LABEL(j); R64FXDBG; }

inline void MOV       (GPR32 d, Imm32 s)                        { B(d); A(5,mp); O<64>(mp); W(0xB8+d); IMM(s); R64FXDBG; }
inline void MOV       (GPR64 d, Imm64 s)                        { A(10); W(72|B(d), 0xB8+d); IMM(s); R64FXDBG; }
inline void MOV       (GPR64 d, GPR64 s)                        { A(3); W(72|RB(d,s), 0x8B, ModRM(3,d,s)); R64FXDBG; }
inline void MOV       (GPR64 d, Mem64 s)                        { A(7); W(72|R(d), 0x8B, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MOV       (GPR64 d, SIBD  s)                        { A(4+s.disp_size); W(72|RXB(d,s), 0x8B, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }

inline void ADD       (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0x83, ModRM(3,0,d)); IMM(s); R64FXDBG; }
inline void OR        (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0x83, ModRM(3,1,d)); IMM(s); R64FXDBG; }
inline void AND       (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0x83, ModRM(3,4,d)); IMM(s); R64FXDBG; }
inline void SUB       (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0x83, ModRM(3,5,d)); IMM(s); R64FXDBG; }
inline void XOR       (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0x83, ModRM(3,6,d)); IMM(s); R64FXDBG; }

inline void ADD       (Mem64 d, Imm8 s)                         { A(8); W(72, 0x83, ModRM(0,0,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }
inline void OR        (Mem64 d, Imm8 s)                         { A(8); W(72, 0x83, ModRM(0,1,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }
inline void AND       (Mem64 d, Imm8 s)                         { A(8); W(72, 0x83, ModRM(0,4,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }
inline void SUB       (Mem64 d, Imm8 s)                         { A(8); W(72, 0x83, ModRM(0,5,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }
inline void XOR       (Mem64 d, Imm8 s)                         { A(8); W(72, 0x83, ModRM(0,6,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }

inline void ADD       (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0x83, ModRM(0,0,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }
inline void OR        (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0x83, ModRM(0,1,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }
inline void AND       (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0x83, ModRM(0,4,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }
inline void SUB       (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0x83, ModRM(0,5,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }
inline void XOR       (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0x83, ModRM(0,6,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }

inline void ADD       (GPR64 d, GPR64 s)                        { A(3); W(72|RB(d,s), 0x03, ModRM(3,d,s)); R64FXDBG; }
inline void OR        (GPR64 d, GPR64 s)                        { A(3); W(72|RB(d,s), 0x0B, ModRM(3,d,s)); R64FXDBG; }
inline void AND       (GPR64 d, GPR64 s)                        { A(3); W(72|RB(d,s), 0x23, ModRM(3,d,s)); R64FXDBG; }
inline void SUB       (GPR64 d, GPR64 s)                        { A(3); W(72|RB(d,s), 0x2B, ModRM(3,d,s)); R64FXDBG; }
inline void XOR       (GPR64 d, GPR64 s)                        { A(3); W(72|RB(d,s), 0x33, ModRM(3,d,s)); R64FXDBG; }

inline void ADD       (GPR64 d, Mem64 s)                        { A(7); W(72|R(d), 0x03, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void OR        (GPR64 d, Mem64 s)                        { A(7); W(72|R(d), 0x0B, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void AND       (GPR64 d, Mem64 s)                        { A(7); W(72|R(d), 0x23, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void SUB       (GPR64 d, Mem64 s)                        { A(7); W(72|R(d), 0x2B, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void XOR       (GPR64 d, Mem64 s)                        { A(7); W(72|R(d), 0x33, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }

inline void ADD       (GPR64 d, SIBD s)                         { A(4+s.disp_size); W(72|RXB(d,s), 0x03, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void OR        (GPR64 d, SIBD s)                         { A(4+s.disp_size); W(72|RXB(d,s), 0x0B, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void AND       (GPR64 d, SIBD s)                         { A(4+s.disp_size); W(72|RXB(d,s), 0x23, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void SUB       (GPR64 d, SIBD s)                         { A(4+s.disp_size); W(72|RXB(d,s), 0x2B, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void XOR       (GPR64 d, SIBD s)                         { A(4+s.disp_size); W(72|RXB(d,s), 0x33, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }

inline void SHL       (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0xC1, ModRM(3,4,d)); IMM(s); R64FXDBG; }
inline void SHL       (GPR64 d)                                 { A(3); W(72|B(d), 0xD1, ModRM(3,4,d)); R64FXDBG; }
inline void SHR       (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0xC1, ModRM(3,5,d)); IMM(s); R64FXDBG; }
inline void SHR       (GPR64 d)                                 { A(3); W(72|B(d), 0xD1, ModRM(3,5,d)); R64FXDBG; }
inline void SAR       (GPR64 d, Imm8 s)                         { A(4); W(72|B(d), 0xC1, ModRM(3,7,d)); IMM(s); R64FXDBG; }
inline void SAR       (GPR64 d)                                 { A(3); W(72|B(d), 0xD1, ModRM(3,7,d)); R64FXDBG; }

inline void SHL       (Mem64 d, Imm8 s)                         { A(8); W(72, 0xC1, ModRM(0,4,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }
inline void SHL       (Mem64 d)                                 { A(7); W(72, 0xD1, ModRM(0,4,5)); RIP(d.addr); R64FXDBG; }
inline void SHR       (Mem64 d, Imm8 s)                         { A(8); W(72, 0xC1, ModRM(0,5,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }
inline void SHR       (Mem64 d)                                 { A(7); W(72, 0xD1, ModRM(0,5,5)); RIP(d.addr); R64FXDBG; }
inline void SAR       (Mem64 d, Imm8 s)                         { A(8); W(72, 0xC1, ModRM(0,7,5)); RIP(d.addr-1); IMM(s); R64FXDBG; }
inline void SAR       (Mem64 d)                                 { A(7); W(72, 0xD1, ModRM(0,7,5)); RIP(d.addr); R64FXDBG; }

inline void SHL       (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0xC1, ModRM(0,4,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }
inline void SHL       (SIBD d)                                  { A(4+d.disp_size); W(72, 0xD1, ModRM(0,4,4), d.sib, d.sib); DISP(d); R64FXDBG; }
inline void SHR       (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0xC1, ModRM(0,5,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }
inline void SHR       (SIBD d)                                  { A(4+d.disp_size); W(72, 0xD1, ModRM(0,5,4), d.sib, d.sib); DISP(d); R64FXDBG; }
inline void SAR       (SIBD d, Imm8 s)                          { A(5+d.disp_size); W(72, 0xC1, ModRM(0,7,4), d.sib, d.sib); DISP(d); IMM(s); R64FXDBG; }
inline void SAR       (SIBD d)                                  { A(4+d.disp_size); W(72, 0xD1, ModRM(0,7,4), d.sib, d.sib); DISP(d); R64FXDBG; }

/* === SSE === */
inline void MOVUPS    (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x10, ModRM(3,d,s)); R64FXDBG; }
inline void MOVAPS    (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x28, ModRM(3,d,s)); R64FXDBG; }
inline void MOVSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x10, ModRM(3,d,s)); R64FXDBG; }

inline void MOVUPS    (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x10, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MOVUPS    (Mem128 d, Xmm s)                         { R(s); A(7,mp); W(0x0F); O<64>(mp); W(0x11, ModRM(0,s,5)); RIP(d.addr); R64FXDBG; }
inline void MOVAPS    (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x28, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MOVAPS    (Mem128 d, Xmm s)                         { R(s); A(7,mp); W(0x0F); O<64>(mp); W(0x29, ModRM(0,s,5)); RIP(d.addr); R64FXDBG; }

inline void MOVUPS    (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x10, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MOVUPS    (SIBD d, Xmm s)                           { RXB(s,d); A(4+d.disp_size,mp); W(0x0F); O<64>(mp); W(0x11, ModRM(d.mod,s,4), d.sib); DISP(d); R64FXDBG; }
inline void MOVAPS    (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x28, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MOVAPS    (SIBD d, Xmm s)                           { RXB(s,d); A(4+d.disp_size,mp); W(0x0F); O<64>(mp); W(0x29, ModRM(d.mod,s,4), d.sib); DISP(d); R64FXDBG; }

inline void MOVSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x10, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MOVSS     (Mem32 d, Xmm s)                          { R(s); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x11, ModRM(0,s,5)); RIP(d.addr); R64FXDBG; }

inline void MOVSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x10, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MOVSS     (SIBD d, Xmm s)                           { RXB(s,d); A(5+d.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x11, ModRM(d.mod,s,4), d.sib); DISP(d); R64FXDBG; }

inline void UNPCKLPS  (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x14, ModRM(3,d,s)); R64FXDBG; }
inline void UNPCKHPS  (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x15, ModRM(3,d,s)); R64FXDBG; }
inline void ANDPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x54, ModRM(3,d,s)); R64FXDBG; }
inline void ANDNPS    (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x55, ModRM(3,d,s)); R64FXDBG; }
inline void ORPS      (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x56, ModRM(3,d,s)); R64FXDBG; }
inline void XORPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x57, ModRM(3,d,s)); R64FXDBG; }

inline void UNPCKLPS  (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x14, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void UNPCKHPS  (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x15, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void ANDPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x54, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void ANDNPS    (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x55, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void ORPS      (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x56, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void XORPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x57, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }

inline void UNPCKLPS  (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x14, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void UNPCKHPS  (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x15, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void ANDPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x54, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void ANDNPS    (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x55, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void ORPS      (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x56, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void XORPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x57, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }

inline void CMPPS     (Xmm d, Xmm s, Imm8 imm)                  { RB(d,s); A(4,mp); W(0x0F); O<64>(mp); W(0xC2, ModRM(3,d,s)); IMM(imm); R64FXDBG; }
inline void SHUFPS    (Xmm d, Xmm s, Imm8 imm)                  { RB(d,s); A(4,mp); W(0x0F); O<64>(mp); W(0xC6, ModRM(3,d,s)); IMM(imm); R64FXDBG; }

inline void CMPPS     (Xmm d, Mem128 s, Imm8 imm)               { R(d); A(8,mp); W(0x0F); O<64>(mp); W(0xC2, ModRM(0,d,5)); RIP(s.addr-1); IMM(imm); R64FXDBG; }
inline void SHUFPS    (Xmm d, Mem128 s, Imm8 imm)               { R(d); A(8,mp); W(0x0F); O<64>(mp); W(0xC6, ModRM(0,d,5)); RIP(s.addr-1); IMM(imm); R64FXDBG; }

inline void CMPPS     (Xmm d, SIBD s, Imm8 imm)                 { RXB(d,s); A(5+s.disp_size,mp); W(0x0F); O<64>(mp); W(0xC2, ModRM(s.mod,d,4), s.sib); DISP(s); IMM(imm); R64FXDBG; }
inline void SHUFPS    (Xmm d, SIBD s, Imm8 imm)                 { RXB(d,s); A(5+s.disp_size,mp); W(0x0F); O<64>(mp); W(0xC6, ModRM(s.mod,d,4), s.sib); DISP(s); IMM(imm); R64FXDBG; }

inline void SQRTPS    (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x51, ModRM(3,d,s)); R64FXDBG; }
inline void RSQRTPS   (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x52, ModRM(3,d,s)); R64FXDBG; }
inline void RCPPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x53, ModRM(3,d,s)); R64FXDBG; }
inline void ADDPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x58, ModRM(3,d,s)); R64FXDBG; }
inline void MULPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x59, ModRM(3,d,s)); R64FXDBG; }
inline void SUBPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x5C, ModRM(3,d,s)); R64FXDBG; }
inline void DIVPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x5E, ModRM(3,d,s)); R64FXDBG; }
inline void MINPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x5D, ModRM(3,d,s)); R64FXDBG; }
inline void MAXPS     (Xmm d, Xmm s)                            { RB(d,s); A(3,mp); W(0x0F); O<64>(mp); W(0x5F, ModRM(3,d,s)); R64FXDBG; }

inline void SQRTPS    (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x51, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void RSQRTPS   (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x52, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void RCPPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x53, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void ADDPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x58, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MULPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x59, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void SUBPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x5C, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void DIVPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x5E, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MINPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x5D, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MAXPS     (Xmm d, Mem128 s)                         { R(d); A(7,mp); W(0x0F); O<64>(mp); W(0x5F, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }

inline void SQRTPS    (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x51, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void RSQRTPS   (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x52, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void RCPPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x53, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void ADDPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x58, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MULPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x59, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void SUBPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x5C, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void DIVPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x5E, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MINPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x5D, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MAXPS     (Xmm d, SIBD s)                           { RXB(d,s); A(4+s.disp_size,mp); W(0x0F); O<64>(mp); W(0x5F, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }

inline void SQRTSS    (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x51, ModRM(3,d,s)); R64FXDBG; }
inline void RSQRTSS   (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x52, ModRM(3,d,s)); R64FXDBG; }
inline void RCPSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x53, ModRM(3,d,s)); R64FXDBG; }
inline void ADDSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x58, ModRM(3,d,s)); R64FXDBG; }
inline void MULSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x59, ModRM(3,d,s)); R64FXDBG; }
inline void SUBSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5C, ModRM(3,d,s)); R64FXDBG; }
inline void DIVSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5E, ModRM(3,d,s)); R64FXDBG; }
inline void MINSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5D, ModRM(3,d,s)); R64FXDBG; }
inline void MAXSS     (Xmm d, Xmm s)                            { RB(d,s); A(4,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5F, ModRM(3,d,s)); R64FXDBG; }

inline void SQRTSS    (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x51, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void RSQRTSS   (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x52, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void RCPSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x53, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void ADDSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x58, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MULSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x59, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void SUBSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5C, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void DIVSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5E, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MINSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5D, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void MAXSS     (Xmm d, Mem32 s)                          { R(d); A(8,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5F, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }

inline void SQRTSS    (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x51, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void RSQRTSS   (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x52, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void RCPSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x53, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void ADDSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x58, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MULSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x59, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void SUBSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5C, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void DIVSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5E, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MINSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5D, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void MAXSS     (Xmm d, SIBD s)                           { RXB(d,s); A(5+s.disp_size,mp); W(0xF3, 0x0F); O<64>(mp); W(0x5F, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }

/* === AVX === */
inline void VMOVAPS   (Xmm d, Xmm s)                            { Vex(RB(d,s),1,0,0,0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x28, ModRM(3,d,s)); R64FXDBG; }
inline void VMOVAPS   (Ymm d, Ymm s)                            { Vex(RB(d,s),1,0,0,1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x28, ModRM(3,d,s)); R64FXDBG; }

inline void VMOVAPS   (Xmm d, Mem128 s)                         { Vex(R(d),1,0,0,0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x28, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VMOVAPS   (Mem128 d, Xmm s)                         { Vex(R(s),1,0,0,0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x29, ModRM(0,s,5)); RIP(d.addr); R64FXDBG; }
inline void VMOVAPS   (Ymm d, Mem128 s)                         { Vex(R(d),1,0,0,1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x28, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VMOVAPS   (Mem128 d, Ymm s)                         { Vex(R(s),1,0,0,1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x29, ModRM(0,s,5)); RIP(d.addr); R64FXDBG; }

inline void VMOVAPS   (Xmm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,0,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x28, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VMOVAPS   (SIBD d, Xmm s)                           { Vex(RXB(s,d),1,0,0,0,0,1); A(5+d.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x29, ModRM(d.mod,s,4), d.sib); DISP(d); R64FXDBG; }
inline void VMOVAPS   (Ymm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,1,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x28, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VMOVAPS   (SIBD d, Ymm s)                           { Vex(RXB(s,d),1,0,0,1,0,1); A(5+d.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x29, ModRM(d.mod,s,4), d.sib); DISP(d); R64FXDBG; }

inline void VCMPPS    (Xmm d, Xmm s1, Xmm s2, Imm8 imm)         { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(5,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC2, ModRM(3,d,s2)); IMM(imm); R64FXDBG; }
inline void VSHUFPS   (Xmm d, Xmm s1, Xmm s2, Imm8 imm)         { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(5,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC6, ModRM(3,d,s2)); IMM(imm); R64FXDBG; }

inline void VCMPPS    (Xmm d, Xmm s1, Mem128 s2, Imm8 imm)      { Vex(R(d),1,0,s1.bits(),0,0,1); A(9,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC2, ModRM(0,d,5)); RIP(s2.addr-1); IMM(imm); R64FXDBG; }
inline void VSHUFPS   (Xmm d, Xmm s1, Mem128 s2, Imm8 imm)      { Vex(R(d),1,0,s1.bits(),0,0,1); A(9,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC6, ModRM(0,d,5)); RIP(s2.addr-1); IMM(imm); R64FXDBG; }

inline void VCMPPS    (Xmm d, Xmm s1, SIBD s2, Imm8 imm)        { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(6+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC2, ModRM(s2.mod,d,4), s2.sib); DISP(s2); IMM(imm); R64FXDBG; }
inline void VSHUFPS   (Xmm d, Xmm s1, SIBD s2, Imm8 imm)        { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(6+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC6, ModRM(s2.mod,d,4), s2.sib); DISP(s2); IMM(imm); R64FXDBG; }

inline void VSQRTPS   (Xmm d, Xmm s)                            { Vex(RB(d,s),1,0,0,0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(3,d,s)); R64FXDBG; }
inline void VRSQRTPS  (Xmm d, Xmm s)                            { Vex(RB(d,s),1,0,0,0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(3,d,s)); R64FXDBG; }
inline void VRCPPS    (Xmm d, Xmm s)                            { Vex(RB(d,s),1,0,0,0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(3,d,s)); R64FXDBG; }
inline void VADDPS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(3,d,s2)); R64FXDBG; }
inline void VMULPS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(3,d,s2)); R64FXDBG; }
inline void VSUBPS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(3,d,s2)); R64FXDBG; }
inline void VDIVPS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(3,d,s2)); R64FXDBG; }
inline void VMINPS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(3,d,s2)); R64FXDBG; }
inline void VMAXPS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(3,d,s2)); R64FXDBG; }

inline void VSQRTPS   (Xmm d, Mem128 s)                         { Vex(R(d),1,0,0,0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VRSQRTPS  (Xmm d, Mem128 s)                         { Vex(R(d),1,0,0,0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VRCPPS    (Xmm d, Mem128 s)                         { Vex(R(d),1,0,0,0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VADDPS    (Xmm d, Xmm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMULPS    (Xmm d, Xmm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VSUBPS    (Xmm d, Xmm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VDIVPS    (Xmm d, Xmm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMINPS    (Xmm d, Xmm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMAXPS    (Xmm d, Xmm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),0,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }

inline void VSQRTPS   (Xmm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,0,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VRSQRTPS  (Xmm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,0,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VRCPPS    (Xmm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,0,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VADDPS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMULPS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VSUBPS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VDIVPS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMINPS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMAXPS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }

inline void VSQRTSS   (Xmm d, Xmm s)                            { Vex(RB(d,s),1,0,0,0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(3,d,s)); R64FXDBG; }
inline void VRSQRTSS  (Xmm d, Xmm s)                            { Vex(RB(d,s),1,0,0,0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(3,d,s)); R64FXDBG; }
inline void VRCPSS    (Xmm d, Xmm s)                            { Vex(RB(d,s),1,0,0,0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(3,d,s)); R64FXDBG; }
inline void VADDSS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(3,d,s2)); R64FXDBG; }
inline void VMULSS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(3,d,s2)); R64FXDBG; }
inline void VSUBSS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(3,d,s2)); R64FXDBG; }
inline void VDIVSS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(3,d,s2)); R64FXDBG; }
inline void VMINSS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(3,d,s2)); R64FXDBG; }
inline void VMAXSS    (Xmm d, Xmm s1, Xmm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),0,2,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(3,d,s2)); R64FXDBG; }

inline void VSQRTSS   (Xmm d, Mem32 s)                          { Vex(R(d),1,0,0,0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VRSQRTSS  (Xmm d, Mem32 s)                          { Vex(R(d),1,0,0,0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VRCPSS    (Xmm d, Mem32 s)                          { Vex(R(d),1,0,0,0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VADDSS    (Xmm d, Xmm s1, Mem32 s2)                 { Vex(R(d),1,0,s1.bits(),0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMULSS    (Xmm d, Xmm s1, Mem32 s2)                 { Vex(R(d),1,0,s1.bits(),0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VSUBSS    (Xmm d, Xmm s1, Mem32 s2)                 { Vex(R(d),1,0,s1.bits(),0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VDIVSS    (Xmm d, Xmm s1, Mem32 s2)                 { Vex(R(d),1,0,s1.bits(),0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMINSS    (Xmm d, Xmm s1, Mem32 s2)                 { Vex(R(d),1,0,s1.bits(),0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMAXSS    (Xmm d, Xmm s1, Mem32 s2)                 { Vex(R(d),1,0,s1.bits(),0,2,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }

inline void VSQRTSS   (Xmm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,0,2,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VRSQRTSS  (Xmm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,0,2,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VRCPSS    (Xmm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,0,2,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VADDSS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,2,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMULSS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,2,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VSUBSS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,2,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VDIVSS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,2,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMINSS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,2,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMAXSS    (Xmm d, Xmm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),0,2,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }

inline void VCMPPS    (Ymm d, Ymm s1, Ymm s2, Imm8 imm)         { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(5,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC2, ModRM(3,d,s2)); IMM(imm); R64FXDBG; }
inline void VSHUFPS   (Ymm d, Ymm s1, Ymm s2, Imm8 imm)         { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(5,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC6, ModRM(3,d,s2)); IMM(imm); R64FXDBG; }

inline void VCMPPS    (Ymm d, Ymm s1, Mem128 s2, Imm8 imm)      { Vex(R(d),1,0,s1.bits(),1,0,1); A(9,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC2, ModRM(0,d,5)); RIP(s2.addr-1); IMM(imm); R64FXDBG; }
inline void VSHUFPS   (Ymm d, Ymm s1, Mem128 s2, Imm8 imm)      { Vex(R(d),1,0,s1.bits(),1,0,1); A(9,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC6, ModRM(0,d,5)); RIP(s2.addr-1); IMM(imm); R64FXDBG; }

inline void VCMPPS    (Ymm d, Ymm s1, SIBD s2, Imm8 imm)        { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(6+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC2, ModRM(s2.mod,d,4), s2.sib); DISP(s2); IMM(imm); R64FXDBG; }
inline void VSHUFPS   (Ymm d, Ymm s1, SIBD s2, Imm8 imm)        { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(6+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xC6, ModRM(s2.mod,d,4), s2.sib); DISP(s2); IMM(imm); R64FXDBG; }

inline void VSQRTPS   (Ymm d, Ymm s)                            { Vex(RB(d,s),1,0,0,1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(3,d,s)); R64FXDBG; }
inline void VRSQRTPS  (Ymm d, Ymm s)                            { Vex(RB(d,s),1,0,0,1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(3,d,s)); R64FXDBG; }
inline void VRCPPS    (Ymm d, Ymm s)                            { Vex(RB(d,s),1,0,0,1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(3,d,s)); R64FXDBG; }
inline void VMULPS    (Ymm d, Ymm s1, Ymm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(3,d,s2)); R64FXDBG; }
inline void VSUBPS    (Ymm d, Ymm s1, Ymm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(3,d,s2)); R64FXDBG; }
inline void VADDPS    (Ymm d, Ymm s1, Ymm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(3,d,s2)); R64FXDBG; }
inline void VDIVPS    (Ymm d, Ymm s1, Ymm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(3,d,s2)); R64FXDBG; }
inline void VMINPS    (Ymm d, Ymm s1, Ymm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(3,d,s2)); R64FXDBG; }
inline void VMAXPS    (Ymm d, Ymm s1, Ymm s2)                   { Vex(RB(d,s2),1,0,s1.bits(),1,0,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(3,d,s2)); R64FXDBG; }

inline void VSQRTPS   (Ymm d, Mem128 s)                         { Vex(R(d),1,0,0,1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VRSQRTPS  (Ymm d, Mem128 s)                         { Vex(R(d),1,0,0,1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VRCPPS    (Ymm d, Mem128 s)                         { Vex(R(d),1,0,0,1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(0,d,5)); RIP(s.addr); R64FXDBG; }
inline void VMULPS    (Ymm d, Ymm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VSUBPS    (Ymm d, Ymm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VADDPS    (Ymm d, Ymm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VDIVPS    (Ymm d, Ymm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMINPS    (Ymm d, Ymm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VMAXPS    (Ymm d, Ymm s1, Mem128 s2)                { Vex(R(d),1,0,s1.bits(),1,0,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }

inline void VSQRTPS   (Ymm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,1,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x51, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VRSQRTPS  (Ymm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,1,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x52, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VRCPPS    (Ymm d, SIBD s)                           { Vex(RXB(d,s),1,0,0,1,0,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x53, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VMULPS    (Ymm d, Ymm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x59, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VSUBPS    (Ymm d, Ymm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5C, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VADDPS    (Ymm d, Ymm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x58, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VDIVPS    (Ymm d, Ymm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5E, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMINPS    (Ymm d, Ymm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5D, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VMAXPS    (Ymm d, Ymm s1, SIBD s2)                  { Vex(RXB(d,s2),1,0,s1.bits(),1,0,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x5F, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }

/* === Gather Instructions === */
inline void VGATHERDPS   (Xmm d, XSIBD s, Xmm m)                { Vex(RXB(d,s),2,0,m.bits(),0,1,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x92, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VGATHERDPD   (Xmm d, XSIBD s, Xmm m)                { Vex(RXB(d,s),2,1,m.bits(),0,1,0); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x92, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VGATHERQPS   (Xmm d, XSIBD s, Xmm m)                { Vex(RXB(d,s),2,0,m.bits(),0,1,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x93, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VGATHERQPD   (Xmm d, XSIBD s, Xmm m)                { Vex(RXB(d,s),2,1,m.bits(),0,1,0); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x93, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VGATHERDPS   (Ymm d, YSIBD s, Ymm m)                { Vex(RXB(d,s),2,0,m.bits(),1,1,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x92, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VGATHERDPD   (Ymm d, YSIBD s, Ymm m)                { Vex(RXB(d,s),2,1,m.bits(),1,1,0); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x92, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VGATHERQPS   (Ymm d, YSIBD s, Ymm m)                { Vex(RXB(d,s),2,0,m.bits(),1,1,1); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x93, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }
inline void VGATHERQPD   (Ymm d, YSIBD s, Ymm m)                { Vex(RXB(d,s),2,1,m.bits(),1,1,0); A(5+s.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x93, ModRM(s.mod,d,4), s.sib); DISP(s); R64FXDBG; }

/* === FMA ===*/
inline void VFMADD132PS   (Xmm d, Xmm s1, Xmm s2)               { Vex(RB(d,s2),2,0,s1.bits(),0,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x98, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMADD213PS   (Xmm d, Xmm s1, Xmm s2)               { Vex(RB(d,s2),2,0,s1.bits(),0,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xA8, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMADD231PS   (Xmm d, Xmm s1, Xmm s2)               { Vex(RB(d,s2),2,0,s1.bits(),0,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xB8, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMSUB132PS   (Xmm d, Xmm s1, Xmm s2)               { Vex(RB(d,s2),2,0,s1.bits(),0,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x9A, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMSUB213PS   (Xmm d, Xmm s1, Xmm s2)               { Vex(RB(d,s2),2,0,s1.bits(),0,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xAA, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMSUB231PS   (Xmm d, Xmm s1, Xmm s2)               { Vex(RB(d,s2),2,0,s1.bits(),0,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xBA, ModRM(3,d,s2)); R64FXDBG; }

inline void VFMADD132PS   (Xmm d, Xmm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),0,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x98, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMADD213PS   (Xmm d, Xmm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),0,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xA8, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMADD231PS   (Xmm d, Xmm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),0,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xB8, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMSUB132PS   (Xmm d, Xmm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),0,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x9A, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMSUB213PS   (Xmm d, Xmm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),0,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xAA, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMSUB231PS   (Xmm d, Xmm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),0,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xBA, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }

inline void VFMADD132PS   (Xmm d, Xmm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),0,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x98, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMADD213PS   (Xmm d, Xmm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),0,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xA8, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMADD231PS   (Xmm d, Xmm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),0,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xB8, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMSUB132PS   (Xmm d, Xmm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),0,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x9A, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMSUB213PS   (Xmm d, Xmm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),0,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xAA, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMSUB231PS   (Xmm d, Xmm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),0,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xBA, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }

inline void VFMADD132PS   (Ymm d, Ymm s1, Ymm s2)               { Vex(RB(d,s2),2,0,s1.bits(),1,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x98, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMADD213PS   (Ymm d, Ymm s1, Ymm s2)               { Vex(RB(d,s2),2,0,s1.bits(),1,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xA8, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMADD231PS   (Ymm d, Ymm s1, Ymm s2)               { Vex(RB(d,s2),2,0,s1.bits(),1,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xB8, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMSUB132PS   (Ymm d, Ymm s1, Ymm s2)               { Vex(RB(d,s2),2,0,s1.bits(),1,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x9A, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMSUB213PS   (Ymm d, Ymm s1, Ymm s2)               { Vex(RB(d,s2),2,0,s1.bits(),1,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xAA, ModRM(3,d,s2)); R64FXDBG; }
inline void VFMSUB231PS   (Ymm d, Ymm s1, Ymm s2)               { Vex(RB(d,s2),2,0,s1.bits(),1,1,1); A(4,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xBA, ModRM(3,d,s2)); R64FXDBG; }

inline void VFMADD132PS   (Ymm d, Ymm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),1,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x98, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMADD213PS   (Ymm d, Ymm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),1,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xA8, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMADD231PS   (Ymm d, Ymm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),1,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xB8, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMSUB132PS   (Ymm d, Ymm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),1,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x9A, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMSUB213PS   (Ymm d, Ymm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),1,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xAA, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }
inline void VFMSUB231PS   (Ymm d, Ymm s1, Mem128 s2)            { Vex(R(d),2,0,s1.bits(),1,1,1); A(8,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xBA, ModRM(0,d,5)); RIP(s2.addr); R64FXDBG; }

inline void VFMADD132PS   (Ymm d, Ymm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),1,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x98, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMADD213PS   (Ymm d, Ymm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),1,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xA8, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMADD231PS   (Ymm d, Ymm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),1,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xB8, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMSUB132PS   (Ymm d, Ymm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),1,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0x9A, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMSUB213PS   (Ymm d, Ymm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),1,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xAA, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }
inline void VFMSUB231PS   (Ymm d, Ymm s1, SIBD s2)              { Vex(RXB(d,s2),2,0,s1.bits(),1,1,1); A(5+s2.disp_size,mp&0xFF0000); O(mp>>16); W((mp>>8)&0xFF, mp&0xFF, 0xBA, ModRM(s2.mod,d,4), s2.sib); DISP(s2); R64FXDBG; }

