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
-- main@0x5630f16377ca
- x_hello@0x5630f16378d8
common_func@0x5630f16378b2 from x.o
g_x_char@0x5630f1838010 = a
- so_hello@0x7f5ff08d46e3
common_func@0x5630f16378b2 from x.o     // <-- not from so
g_so_long@0x5630f1838018 = 1

-- main@0x5630f16377ca
g_x_char@0x5630f1838010 = b
g_so_long0x5630f1838018 = 2

-- main@0x5630f16377ca
- x_hello@0x5630f16378d8
common_func@0x5630f16378b2 from x.o
g_x_char@0x5630f1838010 = b
- so_hello@0x7f5ff08d46e3
common_func@0x5630f16378b2 from x.o     // <-- not from so
g_so_long@0x5630f1838018 = 2
```

## disassemble

```bash
$ gdb main -batch -ex 'disassemble main'
Dump of assembler code for function main:
   0x00000000000007ca <+0>:     push   %rbp
   0x00000000000007cb <+1>:     mov    %rsp,%rbp
   0x00000000000007ce <+4>:     lea    -0xb(%rip),%rdx        # 0x7ca <main>
   0x00000000000007d5 <+11>:    lea    0x212(%rip),%rsi        # 0x9ee <__func__.2253>
   0x00000000000007dc <+18>:    lea    0x1d1(%rip),%rdi        # 0x9b4
   0x00000000000007e3 <+25>:    mov    $0x0,%eax
   0x00000000000007e8 <+30>:    callq  0x690 <printf@plt>
   0x00000000000007ed <+35>:    mov    $0x0,%eax
   0x00000000000007f2 <+40>:    callq  0x8d8 <x_hello>
   0x00000000000007f7 <+45>:    mov    $0x0,%eax
   0x00000000000007fc <+50>:    callq  0x6a0 <so_hello@plt>                                 // <-- call so function
   0x0000000000000801 <+55>:    lea    -0x3e(%rip),%rdx        # 0x7ca <main>
   0x0000000000000808 <+62>:    lea    0x1df(%rip),%rsi        # 0x9ee <__func__.2253>
   0x000000000000080f <+69>:    lea    0x1a8(%rip),%rdi        # 0x9be
   0x0000000000000816 <+76>:    mov    $0x0,%eax
   0x000000000000081b <+81>:    callq  0x690 <printf@plt>
   0x0000000000000820 <+86>:    movb   $0x62,0x2007e9(%rip)        # 0x201010 <g_x_char>
   0x0000000000000827 <+93>:    movq   $0x2,0x2007e6(%rip)        # 0x201018 <g_so_long>    // <-- update so variable
   0x0000000000000832 <+104>:   movzbl 0x2007d7(%rip),%eax        # 0x201010 <g_x_char>
   0x0000000000000839 <+111>:   movsbl %al,%eax
   0x000000000000083c <+114>:   mov    %eax,%edx
   0x000000000000083e <+116>:   lea    0x2007cb(%rip),%rsi        # 0x201010 <g_x_char>
   0x0000000000000845 <+123>:   lea    0x17d(%rip),%rdi        # 0x9c9
   0x000000000000084c <+130>:   mov    $0x0,%eax
   0x0000000000000851 <+135>:   callq  0x690 <printf@plt>
   0x0000000000000856 <+140>:   mov    0x2007bb(%rip),%rax        # 0x201018 <g_so_long>
   0x000000000000085d <+147>:   mov    %rax,%rdx
   0x0000000000000860 <+150>:   lea    0x2007b1(%rip),%rsi        # 0x201018 <g_so_long>
   0x0000000000000867 <+157>:   lea    0x16d(%rip),%rdi        # 0x9db
   0x000000000000086e <+164>:   mov    $0x0,%eax
   0x0000000000000873 <+169>:   callq  0x690 <printf@plt>
   0x0000000000000878 <+174>:   lea    -0xb5(%rip),%rdx        # 0x7ca <main>
   0x000000000000087f <+181>:   lea    0x168(%rip),%rsi        # 0x9ee <__func__.2253>
   0x0000000000000886 <+188>:   lea    0x131(%rip),%rdi        # 0x9be
   0x000000000000088d <+195>:   mov    $0x0,%eax
   0x0000000000000892 <+200>:   callq  0x690 <printf@plt>
   0x0000000000000897 <+205>:   mov    $0x0,%eax
   0x000000000000089c <+210>:   callq  0x8d8 <x_hello>
   0x00000000000008a1 <+215>:   mov    $0x0,%eax
   0x00000000000008a6 <+220>:   callq  0x6a0 <so_hello@plt>
   0x00000000000008ab <+225>:   mov    $0x0,%eax
   0x00000000000008b0 <+230>:   pop    %rbp
   0x00000000000008b1 <+231>:   retq   
End of assembler dump.
```

### .plt

```bash
$ objdump -d -j .plt main

main:     file format elf64-x86-64


Disassembly of section .plt:

0000000000000680 <.plt>:
 680:   ff 35 32 09 20 00       pushq  0x200932(%rip)        # 200fb8 <_GLOBAL_OFFSET_TABLE_+0x8>
 686:   ff 25 34 09 20 00       jmpq   *0x200934(%rip)        # 200fc0 <_GLOBAL_OFFSET_TABLE_+0x10>
 68c:   0f 1f 40 00             nopl   0x0(%rax)

0000000000000690 <printf@plt>:
 690:   ff 25 32 09 20 00       jmpq   *0x200932(%rip)        # 200fc8 <printf@GLIBC_2.2.5>
 696:   68 00 00 00 00          pushq  $0x0
 69b:   e9 e0 ff ff ff          jmpq   680 <.plt>

00000000000006a0 <so_hello@plt>:
 6a0:   ff 25 2a 09 20 00       jmpq   *0x20092a(%rip)        # 200fd0 <so_hello>
 6a6:   68 01 00 00 00          pushq  $0x1
 6ab:   e9 d0 ff ff ff          jmpq   680 <.plt>
```

#### function so_hello@plt

```bash
$ gdb main -batch -ex 'disassemble 0x6a0'
Dump of assembler code for function so_hello@plt:
   0x00000000000006a0 <+0>:     jmpq   *0x20092a(%rip)        # 0x200fd0    // <-- jump to address stored in .got entry
   0x00000000000006a6 <+6>:     pushq  $0x1
   0x00000000000006ab <+11>:    jmpq   0x680
End of assembler dump.
```

```bash
$ gdb main -batch -ex 'x/xg 0x00000000000006a6 + 0x20092a'      // <-- jmpq   *0x20092a(%rip)
0x200fd0:       0x00000000000006a6  // <-- jmpq   *0x2008ea(%rip) jumps to 0x6a6, i.e., the next instruction
```

```bash
$ gdb main -batch -ex 'disassemble 0x680'                       // <-- jmpq   0x680
No function contains specified address.
$ gdb main -batch -ex 'disassemble 0x680, +0x30'    // <-- [0x680, 0x680 + 0x10], the first entry in .plt
Dump of assembler code from 0x680 to 0x6b0:
   0x0000000000000680:  pushq  0x200932(%rip)        # 0x200fb8
   0x0000000000000686:  jmpq   *0x200934(%rip)        # 0x200fc0    // <-- dynamic symbol resolver
   0x000000000000068c:  nopl   0x0(%rax)
   0x0000000000000690 <printf@plt+0>:   jmpq   *0x200932(%rip)        # 0x200fc8
   0x0000000000000696 <printf@plt+6>:   pushq  $0x0
   0x000000000000069b <printf@plt+11>:  jmpq   0x680
   0x00000000000006a0 <so_hello@plt+0>: jmpq   *0x20092a(%rip)        # 0x200fd0
   0x00000000000006a6 <so_hello@plt+6>: pushq  $0x1
   0x00000000000006ab <so_hello@plt+11>:        jmpq   0x680
End of assembler dump.
```

### .got

```bash
$ objdump -d -z -j .got main

main:     file format elf64-x86-64


Disassembly of section .got:

0000000000200fb0 <_GLOBAL_OFFSET_TABLE_>:
  200fb0:       a0 0d 20 00 00 00 00 00 00 00 00 00 00 00 00 00     .. .............
  200fc0:       00 00 00 00 00 00 00 00 96 06 00 00 00 00 00 00     ................
  200fd0:       a6 06 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
  200fe0:       00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
  200ff0:       00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00     ................
```

```bash
$ gdb main -batch -ex 'x/6xg 0x200fb0'
0x200fb0:       0x0000000000200da0      0x0000000000000000
0x200fc0:       0x0000000000000000      0x0000000000000696
0x200fd0:       0x00000000000006a6      0x0000000000000000
```

#### dynamic symbol resolver

```bash
$ gdb main -batch -ex 'p/x 0x0000000000000686 + 0x200932'       // <-- pushq  0x200932(%rip)
$1 = 0x200fb8
$ gdb main -batch -ex 'p/x 0x000000000000068c + 0x200934'       // <-- jmpq   *0x200934(%rip)
$1 = 0x200fc0
```

### Bsymbolic

```bash
$ gcc -o libso.so.1.0.0 -shared -Wl,-soname,libso.so.1 -Wl,-Bsymbolic so.o
$ gcc -o main main.o x.o -Wl,-rpath,. -L. -lso
$ ./main
-- main@0x55ef01f917ca
- x_hello@0x55ef01f918d8
common_func@0x55ef01f918b2 from x.o
g_x_char@0x55ef02192010 = a
- so_hello@0x7f9a91a0f693
common_func@0x7f9a91a0f66a from .so         // <-- from so
g_so_long@0x7f9a91c10028 = 1

-- main@0x55ef01f917ca
g_x_char@0x55ef02192010 = b
g_so_long0x55ef02192018 = 2                 // <-- the second copy of g_so_long

-- main@0x55ef01f917ca
- x_hello@0x55ef01f918d8
common_func@0x55ef01f918b2 from x.o
g_x_char@0x55ef02192010 = b
- so_hello@0x7f9a91a0f693
common_func@0x7f9a91a0f66a from .so         // <-- from so
g_so_long@0x7f9a91c10028 = 1
```

// ld(1) - Linux man page
//  https://linux.die.net/man/1/ld

if link option `-Bsymbolic` or `-Bsymbolic-functions` was specified, shared library resolves symbols from itself first.

#### w/ -Wl,-Bsymbolic

```bash
$ gcc -o libso.so.1.0.0 -shared -Wl,-soname,libso.so.1 -Wl,-Bsymbolic so.o
$ ./main
-- main@0x55b5175af7ca
- x_hello@0x55b5175af8d8
common_func@0x55b5175af8b2 from x.o
g_x_char@0x55b5177b0010 = a
- so_hello@0x7f78745cc693
common_func@0x7f78745cc66a from .so
g_so_long@0x7f78747cd028 = 1

-- main@0x55b5175af7ca
g_x_char@0x55b5177b0010 = b
g_so_long0x55b5177b0018 = 2

-- main@0x55b5175af7ca
- x_hello@0x55b5175af8d8
common_func@0x55b5175af8b2 from x.o
g_x_char@0x55b5177b0010 = b
- so_hello@0x7f78745cc693
common_func@0x7f78745cc66a from .so
g_so_long@0x7f78747cd028 = 1
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'disassemble so_hello'
Dump of assembler code for function so_hello:
   0x0000000000000693 <+0>:     push   %rbp
   0x0000000000000694 <+1>:     mov    %rsp,%rbp
   0x0000000000000697 <+4>:     lea    -0xb(%rip),%rax        # 0x693 <so_hello>
   0x000000000000069e <+11>:    mov    %rax,%rdx
   0x00000000000006a1 <+14>:    lea    0x98(%rip),%rsi        # 0x740 <__func__.2253>
   0x00000000000006a8 <+21>:    lea    0x61(%rip),%rdi        # 0x710
   0x00000000000006af <+28>:    mov    $0x0,%eax
   0x00000000000006b4 <+33>:    callq  0x570 <printf@plt>
   0x00000000000006b9 <+38>:    mov    $0x0,%eax
   0x00000000000006be <+43>:    callq  0x66a <common_func>                                  // <-- call directly
   0x00000000000006c3 <+48>:    lea    0x20095e(%rip),%rax        # 0x201028 <g_so_long>    // <-- access directly
   0x00000000000006ca <+55>:    mov    (%rax),%rax
   0x00000000000006cd <+58>:    mov    %rax,%rdx
   0x00000000000006d0 <+61>:    lea    0x200951(%rip),%rax        # 0x201028 <g_so_long>
   0x00000000000006d7 <+68>:    mov    %rax,%rsi
   0x00000000000006da <+71>:    lea    0x38(%rip),%rdi        # 0x719
   0x00000000000006e1 <+78>:    mov    $0x0,%eax
   0x00000000000006e6 <+83>:    callq  0x570 <printf@plt>
   0x00000000000006eb <+88>:    mov    $0x73,%eax
   0x00000000000006f0 <+93>:    pop    %rbp
   0x00000000000006f1 <+94>:    retq   
End of assembler dump.
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'p/x 0x00000000000006ca + 0x20095e'     // <-- address of g_so_long
$1 = 0x201028
$ objdump -d -z -j .data libso.so.1.0.0

libso.so.1.0.0:     file format elf64-x86-64


Disassembly of section .data:

0000000000201020 <__dso_handle>:
  201020:       20 10 20 00 00 00 00 00                              . .....

0000000000201028 <g_so_long>:
  201028:       01 00 00 00 00 00 00 00                             ........    // <-- g_so_long in .data
```

```bash
// libso.so in IDA
.text:000000000000066A
.text:000000000000066A ; =============== S U B R O U T I N E =======================================
.text:000000000000066A
.text:000000000000066A ; Attributes: bp-based frame
.text:000000000000066A
.text:000000000000066A                 public common_func
.text:000000000000066A common_func     proc near               ; CODE XREF: so_hello+2B↓p
.text:000000000000066A                                         ; DATA XREF: LOAD:0000000000000370↑o ...
.text:000000000000066A                 push    rbp
.text:000000000000066B                 mov     rbp, rsp
.text:000000000000066E                 lea     rax, common_func
.text:0000000000000675                 mov     rdx, rax
.text:0000000000000678                 lea     rsi, __func___2250 ; "common_func"
.text:000000000000067F                 lea     rdi, format     ; "%s@%p from .so\n"
.text:0000000000000686                 mov     eax, 0
.text:000000000000068B                 call    _printf
.text:0000000000000690                 nop
.text:0000000000000691                 pop     rbp
.text:0000000000000692                 retn
.text:0000000000000692 common_func     endp
.text:0000000000000692

.plt:0000000000000560 ; ===========================================================================
.plt:0000000000000560
.plt:0000000000000560 ; Segment type: Pure code
.plt:0000000000000560 ; Segment permissions: Read/Execute
.plt:0000000000000560 _plt            segment para public 'CODE' use64
.plt:0000000000000560                 assume cs:_plt
.plt:0000000000000560                 ;org 560h
.plt:0000000000000560                 assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
.plt:0000000000000560
.plt:0000000000000560 ; =============== S U B R O U T I N E =======================================
.plt:0000000000000560
.plt:0000000000000560
.plt:0000000000000560 sub_560         proc near               ; CODE XREF: .plt:000000000000057B↓j
.plt:0000000000000560                 push    cs:qword_201008
.plt:0000000000000566                 jmp     cs:qword_201010
.plt:0000000000000566 sub_560         endp
.plt:0000000000000566
.plt:0000000000000566 ; ---------------------------------------------------------------------------
.plt:000000000000056C                 align 10h
.plt:0000000000000570
.plt:0000000000000570 ; =============== S U B R O U T I N E =======================================
.plt:0000000000000570
.plt:0000000000000570 ; Attributes: thunk
.plt:0000000000000570
.plt:0000000000000570 ; int printf(const char *format, ...)
.plt:0000000000000570 _printf         proc near               ; CODE XREF: common_func+21↓p
.plt:0000000000000570                                         ; so_hello+21↓p ...
.plt:0000000000000570                 jmp     cs:off_201018
.plt:0000000000000570 _printf         endp
.plt:0000000000000570
.plt:0000000000000576 ; ---------------------------------------------------------------------------
.plt:0000000000000576                 push    0
.plt:000000000000057B                 jmp     sub_560
.plt:000000000000057B _plt            ends
.plt:000000000000057B
.plt.got:0000000000000580 ; ===========================================================================
.plt.got:0000000000000580
.plt.got:0000000000000580 ; Segment type: Pure code
.plt.got:0000000000000580 ; Segment permissions: Read/Execute
.plt.got:0000000000000580 ; Segment alignment 'qword' can not be represented in assembly
.plt.got:0000000000000580 _plt_got        segment para public 'CODE' use64
.plt.got:0000000000000580                 assume cs:_plt_got
.plt.got:0000000000000580                 ;org 580h
.plt.got:0000000000000580                 assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
.plt.got:0000000000000580
.plt.got:0000000000000580 ; =============== S U B R O U T I N E =======================================
.plt.got:0000000000000580
.plt.got:0000000000000580 ; Attributes: thunk
.plt.got:0000000000000580
.plt.got:0000000000000580 __cxa_finalize  proc near               ; CODE XREF: __do_global_dtors_aux+1E↓p
.plt.got:0000000000000580                 jmp     cs:__cxa_finalize_ptr
.plt.got:0000000000000580 __cxa_finalize  endp
.plt.got:0000000000000580
.plt.got:0000000000000580 ; ---------------------------------------------------------------------------
.plt.got:0000000000000586                 align 8
.plt.got:0000000000000586 _plt_got        ends
.plt.got:0000000000000586

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
.got.plt:0000000000201008 qword_201008    dq 0                    ; DATA XREF: sub_560↑r
.got.plt:0000000000201010 qword_201010    dq 0                    ; DATA XREF: sub_560+6↑r
.got.plt:0000000000201018 off_201018      dq offset printf        ; DATA XREF: _printf↑r
.got.plt:0000000000201018 _got_plt        ends
.got.plt:0000000000201018
.data:0000000000201020 ; ===========================================================================
.data:0000000000201020
.data:0000000000201020 ; Segment type: Pure data
.data:0000000000201020 ; Segment permissions: Read/Write
.data:0000000000201020 ; Segment alignment 'qword' can not be represented in assembly
.data:0000000000201020 _data           segment para public 'DATA' use64
.data:0000000000201020                 assume cs:_data
.data:0000000000201020                 ;org 201020h
.data:0000000000201020 __dso_handle    dq offset __dso_handle  ; DATA XREF: __do_global_dtors_aux+17↑r
.data:0000000000201020                                         ; .data:__dso_handle↓o
.data:0000000000201028                 public g_so_long
.data:0000000000201028 g_so_long       db    1                 ; DATA XREF: LOAD:0000000000000310↑o
.data:0000000000201028                                         ; so_hello+30↑o ...
.data:0000000000201029                 db    0
.data:000000000020102A                 db    0
.data:000000000020102B                 db    0
.data:000000000020102C                 db    0
.data:000000000020102D                 db    0
.data:000000000020102E                 db    0
.data:000000000020102F                 db    0
.data:000000000020102F _data           ends
.data:000000000020102F
```

```bash
$ readelf -r libso.so.1.0.0

Relocation section '.rela.dyn' at offset 0x488 contains 7 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000200de0  000000000008 R_X86_64_RELATIVE                    660
000000200de8  000000000008 R_X86_64_RELATIVE                    620
000000201020  000000000008 R_X86_64_RELATIVE                    201020
000000200fe0  000100000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTMClone + 0
000000200fe8  000300000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
000000200ff0  000400000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCloneTa + 0
000000200ff8  000500000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0

Relocation section '.rela.plt' at offset 0x530 contains 1 entry:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000201018  000200000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
```

```bash
$ nm -C -D libso.so.1.0.0
0000000000201030 B __bss_start
000000000000066a T common_func
                 w __cxa_finalize
0000000000201030 D _edata
0000000000201038 B _end
00000000000006f4 T _fini
                 w __gmon_start__
0000000000201028 D g_so_long
0000000000000548 T _init
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
                 U printf
0000000000000693 T so_hello

$ readelf --dyn-syms libso.so.1.0.0

Symbol table '.dynsym' contains 14 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
     3: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
     4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
     5: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
     6: 0000000000201030     0 NOTYPE  GLOBAL DEFAULT   22 _edata
     7: 0000000000201038     0 NOTYPE  GLOBAL DEFAULT   23 _end
     8: 0000000000000693    95 FUNC    GLOBAL DEFAULT   12 so_hello
     9: 0000000000201028     8 OBJECT  GLOBAL DEFAULT   22 g_so_long
    10: 0000000000201030     0 NOTYPE  GLOBAL DEFAULT   23 __bss_start
    11: 0000000000000548     0 FUNC    GLOBAL DEFAULT    9 _init
    12: 00000000000006f4     0 FUNC    GLOBAL DEFAULT   13 _fini
    13: 000000000000066a    41 FUNC    GLOBAL DEFAULT   12 common_func
```

#### w/o -Wl,-Bsymbolic

```bash
$ gcc -o libso.so.1.0.0 -shared -Wl,-soname,libso.so.1 so.o
$ ./main
-- main@0x55886e8287ca
- x_hello@0x55886e8288d8
common_func@0x55886e8288b2 from x.o
g_x_char@0x55886ea29010 = a
- so_hello@0x7f3722e316e3
common_func@0x55886e8288b2 from x.o
g_so_long@0x55886ea29018 = 1

-- main@0x55886e8287ca
g_x_char@0x55886ea29010 = b
g_so_long0x55886ea29018 = 2

-- main@0x55886e8287ca
- x_hello@0x55886e8288d8
common_func@0x55886e8288b2 from x.o
g_x_char@0x55886ea29010 = b
- so_hello@0x7f3722e316e3
common_func@0x55886e8288b2 from x.o
g_so_long@0x55886ea29018 = 2
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'disassemble so_hello'
Dump of assembler code for function so_hello:
   0x00000000000006e3 <+0>:     push   %rbp
   0x00000000000006e4 <+1>:     mov    %rsp,%rbp
   0x00000000000006e7 <+4>:     mov    0x2008ea(%rip),%rax        # 0x200fd8
   0x00000000000006ee <+11>:    mov    %rax,%rdx
   0x00000000000006f1 <+14>:    lea    0x98(%rip),%rsi        # 0x790 <__func__.2253>
   0x00000000000006f8 <+21>:    lea    0x61(%rip),%rdi        # 0x760
   0x00000000000006ff <+28>:    mov    $0x0,%eax
   0x0000000000000704 <+33>:    callq  0x5c0 <printf@plt>
   0x0000000000000709 <+38>:    mov    $0x0,%eax
   0x000000000000070e <+43>:    callq  0x5d0 <common_func@plt>                  // <-- call through plt
   0x0000000000000713 <+48>:    mov    0x2008ce(%rip),%rax        # 0x200fe8    // <-- access through got
   0x000000000000071a <+55>:    mov    (%rax),%rax
   0x000000000000071d <+58>:    mov    %rax,%rdx
   0x0000000000000720 <+61>:    mov    0x2008c1(%rip),%rax        # 0x200fe8
   0x0000000000000727 <+68>:    mov    %rax,%rsi
   0x000000000000072a <+71>:    lea    0x38(%rip),%rdi        # 0x769
   0x0000000000000731 <+78>:    mov    $0x0,%eax
   0x0000000000000736 <+83>:    callq  0x5c0 <printf@plt>
   0x000000000000073b <+88>:    mov    $0x73,%eax
   0x0000000000000740 <+93>:    pop    %rbp
   0x0000000000000741 <+94>:    retq   
End of assembler dump.
```

```bash
$ gdb libso.so.1.0.0 -batch -ex 'p/x 0x000000000000071a + 0x2008ce'     // <-- address of g_so_long
$1 = 0x200fe8
```

```bash
// libso.so from IDA
.text:00000000000006BA
.text:00000000000006BA ; =============== S U B R O U T I N E =======================================
.text:00000000000006BA
.text:00000000000006BA ; Attributes: bp-based frame
.text:00000000000006BA
.text:00000000000006BA                 public common_func
.text:00000000000006BA common_func     proc near               ; DATA XREF: LOAD:0000000000000370↑o
.text:00000000000006BA                                         ; .got:common_func_ptr↓o
.text:00000000000006BA                 push    rbp
.text:00000000000006BB                 mov     rbp, rsp
.text:00000000000006BE                 mov     rax, cs:common_func_ptr
.text:00000000000006C5                 mov     rdx, rax
.text:00000000000006C8                 lea     rsi, __func___2250 ; "common_func"
.text:00000000000006CF                 lea     rdi, format     ; "%s@%p from .so\n"
.text:00000000000006D6                 mov     eax, 0
.text:00000000000006DB                 call    _printf
.text:00000000000006E0                 nop
.text:00000000000006E1                 pop     rbp
.text:00000000000006E2                 retn
.text:00000000000006E2 common_func     endp
.text:00000000000006E2

.plt:00000000000005B0 ; ===========================================================================
.plt:00000000000005B0
.plt:00000000000005B0 ; Segment type: Pure code
.plt:00000000000005B0 ; Segment permissions: Read/Execute
.plt:00000000000005B0 _plt            segment para public 'CODE' use64
.plt:00000000000005B0                 assume cs:_plt
.plt:00000000000005B0                 ;org 5B0h
.plt:00000000000005B0                 assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
.plt:00000000000005B0
.plt:00000000000005B0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005B0
.plt:00000000000005B0
.plt:00000000000005B0 sub_5B0         proc near               ; CODE XREF: .plt:00000000000005CB↓j
.plt:00000000000005B0                 push    cs:qword_201008
.plt:00000000000005B6                 jmp     cs:qword_201010
.plt:00000000000005B6 sub_5B0         endp
.plt:00000000000005B6
.plt:00000000000005B6 ; ---------------------------------------------------------------------------
.plt:00000000000005BC                 align 20h
.plt:00000000000005C0
.plt:00000000000005C0 ; =============== S U B R O U T I N E =======================================
.plt:00000000000005C0
.plt:00000000000005C0 ; Attributes: thunk
.plt:00000000000005C0
.plt:00000000000005C0 ; int printf(const char *format, ...)
.plt:00000000000005C0 _printf         proc near               ; CODE XREF: common_func+21↓p
.plt:00000000000005C0                                         ; so_hello+21↓p ...
.plt:00000000000005C0                 jmp     cs:off_201018
.plt:00000000000005C0 _printf         endp
.plt:00000000000005C0
.plt:00000000000005C6 ; ---------------------------------------------------------------------------
.plt:00000000000005C6                 push    0
.plt:00000000000005CB                 jmp     sub_5B0
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
.plt.got:00000000000005D0
.plt.got:00000000000005D0 ; =============== S U B R O U T I N E =======================================
.plt.got:00000000000005D0
.plt.got:00000000000005D0 ; Attributes: thunk
.plt.got:00000000000005D0
.plt.got:00000000000005D0 j_common_func   proc near               ; CODE XREF: so_hello+2B↓p
.plt.got:00000000000005D0                 jmp     cs:common_func_ptr
.plt.got:00000000000005D0 j_common_func   endp
.plt.got:00000000000005D0
.plt.got:00000000000005D0 ; ---------------------------------------------------------------------------
.plt.got:00000000000005D6                 align 8
.plt.got:00000000000005D8
.plt.got:00000000000005D8 ; =============== S U B R O U T I N E =======================================
.plt.got:00000000000005D8
.plt.got:00000000000005D8 ; Attributes: thunk
.plt.got:00000000000005D8
.plt.got:00000000000005D8 __cxa_finalize  proc near               ; CODE XREF: __do_global_dtors_aux+1E↓p
.plt.got:00000000000005D8                 jmp     cs:__cxa_finalize_ptr
.plt.got:00000000000005D8 __cxa_finalize  endp
.plt.got:00000000000005D8
.plt.got:00000000000005D8 ; ---------------------------------------------------------------------------
.plt.got:00000000000005DE                 align 20h
.plt.got:00000000000005DE _plt_got        ends
.plt.got:00000000000005DE

.got:0000000000200FC8 ; ===========================================================================
.got:0000000000200FC8
.got:0000000000200FC8 ; Segment type: Pure data
.got:0000000000200FC8 ; Segment permissions: Read/Write
.got:0000000000200FC8 ; Segment alignment 'qword' can not be represented in assembly
.got:0000000000200FC8 _got            segment para public 'DATA' use64
.got:0000000000200FC8                 assume cs:_got
.got:0000000000200FC8                 ;org 200FC8h
.got:0000000000200FC8 _ITM_deregisterTMCloneTable_ptr dq offset _ITM_deregisterTMCloneTable
.got:0000000000200FC8                                         ; DATA XREF: deregister_tm_clones+17↑r
.got:0000000000200FD0 __gmon_start___ptr dq offset __gmon_start__
.got:0000000000200FD0                                         ; DATA XREF: _init_proc+4↑r
.got:0000000000200FD8 so_hello_ptr    dq offset so_hello      ; DATA XREF: so_hello+4↑r
.got:0000000000200FE0 common_func_ptr dq offset common_func   ; DATA XREF: j_common_func↑r
.got:0000000000200FE0                                         ; common_func+4↑r
.got:0000000000200FE8 g_so_long_ptr   dq offset g_so_long     ; DATA XREF: so_hello+30↑r
.got:0000000000200FE8                                         ; so_hello+3D↑r
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
.got.plt:0000000000201008 qword_201008    dq 0                    ; DATA XREF: sub_5B0↑r
.got.plt:0000000000201010 qword_201010    dq 0                    ; DATA XREF: sub_5B0+6↑r
.got.plt:0000000000201018 off_201018      dq offset printf        ; DATA XREF: _printf↑r
.got.plt:0000000000201018 _got_plt        ends
.got.plt:0000000000201018
.data:0000000000201020 ; ===========================================================================
.data:0000000000201020
.data:0000000000201020 ; Segment type: Pure data
.data:0000000000201020 ; Segment permissions: Read/Write
.data:0000000000201020 ; Segment alignment 'qword' can not be represented in assembly
.data:0000000000201020 _data           segment para public 'DATA' use64
.data:0000000000201020                 assume cs:_data
.data:0000000000201020                 ;org 201020h
.data:0000000000201020 __dso_handle    dq offset __dso_handle  ; DATA XREF: __do_global_dtors_aux+17↑r
.data:0000000000201020                                         ; .data:__dso_handle↓o
.data:0000000000201028                 public g_so_long
.data:0000000000201028 g_so_long       db    1                 ; DATA XREF: LOAD:0000000000000310↑o
.data:0000000000201028                                         ; .got:g_so_long_ptr↑o
.data:0000000000201029                 db    0
.data:000000000020102A                 db    0
.data:000000000020102B                 db    0
.data:000000000020102C                 db    0
.data:000000000020102D                 db    0
.data:000000000020102E                 db    0
.data:000000000020102F                 db    0
.data:000000000020102F _data           ends
.data:000000000020102F
```

```bash
$ readelf -r libso.so.1.0.0

Relocation section '.rela.dyn' at offset 0x488 contains 10 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000200de8  000000000008 R_X86_64_RELATIVE                    6b0
000000200df0  000000000008 R_X86_64_RELATIVE                    670
000000201020  000000000008 R_X86_64_RELATIVE                    201020
000000200fc8  000100000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTMClone + 0
000000200fd0  000300000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
000000200fd8  000800000006 R_X86_64_GLOB_DAT 00000000000006e3 so_hello + 0
000000200fe0  000d00000006 R_X86_64_GLOB_DAT 00000000000006ba common_func + 0
000000200fe8  000900000006 R_X86_64_GLOB_DAT 0000000000201028 g_so_long + 0
000000200ff0  000400000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCloneTa + 0
000000200ff8  000500000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0

Relocation section '.rela.plt' at offset 0x578 contains 1 entry:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000201018  000200000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
```

```bash
$ nm -C -D libso.so.1.0.0
0000000000201030 B __bss_start
00000000000006ba T common_func
                 w __cxa_finalize
0000000000201030 D _edata
0000000000201038 B _end
0000000000000744 T _fini
                 w __gmon_start__
0000000000201028 D g_so_long
0000000000000590 T _init
                 w _ITM_deregisterTMCloneTable
                 w _ITM_registerTMCloneTable
                 U printf
00000000000006e3 T so_hello

$ readelf --dyn-syms libso.so.1.0.0

Symbol table '.dynsym' contains 14 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
     3: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
     4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
     5: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)
     6: 0000000000201030     0 NOTYPE  GLOBAL DEFAULT   22 _edata
     7: 0000000000201038     0 NOTYPE  GLOBAL DEFAULT   23 _end
     8: 00000000000006e3    95 FUNC    GLOBAL DEFAULT   12 so_hello
     9: 0000000000201028     8 OBJECT  GLOBAL DEFAULT   22 g_so_long
    10: 0000000000201030     0 NOTYPE  GLOBAL DEFAULT   23 __bss_start
    11: 0000000000000590     0 FUNC    GLOBAL DEFAULT    9 _init
    12: 0000000000000744     0 FUNC    GLOBAL DEFAULT   13 _fini
    13: 00000000000006ba    41 FUNC    GLOBAL DEFAULT   12 common_func
```
