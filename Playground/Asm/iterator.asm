    .file   "main.cpp"
# GNU C++ (GCC) version 4.8.0 (x86_64-unknown-linux-gnu)
#   compiled by GNU C version 4.7.2, GMP version 5.0.2, MPFR version 3.1.0-p3, MPC version 0.9
# GGC heuristics: --param ggc-min-expand=30 --param ggc-min-heapsize=4096
# options passed:  -imultilib . -imultiarch x86_64-linux-gnu -D_GNU_SOURCE
# -D USE_ITERATOR main.cpp -mtune=generic -march=x86-64 -auxbase-strip -
# -O3 -fverbose-asm
# options enabled:  -faggressive-loop-optimizations
# -fasynchronous-unwind-tables -fauto-inc-dec -fbranch-count-reg
# -fcaller-saves -fcombine-stack-adjustments -fcommon -fcompare-elim
# -fcprop-registers -fcrossjumping -fcse-follow-jumps -fdefer-pop
# -fdelete-null-pointer-checks -fdevirtualize -fdwarf2-cfi-asm
# -fearly-inlining -feliminate-unused-debug-types -fexceptions
# -fexpensive-optimizations -fforward-propagate -ffunction-cse -fgcse
# -fgcse-after-reload -fgcse-lm -fgnu-runtime -fguess-branch-probability
# -fhoist-adjacent-loads -fident -fif-conversion -fif-conversion2
# -findirect-inlining -finline -finline-atomics -finline-functions
# -finline-functions-called-once -finline-small-functions -fipa-cp
# -fipa-cp-clone -fipa-profile -fipa-pure-const -fipa-reference -fipa-sra
# -fira-hoist-pressure -fira-share-save-slots -fira-share-spill-slots
# -fivopts -fkeep-static-consts -fleading-underscore -fmath-errno
# -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
# -fomit-frame-pointer -foptimize-register-move -foptimize-sibling-calls
# -foptimize-strlen -fpartial-inlining -fpeephole -fpeephole2
# -fpredictive-commoning -fprefetch-loop-arrays -free -freg-struct-return
# -fregmove -freorder-blocks -freorder-functions -frerun-cse-after-loop
# -fsched-critical-path-heuristic -fsched-dep-count-heuristic
# -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
# -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
# -fsched-stalled-insns-dep -fschedule-insns2 -fshow-column -fshrink-wrap
# -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types
# -fstrict-aliasing -fstrict-overflow -fstrict-volatile-bitfields
# -fsync-libcalls -fthread-jumps -ftoplevel-reorder -ftrapping-math
# -ftree-bit-ccp -ftree-builtin-call-dce -ftree-ccp -ftree-ch
# -ftree-coalesce-vars -ftree-copy-prop -ftree-copyrename -ftree-cselim
# -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre
# -ftree-loop-distribute-patterns -ftree-loop-if-convert -ftree-loop-im
# -ftree-loop-ivcanon -ftree-loop-optimize -ftree-parallelize-loops=
# -ftree-partial-pre -ftree-phiprop -ftree-pre -ftree-pta -ftree-reassoc
# -ftree-scev-cprop -ftree-sink -ftree-slp-vectorize -ftree-slsr -ftree-sra
# -ftree-switch-conversion -ftree-tail-merge -ftree-ter
# -ftree-vect-loop-version -ftree-vectorize -ftree-vrp -funit-at-a-time
# -funswitch-loops -funwind-tables -fvect-cost-model -fverbose-asm
# -fzero-initialized-in-bss -m128bit-long-double -m64 -m80387
# -maccumulate-outgoing-args -malign-stringops -mfancy-math-387
# -mfp-ret-in-387 -mglibc -mieee-fp -mlong-double-80 -mmmx -mno-sse4
# -mpush-args -mred-zone -msse -msse2 -mtls-direct-seg-refs

    .section    .text._ZNSt6vectorIiSaIiEED2Ev,"axG",@progbits,_ZNSt6vectorIiSaIiEED5Ev,comdat
    .align 2
    .p2align 4,,15
    .weak   _ZNSt6vectorIiSaIiEED2Ev
    .type   _ZNSt6vectorIiSaIiEED2Ev, @function
_ZNSt6vectorIiSaIiEED2Ev:
.LFB460:
    .cfi_startproc
    movq    (%rdi), %rdi    # MEM[(struct _Vector_base *)this_1(D)]._M_impl._M_start, D.8837
    testq   %rdi, %rdi  # D.8837
    je  .L1 #,
    jmp _ZdlPv  #
    .p2align 4,,10
    .p2align 3
.L1:
    rep; ret
    .cfi_endproc
.LFE460:
    .size   _ZNSt6vectorIiSaIiEED2Ev, .-_ZNSt6vectorIiSaIiEED2Ev
    .weak   _ZNSt6vectorIiSaIiEED1Ev
    .set    _ZNSt6vectorIiSaIiEED1Ev,_ZNSt6vectorIiSaIiEED2Ev
    .text
    .p2align 4,,15
    .globl  _Z7processN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEES5_
    .type   _Z7processN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEES5_, @function
_Z7processN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEES5_:
.LFB456:
    .cfi_startproc
    pushq   %rbp    #
    .cfi_def_cfa_offset 16
    .cfi_offset 6, -16
    movq    %rsi, %rbp  # end, end
    pushq   %rbx    #
    .cfi_def_cfa_offset 24
    .cfi_offset 3, -24
    movq    %rdi, %rbx  # begin, ivtmp.41
    subq    $8, %rsp    #,
    .cfi_def_cfa_offset 32
    cmpq    %rsi, %rdi  # end, ivtmp.41
    je  .L4 #,
    .p2align 4,,10
    .p2align 3
.L8:
    movl    (%rbx), %edi    # MEM[base: _11, offset: 0],
    addq    $4, %rbx    #, ivtmp.41
    call    _Z9disappeari   #
    cmpq    %rbx, %rbp  # ivtmp.41, end
    jne .L8 #,
.L4:
    addq    $8, %rsp    #,
    .cfi_def_cfa_offset 24
    popq    %rbx    #
    .cfi_def_cfa_offset 16
    popq    %rbp    #
    .cfi_def_cfa_offset 8
    ret
    .cfi_endproc
.LFE456:
    .size   _Z7processN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEES5_, .-_Z7processN9__gnu_cxx17__normal_iteratorIPiSt6vectorIiSaIiEEEES5_
    .section    .text.startup,"ax",@progbits
    .p2align 4,,15
    .globl  main
    .type   main, @function
main:
.LFB457:
    .cfi_startproc
    pushq   %rbp    #
    .cfi_def_cfa_offset 16
    .cfi_offset 6, -16
    pushq   %rbx    #
    .cfi_def_cfa_offset 24
    .cfi_offset 3, -24
    subq    $8, %rsp    #,
    .cfi_def_cfa_offset 32
    movq    data+8(%rip), %rbp  # MEM[(int * const &)&data + 8], D.8856
    movq    data(%rip), %rbx    # MEM[(int * const &)&data], ivtmp.49
    cmpq    %rbx, %rbp  # ivtmp.49, D.8856
    je  .L14    #,
    .p2align 4,,10
    .p2align 3
.L15:
    movl    (%rbx), %edi    # MEM[base: _12, offset: 0],
    addq    $4, %rbx    #, ivtmp.49
    call    _Z9disappeari   #
    cmpq    %rbx, %rbp  # ivtmp.49, D.8856
    jne .L15    #,
.L14:
    addq    $8, %rsp    #,
    .cfi_def_cfa_offset 24
    xorl    %eax, %eax  #
    popq    %rbx    #
    .cfi_def_cfa_offset 16
    popq    %rbp    #
    .cfi_def_cfa_offset 8
    ret
    .cfi_endproc
.LFE457:
    .size   main, .-main
    .p2align 4,,15
    .type   _GLOBAL__sub_I_data, @function
_GLOBAL__sub_I_data:
.LFB534:
    .cfi_startproc
    subq    $8, %rsp    #,
    .cfi_def_cfa_offset 16
    movl    $data, %edi #,
    call    _Z8get_datav    #
    movl    $__dso_handle, %edx #,
    movl    $data, %esi #,
    movl    $_ZNSt6vectorIiSaIiEED1Ev, %edi #,
    addq    $8, %rsp    #,
    .cfi_def_cfa_offset 8
    jmp __cxa_atexit    #
    .cfi_endproc
.LFE534:
    .size   _GLOBAL__sub_I_data, .-_GLOBAL__sub_I_data
    .section    .init_array,"aw"
    .align 8
    .quad   _GLOBAL__sub_I_data
    .globl  data
    .bss
    .align 16
    .type   data, @object
    .size   data, 24
data:
    .zero   24
    .hidden __dso_handle
    .ident  "GCC: (GNU) 4.8.0"
    .section    .note.GNU-stack,"",@progbits
