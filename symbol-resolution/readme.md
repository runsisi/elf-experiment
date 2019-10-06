## build

```bash
$ gcc -c x.c main.c
$ gcc -c -fPIC so.c
$ gcc -o libso.so.1.0.0 -shared -Wl,-soname,libso.so.1 so.o
```

```bash
// Managing Linux Shared Libraries
// https://medium.com/@manojddesilva/managing-linux-shared-libraries-be67ae14af2a
$ ldconfig -n .
$ ln -sf libso.so.1 libso.so
```

```bash
$ gcc -o main main.o x.o -Wl,-rpath,. -L. -lso
```

## run

```bash
$ ./main
--- calls in main 1st:
common func from x (i.e., main)
hello from x,           0x5592c233b010: a
common func from x (i.e., main)             // <-- not from so
hello from so,          0x5592c233b018: 1

--- variables in main:
g_x_char,               0x5592c233b010: b
g_so_long,              0x5592c233b018: 2

--- calls in main 2nd:
common func from x (i.e., main)
hello from x,           0x5592c233b010: b
common func from x (i.e., main)             // <-- not from so
hello from so,          0x5592c233b018: 2
```

## disassemble

```bash
$ gdb main -batch -ex 'disassemble main'
Dump of assembler code for function main:
   0x000000000000080a <+0>:     push   %rbp
   0x000000000000080b <+1>:     mov    %rsp,%rbp
   0x000000000000080e <+4>:     lea    0x17f(%rip),%rdi        # 0x994
   0x0000000000000815 <+11>:    callq  0x6c0 <puts@plt>
   0x000000000000081a <+16>:    mov    $0x0,%eax
   0x000000000000081f <+21>:    callq  0x8cc <x_hello>
   0x0000000000000824 <+26>:    mov    $0x0,%eax
   0x0000000000000829 <+31>:    callq  0x6e0 <so_hello@plt>                                 // <-- call so function
   0x000000000000082e <+36>:    movb   $0x62,0x2007db(%rip)        # 0x201010 <g_x_char>
   0x0000000000000835 <+43>:    movq   $0x2,0x2007d8(%rip)        # 0x201018 <g_so_long>    // <-- update so variable
   0x0000000000000840 <+54>:    lea    0x164(%rip),%rdi        # 0x9ab
   0x0000000000000847 <+61>:    callq  0x6c0 <puts@plt>
   0x000000000000084c <+66>:    movzbl 0x2007bd(%rip),%eax        # 0x201010 <g_x_char>
   0x0000000000000853 <+73>:    movsbl %al,%eax
   0x0000000000000856 <+76>:    mov    %eax,%edx
   0x0000000000000858 <+78>:    lea    0x2007b1(%rip),%rsi        # 0x201010 <g_x_char>
   0x000000000000085f <+85>:    lea    0x15d(%rip),%rdi        # 0x9c3
   0x0000000000000866 <+92>:    mov    $0x0,%eax
   0x000000000000086b <+97>:    callq  0x6d0 <printf@plt>
   0x0000000000000870 <+102>:   mov    0x2007a1(%rip),%rax        # 0x201018 <g_so_long>
   0x0000000000000877 <+109>:   mov    %rax,%rdx
   0x000000000000087a <+112>:   lea    0x200797(%rip),%rsi        # 0x201018 <g_so_long>
   0x0000000000000881 <+119>:   lea    0x14f(%rip),%rdi        # 0x9d7
   0x0000000000000888 <+126>:   mov    $0x0,%eax
   0x000000000000088d <+131>:   callq  0x6d0 <printf@plt>
   0x0000000000000892 <+136>:   lea    0x154(%rip),%rdi        # 0x9ed
   0x0000000000000899 <+143>:   callq  0x6c0 <puts@plt>
   0x000000000000089e <+148>:   mov    $0x0,%eax
   0x00000000000008a3 <+153>:   callq  0x8cc <x_hello>
   0x00000000000008a8 <+158>:   mov    $0x0,%eax
   0x00000000000008ad <+163>:   callq  0x6e0 <so_hello@plt>
   0x00000000000008b2 <+168>:   mov    $0x0,%eax
   0x00000000000008b7 <+173>:   pop    %rbp
   0x00000000000008b8 <+174>:   retq   
End of assembler dump.
```

### .plt

```bash
$ objdump -d -j .plt main

main:     file format elf64-x86-64


Disassembly of section .plt:

00000000000006b0 <.plt>:
 6b0:   ff 35 fa 08 20 00       pushq  0x2008fa(%rip)        # 200fb0 <_GLOBAL_OFFSET_TABLE_+0x8>
 6b6:   ff 25 fc 08 20 00       jmpq   *0x2008fc(%rip)        # 200fb8 <_GLOBAL_OFFSET_TABLE_+0x10>
 6bc:   0f 1f 40 00             nopl   0x0(%rax)

00000000000006c0 <puts@plt>:
 6c0:   ff 25 fa 08 20 00       jmpq   *0x2008fa(%rip)        # 200fc0 <puts@GLIBC_2.2.5>
 6c6:   68 00 00 00 00          pushq  $0x0
 6cb:   e9 e0 ff ff ff          jmpq   6b0 <.plt>

00000000000006d0 <printf@plt>:
 6d0:   ff 25 f2 08 20 00       jmpq   *0x2008f2(%rip)        # 200fc8 <printf@GLIBC_2.2.5>
 6d6:   68 01 00 00 00          pushq  $0x1
 6db:   e9 d0 ff ff ff          jmpq   6b0 <.plt>

00000000000006e0 <so_hello@plt>:
 6e0:   ff 25 ea 08 20 00       jmpq   *0x2008ea(%rip)        # 200fd0 <so_hello>
 6e6:   68 02 00 00 00          pushq  $0x2
 6eb:   e9 c0 ff ff ff          jmpq   6b0 <.plt>
```

#### function so_hello

```bash
$ gdb main -batch -ex 'disassemble 0x6e0'
Dump of assembler code for function so_hello@plt:
   0x00000000000006e0 <+0>:     jmpq   *0x2008ea(%rip)        # 0x200fd0    // <-- jump to address stored in .got entry
   0x00000000000006e6 <+6>:     pushq  $0x2
   0x00000000000006eb <+11>:    jmpq   0x6b0
End of assembler dump.
```

```bash
$ gdb main -batch -ex 'x/xg 0x00000000000006e6 + 0x2008ea' 
0x200fd0:       0x00000000000006e6  // <-- jmpq   *0x2008ea(%rip) will jump to 0x6e6 then 0x6b0
```

```bash
$ gdb main -batch -ex 'disassemble 0x6b0'
No function contains specified address.
$ gdb main -batch -ex 'disassemble 0x6b0, +0x40'    // <-- [0x6b0, 0x6b0 + 0x10], the first entry in .plt
Dump of assembler code from 0x6b0 to 0x6f0:
   0x00000000000006b0:  pushq  0x2008fa(%rip)        # 0x200fb0
   0x00000000000006b6:  jmpq   *0x2008fc(%rip)        # 0x200fb8    // <-- dynamic symbol resolver
   0x00000000000006bc:  nopl   0x0(%rax)
   0x00000000000006c0 <puts@plt+0>:     jmpq   *0x2008fa(%rip)        # 0x200fc0
   0x00000000000006c6 <puts@plt+6>:     pushq  $0x0
   0x00000000000006cb <puts@plt+11>:    jmpq   0x6b0
   0x00000000000006d0 <printf@plt+0>:   jmpq   *0x2008f2(%rip)        # 0x200fc8
   0x00000000000006d6 <printf@plt+6>:   pushq  $0x1
   0x00000000000006db <printf@plt+11>:  jmpq   0x6b0
   0x00000000000006e0 <so_hello@plt+0>: jmpq   *0x2008ea(%rip)        # 0x200fd0
   0x00000000000006e6 <so_hello@plt+6>: pushq  $0x2
   0x00000000000006eb <so_hello@plt+11>:        jmpq   0x6b0
End of assembler dump.
```

### .got

```bash
$ objdump -d -z -j .got main

main:     file format elf64-x86-64


Disassembly of section .got:

0000000000200fa8 <_GLOBAL_OFFSET_TABLE_>:
  200fa8:       98 0d 20 00 00 00 00 00 00 00 00 00 00 00 00 00     .. .............
  200fb8:       00 00 00 00 00 00 00 00 c6 06 00 00 00 00 00 00     ................
  200fc8:       d6 06 00 00 00 00 00 00 e6 06 00 00 00 00 00 00     ................
  200fd8:       00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
  200fe8:       00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
  200ff8:       00 00 00 00 00 00 00 00                             ........
```

```bash
$ gdb main -batch -ex 'x/6xg 0x200fa8'
0x200fa8:       0x0000000000200d98      0x0000000000000000
0x200fb8:       0x0000000000000000      0x00000000000006c6
0x200fc8:       0x00000000000006d6      0x00000000000006e6
```

#### dynamic symbol resolver

```bash
$ gdb main -batch -ex 'p/x 0x00000000000006b6 + 0x2008fa'  
$1 = 0x200fb0
$ $ gdb main -batch -ex 'p/x 0x00000000000006bc + 0x2008fc' 
$1 = 0x200fb8
```

### Bsymbolic

```bash
$ gcc -o libso.so.1.0.0 -shared -Wl,-soname,libso.so.1 -Wl,-Bsymbolic so.o
$ gcc -o main main.o x.o -Wl,-rpath,. -L. -lso
$ ./main
--- calls in main 1st:
common func from x (i.e., main)
hello from x,           0x5651f5d1a010: a
common func from so                         // <-- from so
hello from so,          0x7fd5a004d030: 1

--- variables in main:
g_x_char,               0x5651f5d1a010: b
g_so_long,              0x5651f5d1a018: 2   // <-- the second copy of g_so_long

--- calls in main 2nd:
common func from x (i.e., main)
hello from x,           0x5651f5d1a010: b
common func from so                         // <-- from so
hello from so,          0x7fd5a004d030: 1
```

// ld(1) - Linux man page
//  https://linux.die.net/man/1/ld

if link option `-Bsymbolic` or `-Bsymbolic-functions` was specified, shared library resolves symbols from itself first.

#### w/ -Wl,-Bsymbolic

```bash
$ gcc -o libso.so.1.0.0 -shared -Wl,-soname,libso.so.1 -Wl,-Bsymbolic so.o
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'disassemble so_hello'
Dump of assembler code for function so_hello:
   0x00000000000006cd <+0>:     push   %rbp
   0x00000000000006ce <+1>:     mov    %rsp,%rbp
   0x00000000000006d1 <+4>:     mov    $0x0,%eax
   0x00000000000006d6 <+9>:     callq  0x6ba <common_func>                                  // <-- call directly
   0x00000000000006db <+14>:    lea    0x20094e(%rip),%rax        # 0x201030 <g_so_long>    // access directly
   0x00000000000006e2 <+21>:    mov    (%rax),%rax
   0x00000000000006e5 <+24>:    mov    %rax,%rdx
   0x00000000000006e8 <+27>:    lea    0x200941(%rip),%rax        # 0x201030 <g_so_long>
   0x00000000000006ef <+34>:    mov    %rax,%rsi
   0x00000000000006f2 <+37>:    lea    0x30(%rip),%rdi        # 0x729
   0x00000000000006f9 <+44>:    mov    $0x0,%eax
   0x00000000000006fe <+49>:    callq  0x5c0 <printf@plt>
   0x0000000000000703 <+54>:    mov    $0x73,%eax
   0x0000000000000708 <+59>:    pop    %rbp
   0x0000000000000709 <+60>:    retq   
End of assembler dump.
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'p/x 0x00000000000006e2 + 0x20094e'     // <-- address of g_so_long
$1 = 0x201030
$ objdump -d -z -j .data libso.so.1.0.0

libso.so.1.0.0:     file format elf64-x86-64


Disassembly of section .data:

0000000000201028 <__dso_handle>:
  201028:       28 10 20 00 00 00 00 00                             (. .....

0000000000201030 <g_so_long>:
  201030:       01 00 00 00 00 00 00 00                             ........    // <-- in .data
```

```bash
// from IDA
.text:00000000000006BA
.text:00000000000006BA ; =============== S U B R O U T I N E =======================================
.text:00000000000006BA
.text:00000000000006BA ; Attributes: bp-based frame
.text:00000000000006BA
.text:00000000000006BA                 public common_func
.text:00000000000006BA common_func     proc near               ; CODE XREF: so_hello+9↓p
.text:00000000000006BA                                         ; DATA XREF: LOAD:0000000000000388↑o
.text:00000000000006BA                 push    rbp
.text:00000000000006BB                 mov     rbp, rsp
.text:00000000000006BE                 lea     rdi, s          ; "common func from so"
.text:00000000000006C5                 call    _puts
.text:00000000000006CA                 nop
.text:00000000000006CB                 pop     rbp
.text:00000000000006CC                 retn
.text:00000000000006CC common_func     endp
.text:00000000000006CC

.plt:00000000000005A0 ; ===========================================================================
.plt:00000000000005A0
.plt:00000000000005A0 ; Segment type: Pure code
.plt:00000000000005A0 ; Segment permissions: Read/Execute
.plt:00000000000005A0 _plt            segment para public 'CODE' use64
.plt:00000000000005A0                 assume cs:_plt
.plt:00000000000005A0                 ;org 5A0h
.plt:00000000000005A0                 assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
.plt:00000000000005A0
.plt:00000000000005A0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005A0
.plt:00000000000005A0
.plt:00000000000005A0 sub_5A0         proc near               ; CODE XREF: .plt:00000000000005BB↓j
.plt:00000000000005A0                                         ; .plt:00000000000005CB↓j
.plt:00000000000005A0                 push    cs:qword_201008
.plt:00000000000005A6                 jmp     cs:qword_201010
.plt:00000000000005A6 sub_5A0         endp
.plt:00000000000005A6
.plt:00000000000005A6 ; ---------------------------------------------------------------------------
.plt:00000000000005AC                 align 10h
.plt:00000000000005B0
.plt:00000000000005B0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005B0
.plt:00000000000005B0 ; Attributes: thunk
.plt:00000000000005B0
.plt:00000000000005B0 ; int puts(const char *s)
.plt:00000000000005B0 _puts           proc near               ; CODE XREF: common_func+B↓p
.plt:00000000000005B0                 jmp     cs:off_201018
.plt:00000000000005B0 _puts           endp
.plt:00000000000005B0
.plt:00000000000005B6 ; ---------------------------------------------------------------------------
.plt:00000000000005B6                 push    0
.plt:00000000000005BB                 jmp     sub_5A0
.plt:00000000000005C0
.plt:00000000000005C0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005C0
.plt:00000000000005C0 ; Attributes: thunk
.plt:00000000000005C0
.plt:00000000000005C0 ; int printf(const char *format, ...)
.plt:00000000000005C0 _printf         proc near               ; CODE XREF: so_hello+31↓p
.plt:00000000000005C0                 jmp     cs:off_201020
.plt:00000000000005C0 _printf         endp
.plt:00000000000005C0
.plt:00000000000005C6 ; ---------------------------------------------------------------------------
.plt:00000000000005C6                 push    1
.plt:00000000000005CB                 jmp     sub_5A0
.plt:00000000000005CB _plt            ends
.plt:00000000000005CB
.plt.got:00000000000005D0 ; ===========================================================================
.plt.got:00000000000005D0
.plt.got:00000000000005D0 ; Segment type: Pure code
.plt.got:00000000000005D0 ; Segment permissions: Read/Execute
.plt.got:00000000000005D0 ; Segment alignment 'qword' can not be represented in assembly
.plt.got:00000000000005D0 _plt_got        segment para public 'CODE' use64
.plt.got:00000000000005D0                 assume cs:_plt_got
.plt.got:00000000000005D0                 ;org 5D0h
.plt.got:00000000000005D0                 assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
.plt.got:00000000000005D0 ; [00000006 BYTES: COLLAPSED FUNCTION __cxa_finalize. PRESS CTRL-NUMPAD+ TO EXPAND]
.plt.got:00000000000005D6                 align 8
.plt.got:00000000000005D6 _plt_got        ends
.plt.got:00000000000005D6

.got:0000000000200FE0 ; ===========================================================================
.got:0000000000200FE0
.got:0000000000200FE0 ; Segment type: Pure data
.got:0000000000200FE0 ; Segment permissions: Read/Write
.got:0000000000200FE0 ; Segment alignment 'qword' can not be represented in assembly
.got:0000000000200FE0 _got            segment para public 'DATA' use64
.got:0000000000200FE0                 assume cs:_got
.got:0000000000200FE0                 ;org 200FE0h
.got:0000000000200FE0 _ITM_deregisterTMCloneTable_ptr dq offset _ITM_deregisterTMCloneTable
.got:0000000000200FE0                                         ; DATA XREF: deregister_tm_clones+17↑r
.got:0000000000200FE8 __gmon_start___ptr dq offset __gmon_start__
.got:0000000000200FE8                                         ; DATA XREF: _init_proc+4↑r
.got:0000000000200FF0 _ITM_registerTMCloneTable_ptr dq offset _ITM_registerTMCloneTable
.got:0000000000200FF0                                         ; DATA XREF: register_tm_clones+28↑r
.got:0000000000200FF8 __cxa_finalize_ptr dq offset __imp___cxa_finalize
.got:0000000000200FF8                                         ; DATA XREF: __cxa_finalize↑r
.got:0000000000200FF8                                         ; __do_global_dtors_aux+9↑r
.got:0000000000200FF8 _got            ends
.got:0000000000200FF8
.got.plt:0000000000201000 ; ===========================================================================
.got.plt:0000000000201000
.got.plt:0000000000201000 ; Segment type: Pure data
.got.plt:0000000000201000 ; Segment permissions: Read/Write
.got.plt:0000000000201000 ; Segment alignment 'qword' can not be represented in assembly
.got.plt:0000000000201000 _got_plt        segment para public 'DATA' use64
.got.plt:0000000000201000                 assume cs:_got_plt
.got.plt:0000000000201000                 ;org 201000h
.got.plt:0000000000201000 _GLOBAL_OFFSET_TABLE_ dq offset _DYNAMIC
.got.plt:0000000000201008 qword_201008    dq 0                    ; DATA XREF: sub_5A0↑r
.got.plt:0000000000201010 qword_201010    dq 0                    ; DATA XREF: sub_5A0+6↑r
.got.plt:0000000000201018 off_201018      dq offset puts          ; DATA XREF: _puts↑r
.got.plt:0000000000201020 off_201020      dq offset printf        ; DATA XREF: _printf↑r
.got.plt:0000000000201020 _got_plt        ends
.got.plt:0000000000201020
.data:0000000000201028 ; ===========================================================================
.data:0000000000201028
.data:0000000000201028 ; Segment type: Pure data
.data:0000000000201028 ; Segment permissions: Read/Write
.data:0000000000201028 ; Segment alignment 'qword' can not be represented in assembly
.data:0000000000201028 _data           segment para public 'DATA' use64
.data:0000000000201028                 assume cs:_data
.data:0000000000201028                 ;org 201028h
.data:0000000000201028 __dso_handle    dq offset __dso_handle  ; DATA XREF: __do_global_dtors_aux+17↑r
.data:0000000000201028                                         ; .data:__dso_handle↓o
.data:0000000000201030                 public g_so_long
.data:0000000000201030 g_so_long       db    1                 ; DATA XREF: LOAD:0000000000000328↑o
.data:0000000000201030                                         ; so_hello+E↑o ...
.data:0000000000201031                 db    0
.data:0000000000201032                 db    0
.data:0000000000201033                 db    0
.data:0000000000201034                 db    0
.data:0000000000201035                 db    0
.data:0000000000201036                 db    0
.data:0000000000201037                 db    0
.data:0000000000201037 _data           ends
.data:0000000000201037
```

```bash
$ readelf -r libso.so.1.0.0

Relocation section '.rela.dyn' at offset 0x4a8 contains 7 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000200de0  000000000008 R_X86_64_RELATIVE                    6b0
000000200de8  000000000008 R_X86_64_RELATIVE                    670
000000201028  000000000008 R_X86_64_RELATIVE                    201028
000000200fe0  000100000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTMClone + 0
000000200fe8  000400000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
000000200ff0  000500000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCloneTa + 0
000000200ff8  000600000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0

Relocation section '.rela.plt' at offset 0x550 contains 2 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000201018  000200000007 R_X86_64_JUMP_SLO 0000000000000000 puts@GLIBC_2.2.5 + 0
000000201020  000300000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
```

```bash
$ nm -C -D libso.so.1.0.0
0000000000201038 B __bss_start
00000000000006ba T common_func
                 w __cxa_finalize
0000000000201038 D _edata
0000000000201040 B _end
000000000000070c T _fini
                 w __gmon_start__
0000000000201030 D g_so_long
0000000000000580 T _init
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
                 U printf
                 U puts
00000000000006cd T so_hello

$ readelf --dyn-syms libso.so.1.0.0

Symbol table '.dynsym' contains 15 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5 (2)
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
     4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
     5: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
     6: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
     7: 0000000000201038     0 NOTYPE  GLOBAL DEFAULT   22 _edata
     8: 0000000000201040     0 NOTYPE  GLOBAL DEFAULT   23 _end
     9: 00000000000006cd    61 FUNC    GLOBAL DEFAULT   12 so_hello
    10: 0000000000201030     8 OBJECT  GLOBAL DEFAULT   22 g_so_long
    11: 0000000000201038     0 NOTYPE  GLOBAL DEFAULT   23 __bss_start
    12: 0000000000000580     0 FUNC    GLOBAL DEFAULT    9 _init
    13: 000000000000070c     0 FUNC    GLOBAL DEFAULT   13 _fini
    14: 00000000000006ba    19 FUNC    GLOBAL DEFAULT   12 common_func
```

#### w/o -Wl,-Bsymbolic

```bash
$ gcc -o libso.so.1.0.0 -shared -Wl,-soname,libso.so.1 so.o
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'disassemble so_hello'
Dump of assembler code for function so_hello:
   0x000000000000070d <+0>:     push   %rbp
   0x000000000000070e <+1>:     mov    %rsp,%rbp
   0x0000000000000711 <+4>:     mov    $0x0,%eax
   0x0000000000000716 <+9>:     callq  0x600 <common_func@plt>                  // <-- call through plt
   0x000000000000071b <+14>:    mov    0x2008c6(%rip),%rax        # 0x200fe8    // <-- access through got
   0x0000000000000722 <+21>:    mov    (%rax),%rax
   0x0000000000000725 <+24>:    mov    %rax,%rdx
   0x0000000000000728 <+27>:    mov    0x2008b9(%rip),%rax        # 0x200fe8
   0x000000000000072f <+34>:    mov    %rax,%rsi
   0x0000000000000732 <+37>:    lea    0x30(%rip),%rdi        # 0x769
   0x0000000000000739 <+44>:    mov    $0x0,%eax
   0x000000000000073e <+49>:    callq  0x5f0 <printf@plt>
   0x0000000000000743 <+54>:    mov    $0x73,%eax
   0x0000000000000748 <+59>:    pop    %rbp
   0x0000000000000749 <+60>:    retq   
End of assembler dump.
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'p/x 0x0000000000000722 + 0x2008c6'     // <-- address of g_so_long
$1 = 0x200fe8
```

```bash
// from IDA
.text:00000000000006FA
.text:00000000000006FA ; =============== S U B R O U T I N E =======================================
.text:00000000000006FA
.text:00000000000006FA ; Attributes: bp-based frame
.text:00000000000006FA
.text:00000000000006FA                 public common_func
.text:00000000000006FA common_func     proc near               ; DATA XREF: LOAD:0000000000000388↑o
.text:00000000000006FA                                         ; .got.plt:off_201028↓o
.text:00000000000006FA                 push    rbp
.text:00000000000006FB                 mov     rbp, rsp
.text:00000000000006FE                 lea     rdi, s          ; "common func from so"
.text:0000000000000705                 call    _puts
.text:000000000000070A                 nop
.text:000000000000070B                 pop     rbp
.text:000000000000070C                 retn
.text:000000000000070C common_func     endp
.text:000000000000070C

.plt:00000000000005D0 ; ===========================================================================
.plt:00000000000005D0
.plt:00000000000005D0 ; Segment type: Pure code
.plt:00000000000005D0 ; Segment permissions: Read/Execute
.plt:00000000000005D0 _plt            segment para public 'CODE' use64
.plt:00000000000005D0                 assume cs:_plt
.plt:00000000000005D0                 ;org 5D0h
.plt:00000000000005D0                 assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
.plt:00000000000005D0
.plt:00000000000005D0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005D0
.plt:00000000000005D0
.plt:00000000000005D0 sub_5D0         proc near               ; CODE XREF: .plt:00000000000005EB↓j
.plt:00000000000005D0                                         ; .plt:00000000000005FB↓j ...
.plt:00000000000005D0                 push    cs:qword_201008
.plt:00000000000005D6                 jmp     cs:qword_201010
.plt:00000000000005D6 sub_5D0         endp
.plt:00000000000005D6
.plt:00000000000005D6 ; ---------------------------------------------------------------------------
.plt:00000000000005DC                 align 20h
.plt:00000000000005E0
.plt:00000000000005E0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005E0
.plt:00000000000005E0 ; Attributes: thunk
.plt:00000000000005E0
.plt:00000000000005E0 ; int puts(const char *s)
.plt:00000000000005E0 _puts           proc near               ; CODE XREF: common_func+B↓p
.plt:00000000000005E0                 jmp     cs:off_201018
.plt:00000000000005E0 _puts           endp
.plt:00000000000005E0
.plt:00000000000005E6 ; ---------------------------------------------------------------------------
.plt:00000000000005E6                 push    0
.plt:00000000000005EB                 jmp     sub_5D0
.plt:00000000000005F0
.plt:00000000000005F0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005F0
.plt:00000000000005F0 ; Attributes: thunk
.plt:00000000000005F0
.plt:00000000000005F0 ; int printf(const char *format, ...)
.plt:00000000000005F0 _printf         proc near               ; CODE XREF: so_hello+31↓p
.plt:00000000000005F0                 jmp     cs:off_201020
.plt:00000000000005F0 _printf         endp
.plt:00000000000005F0
.plt:00000000000005F6 ; ---------------------------------------------------------------------------
.plt:00000000000005F6                 push    1
.plt:00000000000005FB                 jmp     sub_5D0
.plt:0000000000000600
.plt:0000000000000600 ; =============== S U B R O U T I N E =======================================
.plt:0000000000000600
.plt:0000000000000600 ; Attributes: thunk
.plt:0000000000000600
.plt:0000000000000600 _common_func    proc near               ; CODE XREF: so_hello+9↓p
.plt:0000000000000600                 jmp     cs:off_201028
.plt:0000000000000600 _common_func    endp
.plt:0000000000000600
.plt:0000000000000606 ; ---------------------------------------------------------------------------
.plt:0000000000000606                 push    2
.plt:000000000000060B                 jmp     sub_5D0
.plt:000000000000060B _plt            ends
.plt:000000000000060B
.plt.got:0000000000000610 ; ===========================================================================
.plt.got:0000000000000610
.plt.got:0000000000000610 ; Segment type: Pure code
.plt.got:0000000000000610 ; Segment permissions: Read/Execute
.plt.got:0000000000000610 ; Segment alignment 'qword' can not be represented in assembly
.plt.got:0000000000000610 _plt_got        segment para public 'CODE' use64
.plt.got:0000000000000610                 assume cs:_plt_got
.plt.got:0000000000000610                 ;org 610h
.plt.got:0000000000000610                 assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
.plt.got:0000000000000610 ; [00000006 BYTES: COLLAPSED FUNCTION __cxa_finalize. PRESS CTRL-NUMPAD+ TO EXPAND]
.plt.got:0000000000000616                 align 8
.plt.got:0000000000000616 _plt_got        ends
.plt.got:0000000000000616

.got:0000000000200FD8 ; ===========================================================================
.got:0000000000200FD8
.got:0000000000200FD8 ; Segment type: Pure data
.got:0000000000200FD8 ; Segment permissions: Read/Write
.got:0000000000200FD8 ; Segment alignment 'qword' can not be represented in assembly
.got:0000000000200FD8 _got            segment para public 'DATA' use64
.got:0000000000200FD8                 assume cs:_got
.got:0000000000200FD8                 ;org 200FD8h
.got:0000000000200FD8 _ITM_deregisterTMCloneTable_ptr dq offset _ITM_deregisterTMCloneTable
.got:0000000000200FD8                                         ; DATA XREF: deregister_tm_clones+17↑r
.got:0000000000200FE0 __gmon_start___ptr dq offset __gmon_start__
.got:0000000000200FE0                                         ; DATA XREF: _init_proc+4↑r
.got:0000000000200FE8 g_so_long_ptr   dq offset g_so_long     ; DATA XREF: so_hello+E↑r     // <-- here
.got:0000000000200FE8                                         ; so_hello+1B↑r
.got:0000000000200FF0 _ITM_registerTMCloneTable_ptr dq offset _ITM_registerTMCloneTable
.got:0000000000200FF0                                         ; DATA XREF: register_tm_clones+28↑r
.got:0000000000200FF8 __cxa_finalize_ptr dq offset __imp___cxa_finalize
.got:0000000000200FF8                                         ; DATA XREF: __cxa_finalize↑r
.got:0000000000200FF8                                         ; __do_global_dtors_aux+9↑r
.got:0000000000200FF8 _got            ends
.got:0000000000200FF8
.got.plt:0000000000201000 ; ===========================================================================
.got.plt:0000000000201000
.got.plt:0000000000201000 ; Segment type: Pure data
.got.plt:0000000000201000 ; Segment permissions: Read/Write
.got.plt:0000000000201000 ; Segment alignment 'qword' can not be represented in assembly
.got.plt:0000000000201000 _got_plt        segment para public 'DATA' use64
.got.plt:0000000000201000                 assume cs:_got_plt
.got.plt:0000000000201000                 ;org 201000h
.got.plt:0000000000201000 _GLOBAL_OFFSET_TABLE_ dq offset _DYNAMIC
.got.plt:0000000000201008 qword_201008    dq 0                    ; DATA XREF: sub_5D0↑r
.got.plt:0000000000201010 qword_201010    dq 0                    ; DATA XREF: sub_5D0+6↑r
.got.plt:0000000000201018 off_201018      dq offset puts          ; DATA XREF: _puts↑r
.got.plt:0000000000201020 off_201020      dq offset printf        ; DATA XREF: _printf↑r
.got.plt:0000000000201028 off_201028      dq offset common_func   ; DATA XREF: _common_func↑r
.got.plt:0000000000201028 _got_plt        ends
.got.plt:0000000000201028
.data:0000000000201030 ; ===========================================================================
.data:0000000000201030
.data:0000000000201030 ; Segment type: Pure data
.data:0000000000201030 ; Segment permissions: Read/Write
.data:0000000000201030 ; Segment alignment 'qword' can not be represented in assembly
.data:0000000000201030 _data           segment para public 'DATA' use64
.data:0000000000201030                 assume cs:_data
.data:0000000000201030                 ;org 201030h
.data:0000000000201030 __dso_handle    dq offset __dso_handle  ; DATA XREF: __do_global_dtors_aux+17↑r
.data:0000000000201030                                         ; .data:__dso_handle↓o
.data:0000000000201038                 public g_so_long
.data:0000000000201038 g_so_long       db    1                 ; DATA XREF: LOAD:0000000000000328↑o
.data:0000000000201038                                         ; .got:g_so_long_ptr↑o
.data:0000000000201039                 db    0
.data:000000000020103A                 db    0
.data:000000000020103B                 db    0
.data:000000000020103C                 db    0
.data:000000000020103D                 db    0
.data:000000000020103E                 db    0
.data:000000000020103F                 db    0
.data:000000000020103F _data           ends
.data:000000000020103F
```

```bash
$ readelf -r libso.so.1.0.0

Relocation section '.rela.dyn' at offset 0x4a8 contains 8 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000200df8  000000000008 R_X86_64_RELATIVE                    6f0
000000200e00  000000000008 R_X86_64_RELATIVE                    6b0
000000201030  000000000008 R_X86_64_RELATIVE                    201030
000000200fd8  000100000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTMClone + 0
000000200fe0  000400000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
000000200fe8  000a00000006 R_X86_64_GLOB_DAT 0000000000201038 g_so_long + 0     // <-- need to resolve
000000200ff0  000500000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCloneTa + 0
000000200ff8  000600000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0

Relocation section '.rela.plt' at offset 0x568 contains 3 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000201018  000200000007 R_X86_64_JUMP_SLO 0000000000000000 puts@GLIBC_2.2.5 + 0
000000201020  000300000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
000000201028  000e00000007 R_X86_64_JUMP_SLO 00000000000006fa common_func + 0   // <-- need to resolve
```

```bash
$ nm -C -D libso.so.1.0.0
0000000000201040 B __bss_start
00000000000006fa T common_func
                 w __cxa_finalize
0000000000201040 D _edata
0000000000201048 B _end
000000000000074c T _fini
                 w __gmon_start__
0000000000201038 D g_so_long
00000000000005b0 T _init
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
                 U printf
                 U puts
000000000000070d T so_hello

$ readelf --dyn-syms libso.so.1.0.0

Symbol table '.dynsym' contains 15 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5 (2)
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
     4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
     5: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
     6: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
     7: 0000000000201040     0 NOTYPE  GLOBAL DEFAULT   22 _edata
     8: 0000000000201048     0 NOTYPE  GLOBAL DEFAULT   23 _end
     9: 000000000000070d    61 FUNC    GLOBAL DEFAULT   12 so_hello
    10: 0000000000201038     8 OBJECT  GLOBAL DEFAULT   22 g_so_long
    11: 0000000000201040     0 NOTYPE  GLOBAL DEFAULT   23 __bss_start
    12: 00000000000005b0     0 FUNC    GLOBAL DEFAULT    9 _init
    13: 000000000000074c     0 FUNC    GLOBAL DEFAULT   13 _fini
    14: 00000000000006fa    19 FUNC    GLOBAL DEFAULT   12 common_func
```
