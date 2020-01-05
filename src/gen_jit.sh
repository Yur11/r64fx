#!/bin/bash
function gen_jit {

instr 'RET       ()'                                 C3
instr 'NOP       ()'                                 90
echo ''

instr 'PUSH      (Imm32 i)'                          68 id
instr 'PUSH      (Mem64 m)'                          FF /6
instr 'POP       (Mem64 m)'                          8F /0
echo ''

instr 'JNO       (JumpLabel8 &j)'                    71 cb
instr 'JB        (JumpLabel8 &j)'                    72 cb
instr 'JNAE      (JumpLabel8 &j)'                    72 cb
instr 'JAE       (JumpLabel8 &j)'                    73 cb
instr 'JNB       (JumpLabel8 &j)'                    73 cb
instr 'JNC       (JumpLabel8 &j)'                    73 cb
instr 'JE        (JumpLabel8 &j)'                    74 cb
instr 'JZ        (JumpLabel8 &j)'                    74 cb
instr 'JNE       (JumpLabel8 &j)'                    75 cb
instr 'JNZ       (JumpLabel8 &j)'                    75 cb
instr 'JBE       (JumpLabel8 &j)'                    76 cb
instr 'JNA       (JumpLabel8 &j)'                    76 cb
instr 'JS        (JumpLabel8 &j)'                    78 cb
instr 'JA        (JumpLabel8 &j)'                    77 cb
instr 'JNBE      (JumpLabel8 &j)'                    77 cb
instr 'JP        (JumpLabel8 &j)'                    7A cb
instr 'JPE       (JumpLabel8 &j)'                    7A cb
instr 'JNP       (JumpLabel8 &j)'                    7B cb
instr 'JPO       (JumpLabel8 &j)'                    7B cb
instr 'JL        (JumpLabel8 &j)'                    7C cb
instr 'JNGE      (JumpLabel8 &j)'                    7C cb
instr 'JGE       (JumpLabel8 &j)'                    7D cb
instr 'JNL       (JumpLabel8 &j)'                    7D cb
instr 'JLE       (JumpLabel8 &j)'                    7E cb
instr 'JNG       (JumpLabel8 &j)'                    7E cb
instr 'JG        (JumpLabel8 &j)'                    7F cb
instr 'JNLE      (JumpLabel8 &j)'                    7F cb
instr 'JCXZ      (JumpLabel8 &j)'                    E3 cb
echo ''

instr 'JNE (JumpLabel32 &j)'                         0F 85 cd
echo ''

instr 'MOV       (GPR32 d, Imm32 s)'                 B8+rd id
instr 'MOV       (GPR64 d, Imm64 s)'                 REX.W + B8+rd io
instr 'MOV       (GPR64 d, GPR64 s)'                 REX.W + 8B /r
instr 'MOV       (GPR64 d, Mem64 s)'                 REX.W + 8B /r
instr 'MOV       (GPR64 d, SIBD  s)'                 REX.W + 8B /r
echo ''

for op in GPR64 Mem64 SIBD; do
    instr "ADD       ($op d, Imm8 s)"                REX.W + 83 /0 ib
    instr "OR        ($op d, Imm8 s)"                REX.W + 83 /1 ib
    instr "AND       ($op d, Imm8 s)"                REX.W + 83 /4 ib
    instr "SUB       ($op d, Imm8 s)"                REX.W + 83 /5 ib
    instr "XOR       ($op d, Imm8 s)"                REX.W + 83 /6 ib
    echo
done

for op in GPR64 Mem64 SIBD; do
    instr "ADD       (GPR64 d, $op s)"               REX.W + 03 /r
    instr "OR        (GPR64 d, $op s)"               REX.W + 0B /r
    instr "AND       (GPR64 d, $op s)"               REX.W + 23 /r
    instr "SUB       (GPR64 d, $op s)"               REX.W + 2B /r
    instr "XOR       (GPR64 d, $op s)"               REX.W + 33 /r
    echo ''
done

echo '/* === SSE === */'
instr "MOVUPS    (Xmm d, Xmm s)"                     NP 0F 10 /r
instr "MOVAPS    (Xmm d, Xmm s)"                     NP 0F 28 /r
instr "MOVSS     (Xmm d, Xmm s)"                     F3 0F 10 /r
echo ''

for op in Mem128 SIBD; do
    instr "MOVUPS    (Xmm d, $op s)"                 NP 0F 10 /r
    instr "MOVUPS    ($op d, Xmm s)"                 NP 0F 11 /r
    instr "MOVAPS    (Xmm d, $op s)"                 NP 0F 28 /r
    instr "MOVAPS    ($op d, Xmm s)"                 NP 0F 29 /r
    echo ''
done

for op in Mem32 SIBD; do
    instr "MOVSS     (Xmm d, $op s)"                 F3 0F 10 /r
    instr "MOVSS     ($op d, Xmm s)"                 F3 0F 11 /r
    echo ''
done

for op in Xmm Mem128 SIBD; do
    instr "UNPCKLPS  (Xmm d, $op s)"                 NP 0F 14 /r ib
    instr "UNPCKHPS  (Xmm d, $op s)"                 NP 0F 15 /r ib
    instr "ANDPS     (Xmm d, $op s)"                 NP 0F 54 /r
    instr "ANDNPS    (Xmm d, $op s)"                 NP 0F 55 /r
    instr "ORPS      (Xmm d, $op s)"                 NP 0F 56 /r
    instr "XORPS     (Xmm d, $op s)"                 NP 0F 57 /r
    echo ''
done

for op in Xmm Mem128 SIBD; do
    instr "CMPPS     (Xmm d, $op s, Imm8 imm)"       NP 0F C2 /r ib
    instr "SHUFPS    (Xmm d, $op s, Imm8 imm)"       NP 0F C6 /r ib
    echo ''
done

for op in Xmm Mem128 SIBD; do
    instr "SQRTPS    (Xmm d, $op s)"                 NP 0F 51 /r
    instr "RSQRTPS   (Xmm d, $op s)"                 NP 0F 52 /r
    instr "RCPPS     (Xmm d, $op s)"                 NP 0F 53 /r
    instr "ADDPS     (Xmm d, $op s)"                 NP 0F 58 /r
    instr "MULPS     (Xmm d, $op s)"                 NP 0F 59 /r
    instr "SUBPS     (Xmm d, $op s)"                 NP 0F 5C /r
    instr "DIVPS     (Xmm d, $op s)"                 NP 0F 5E /r
    instr "MINPS     (Xmm d, $op s)"                 NP 0F 5D /r
    instr "MAXPS     (Xmm d, $op s)"                 NP 0F 5F /r
    echo ''
done

for op in Xmm Mem32 SIBD; do
    instr "SQRTSS    (Xmm d, $op s)"                 F3 0F 51 /r
    instr "RSQRTSS   (Xmm d, $op s)"                 F3 0F 52 /r
    instr "RCPSS     (Xmm d, $op s)"                 F3 0F 53 /r
    instr "ADDSS     (Xmm d, $op s)"                 F3 0F 58 /r
    instr "MULSS     (Xmm d, $op s)"                 F3 0F 59 /r
    instr "SUBSS     (Xmm d, $op s)"                 F3 0F 5C /r
    instr "DIVSS     (Xmm d, $op s)"                 F3 0F 5E /r
    instr "MINSS     (Xmm d, $op s)"                 F3 0F 5D /r
    instr "MAXSS     (Xmm d, $op s)"                 F3 0F 5F /r
    echo ''
done

echo "/* === AVX === */"
instr 'VMOVAPS   (Xmm d, Xmm s)'                     VEX.128.0F.WIG 28 /r
instr 'VMOVAPS   (Ymm d, Ymm s)'                     VEX.256.0F.WIG 28 /r
echo ''

for op in Mem128 SIBD; do
    instr "VMOVAPS   (Xmm d, $op s)"                 VEX.128.0F.WIG 28 /r
    instr "VMOVAPS   ($op d, Xmm s)"                 VEX.128.0F.WIG 29 /r
    instr "VMOVAPS   (Ymm d, $op s)"                 VEX.256.0F.WIG 28 /r
    instr "VMOVAPS   ($op d, Ymm s)"                 VEX.256.0F.WIG 29 /r
    echo ''
done

for op in Xmm Mem128 SIBD; do
    instr "VCMPPS     (Xmm d, Xmm s1, $op s2, Imm8 imm)"\
                                                     VEX.NDS.128.0F.WIG C2 /r ib
    echo ''
done

for op in Xmm Mem128 SIBD; do
    instr "VSQRTPS    (Xmm d, $op s)         "       VEX.128.0F.WIG 51 /r
    instr "VRSQRTPS   (Xmm d, $op s)         "       VEX.128.0F.WIG 52 /r
    instr "VRCPPS     (Xmm d, $op s)         "       VEX.128.0F.WIG 53 /r
    instr "VADDPS     (Xmm d, Xmm s1, $op s2)"       VEX.128.0F.WIG 58 /r
    instr "VMULPS     (Xmm d, Xmm s1, $op s2)"       VEX.128.0F.WIG 59 /r
    instr "VSUBPS     (Xmm d, Xmm s1, $op s2)"       VEX.128.0F.WIG 5C /r
    instr "VDIVPS     (Xmm d, Xmm s1, $op s2)"       VEX.128.0F.WIG 5E /r
    instr "VMINPS     (Xmm d, Xmm s1, $op s2)"       VEX.128.0F.WIG 5D /r
    instr "VMAXPS     (Xmm d, Xmm s1, $op s2)"       VEX.128.0F.WIG 5F /r
    echo ''
done

for op in Xmm Mem32 SIBD; do
    instr "VSQRTSS    (Xmm d, $op s)         "       VEX.NDS.LIG.F3.0F.WIG 51 /r
    instr "VRSQRTSS   (Xmm d, $op s)         "       VEX.NDS.LIG.F3.0F.WIG 52 /r
    instr "VRCPSS     (Xmm d, $op s)         "       VEX.NDS.LIG.F3.0F.WIG 53 /r
    instr "VADDSS     (Xmm d, Xmm s1, $op s2)"       VEX.NDS.LIG.F3.0F.WIG 58 /r
    instr "VMULSS     (Xmm d, Xmm s1, $op s2)"       VEX.NDS.LIG.F3.0F.WIG 59 /r
    instr "VSUBSS     (Xmm d, Xmm s1, $op s2)"       VEX.NDS.LIG.F3.0F.WIG 5C /r
    instr "VDIVSS     (Xmm d, Xmm s1, $op s2)"       VEX.NDS.LIG.F3.0F.WIG 5E /r
    instr "VMINSS     (Xmm d, Xmm s1, $op s2)"       VEX.NDS.LIG.F3.0F.WIG 5D /r
    instr "VMAXSS     (Xmm d, Xmm s1, $op s2)"       VEX.NDS.LIG.F3.0F.WIG 5F /r
    echo ''
done

for op in Ymm Mem128 SIBD; do
    instr "VCMPPS     (Ymm d, Ymm s1, $op s2, Imm8 imm)"\
                                                     VEX.NDS.256.0F.WIG C2 /r ib
    echo ''
done

for op in Ymm Mem128 SIBD; do
    instr "VSQRTPS    (Ymm d, $op s)         "       VEX.256.0F.WIG 51 /r
    instr "VRSQRTPS   (Ymm d, $op s)         "       VEX.256.0F.WIG 52 /r
    instr "VRCPPS     (Ymm d, $op s)         "       VEX.256.0F.WIG 53 /r
    instr "VMULPS     (Ymm d, Ymm s1, $op s2)"       VEX.NDS.256.0F.WIG 59 /r
    instr "VSUBPS     (Ymm d, Ymm s1, $op s2)"       VEX.NDS.256.0F.WIG 5C /r
    instr "VADDPS     (Ymm d, Ymm s1, $op s2)"       VEX.NDS.256.0F.WIG 58 /r
    instr "VDIVPS     (Ymm d, Ymm s1, $op s2)"       VEX.NDS.256.0F.WIG 5E /r
    instr "VMINPS     (Ymm d, Ymm s1, $op s2)"       VEX.NDS.256.0F.WIG 5D /r
    instr "VMAXPS     (Ymm d, Ymm s1, $op s2)"       VEX.NDS.256.0F.WIG 5F /r
    echo ''
done

} # gen_jit


function instr
{
    local i=''

    # Parse encoding string
    local enc_vex=''
    local enc_rexw=''
    local enc_pref_bytes=()
    local enc_opcode_byte=''
    local enc_modrm_reg_field=''
    local enc_add_reg_code=''
    local imm_size=''

    local signature=$1; shift
    local dbg="$signature [$@]:"

    if [[ "$1" == 'REX.W' && "$2" == '+' ]]; then
        enc_rexw=1; shift; shift
    fi

    IFS=' '; for i in $@; do
        [[ $i == 'NP' ]] && continue

        if [[ "${i:0:4}" == 'VEX.' ]]; then
            enc_vex="${i:4}"
        else
            if [[ "${i:0:1}" == '/' ]]; then
                enc_modrm_reg_field="${i:1}"
            elif [[ ${i:0:1} == 'i' || ${i:0:1} == 'c' ]]; then
                case "${i:1:1}" in
                    b) imm_size=1 ;;
                    w) imm_size=2 ;;
                    d) imm_size=4 ;;
                    o) imm_size=8 ;;

                    *)
                        die "$dbg Bad immediate type!"
                    ;;
                esac
            else
                if [[ $i == '0F' || $i == '66' || $i == 'F2' || $i == 'F3' ]]; then
                    enc_pref_bytes+=("$i")
                else
                    [[ -z "$enc_opcode_byte" ]] ||
                        die "$dbg Multiple opcode bytes '$enc_opcode_byte'!"

                    if [[ "${i:2:2}" == "+r" ]]; then
                        # Register code is added to the ocode bytes
                        enc_opcode_byte="${i%+*}"
                        enc_add_reg_code="${i#*+}"
                    else
                        enc_opcode_byte="$i"
                    fi
                fi
            fi
        fi
    done

    # Parse signature string, extract operands
    local op_r=()
    local op_mem=''
    local op_sibd=''
    local op_imm=''
    local op_jmp=''
    local out_w=0

    local a=${signature##*(}; a=${a%%)*}
    IFS=','; for i in $a; do
        operand="${i#' '}"
        operand_name=${operand##*' '}
        operand_type="${operand%%' '*}"

        case "$operand_type" in
            Xmm|Ymm|Zmm|GPR*)
                op_r+=("$operand_name")
                [[ "$operand_type" == GPR64 ]] && out_w=8
            ;;

            Mem*)
                [[ -z "$op_mem" ]] ||
                    die "$dbg Multiple Mem* operands!"

                op_mem="$operand_name"
            ;;

            SIBD)
                [[ -z "$op_sibd" ]] ||
                    die "$dbg Multiple SIBD operands!"

                op_sibd="$operand_name"
            ;;

            Imm*)
                [[ -z "$op_imm" ]] ||
                    die "$dbg Multiple Imm* operands!"

                [[ -n "$imm_size" ]] ||
                    die "$dbg Immediate operand not specified in encoding!"

                [[ "$operand_type" == "Imm$(( $imm_size * 8 ))" ]] ||
                    die "$dbg Bad immediate type specified in encoding!"

                op_imm="$operand_name"
            ;;

            JumpLabel*)
                op_jmp="${operand_name/&/}"
            ;;

            *)
                die "$dbg: Bad operand type '$operand'!"
            ;;
        esac
    done;

    local op_r1=''
    local op_r2=''
    local op_vv=''

    [[ ${#op_r[@]} -le 3 ]] || die "$dbg Too many register operands!"
    [[ ${#op_r[@]} -ge 1 ]] && op_r1=${op_r[0]}
    [[ ${#op_r[@]} -eq 2 ]] && op_r2=${op_r[1]}
    [[ ${#op_r[@]} -eq 3 ]] && { op_r2=${op_r[2]}; op_vv=${op_r[1]}; }


    # Calculate instruction size

    # Opcode + Mandatory prefix bytes
    local out_nbytes=$(( 1 + ${#enc_pref_bytes[@]} ))

    # ModRM byte
    [[ -n "$enc_modrm_reg_field" ]] && : $(( out_nbytes++ ))

    # SIB byte + dynamically added displacement
    [[ -n "$op_sibd" ]] && : $(( out_nbytes++ ))

    # RIP memory offset
    [[ -n "$op_mem" ]] && : $(( out_nbytes+=4 ))

    # Immediate
    [[ -n "$op_imm" || -n "$op_jmp" ]] && : $(( out_nbytes+=$imm_size ))


    # Compute REX/VEX/XOP prefixes & adjust instruction size
    local out_r=''
    local out_b=''
    local out_rb=''
    local out_rxb=''
    local out_vex=''

    if [[ -n "$op_r1" ]]; then
        if [[ -n "$op_r2" ]]; then
            out_rb="$op_r1,$op_r2"
        elif [[ -n "$op_sibd" ]]; then
            out_rxb="$op_r1,$op_sibd"
        elif [[ -n "$enc_modrm_reg_field" ]]; then
            if [[ "$enc_modrm_reg_field" == 'r' ]]; then
                out_r="$op_r1"
            else
                out_b="$op_r1"
            fi
        elif [[ -n "$enc_add_reg_code" ]]; then
            out_b="$op_r1"
        fi
    fi

    local may_use_c5_vex=0
    if [[ -n "$enc_vex" ]]; then
        # Parse VEX encoding
        local vex_map_select=''
        local vex_w=0
        local vex_l=0
        local vex_pp=0

        IFS='.'; for i in $enc_vex; do
            case $i in
                W*)
                    if [[ $i == W1 ]]; then
                        vex_w=1
                    else
                        may_use_c5_vex=1
                        if [[ $i == W0 ]]; then
                            vex_w=0
                        elif [[ $i != WIG ]]; then
                            die "$dbg Bad VEX encoding '$i'!"
                        fi
                    fi
                ;;

                NDS|NDD|DDS)
                    :
                ;;

                0F)   vex_map_select=1 ;;
                0F38) vex_map_select=2 ;;
                0F3A) vex_map_select=3 ;;

                128|LIG) vex_l=0 ;;
                256) vex_l=1 ;;

                66) vex_pp=1 ;;
                F3) vex_pp=2 ;;
                F2) vex_pp=3 ;;

                *)
                    die "$dbg Bad VEX encoding '$i'!"
                ;;
            esac
        done

        [[ -n "$vex_map_select" ]] ||
            die "$dbg No map select!"

        local vex_vvvv_reg=''
        local vex_vvvv_const=''

        # If VEX.vvvv is not used it must be set to 1111 or instruction will #UD!
        if [[ -n "$op_vv" ]]; then
            vex_vvvv_reg="$op_vv.bits()"
            vex_vvvv_const=''
        else
            vex_vvvv_const=0
        fi

        local rxb=''
        if   [[ -n "$out_r" ]]; then
            rxb="R($out_r)"
        elif [[ -n "$out_b" ]]; then
            rxb="B($out_b)"
        elif [[ -n "$out_rb"  ]]; then
            rxb="RB($out_rb)"
        elif [[ -n "$out_rxb" ]]; then
            rxb="RXB($out_rxb)"
        fi

        out_vex="$rxb,$vex_map_select,$vex_w,$vex_vvvv_reg$vex_vvvv_const,$vex_l,$vex_pp,$may_use_c5_vex"

        if [[ -n "$may_use_c5_vex" ]]; then
            : $(( out_nbytes+=2 ))
        else
            : $(( out_nbytes+=3 ))
        fi
    else
        [[ -n "$enc_rexw" ]] && : $(( out_nbytes++ )) # Static REX
    fi


    # Generate Output
    local out="inline void $signature "
    while [[ ${#out} -lt 52 ]]; do
        out+=' '
    done
    out+="{ "

    # VEX/REX
    if [[ -z "$enc_rexw" ]]; then
        if [[ -n "$out_vex" ]]; then
            out+="Vex($out_vex); "
        else
            if   [[ -n "$out_r"   ]]; then
                out+="R($out_r); "
            elif [[ -n "$out_b"   ]]; then
                out+="B($out_b); "
            elif [[ -n "$out_rb"  ]]; then
                out+="RB($out_rb); "
            elif [[ -n "$out_rxb" ]]; then
                out+="RXB($out_rxb); "
            fi
        fi
    fi

    # Allocate instruction bytes
    out+="A($out_nbytes"

    if [[ -n "$op_sibd" ]]; then
        out+="+$op_sibd.disp_size"
    fi

    if [[ -n "$out_vex" && -n "$may_use_c5_vex" ]]; then
        out+=',m_pref&0xFF0000'
    elif [[ -z "$enc_rexw" ]] && [[ -n "$out_r$out_b$out_rb$out_rxb" ]]; then
        out+=',m_pref'
    fi

    out+=')'


    # Opcode prefix bytes string
    local out_pref=''
    for i in ${enc_pref_bytes[@]}; do
        out_pref+="0x$i, "
    done

    if [[ -n "$out_pref" ]]; then
        out+="; W(${out_pref%', '}"
    fi


    # VEX or REX prefixes
    if [[ -n "$enc_rexw" ]]; then
        # Mandatory REX prefix byte
        [[ -n "$out_pref" ]] && out+=', ' || out+='; W('
        out+='72'
        if   [[ -n "$out_r"   ]]; then
            out+="|R($out_r), "
        elif [[ -n "$out_b"   ]]; then
            out+="|B($out_b), "
        elif [[ -n "$out_rb"  ]]; then
            out+="|RB($out_rb), "
        elif [[ -n "$out_rxb" ]]; then
            out+="|RXB($out_rxb), "
        else
            out+=', '
        fi
    elif [[ -n "$out_vex"  ]]; then
        out+='; O(m_pref>>16); W((m_pref>>8)&0xFF, m_pref&0xFF, '
    elif [[ -n "$out_r$out_b$out_rb$out_rxb" ]]; then
        [[ -n "$out_pref" ]] && out+=')'
        out+='; O<64>(m_pref); W('
    elif [[ -n "$out_pref" ]]; then
        out+=', '
    else
        out+='; W('
    fi


    # Write opcode, modrm & sib bytes
    out+="0x$enc_opcode_byte"
    if [[ -n "$op_r1" ]]; then
        if [[ -n "$op_mem" ]]; then
            out+=", ModRM(0,$op_r1,5)"
        elif [[ -n "$op_sibd" ]]; then
            out+=", ModRM($op_sibd.mod,$op_r1,4)"
        elif [[ -n "$op_r2" ]]; then
            out+=", ModRM(3,$op_r1,$op_r2)"
        else
            if [[ -n "$enc_add_reg_code" ]]; then
                out+="+$op_r1"
            elif [[ -n "$enc_modrm_reg_field" ]]; then
                out+=", ModRM(3,$enc_modrm_reg_field,$op_r1)"
            else
                die "$dbg Bad encoding!"
            fi
        fi
    else
        if [[ -n "$op_mem" ]]; then
            out+=", ModRM(0,$enc_modrm_reg_field,5)"
        elif [[ -n "$op_sibd" ]]; then
            out+=", ModRM(0,$enc_modrm_reg_field,4), $op_sibd.sib"
        fi
    fi

    if [[ $op_sibd ]]; then
        out+=", $op_sibd.sib"
    fi
    out+=')'


    # Write displacement and immediate
    if [[ -n "$op_mem" ]]; then
        out+="; RIP($op_mem.addr"
        if [[ -n "$op_imm" ]]; then
            out+="-$imm_size" #Adjust for immediate
        fi
        out+=')'
    elif [[ -n "$op_sibd" ]]; then
        out+="; DISP($op_sibd)"
    fi

    # Write immediate
    if [[ -n "$op_imm" ]]; then
        out+="; IMM($op_imm)"
    fi

    # Write jump JumpLabel
    if [[ -n "$op_jmp" ]]; then
        out+="; LABEL($op_jmp)"
    fi

    echo "$out; }"
}

function die
{
    [[ -n "$1" ]] && >&2 echo "$1"
    exit 1
}

gen_jit
exit 0
