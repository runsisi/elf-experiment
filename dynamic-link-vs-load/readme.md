### 1.1 dynamic link

bar: dynamic link foo

main: static link foo, dynamic link bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main main.c libfoo.a -Wl,-rpath-link,. -L. -lbar
```

```bash
$ LD_LIBRARY_PATH=. ./main
-- main@0x5558da8267aa
- func_foo@0x5558da8267e8
initialize var_foo@0x5558daa27014
- func_bar@0x7fb0580f467a
- func_foo@0x5558da8267e8
$ ldd main
        linux-vdso.so.1 (0x00007fff30f99000)
        libbar.so => not found
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f7307fb7000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f73085aa000)
$ readelf --dyn-syms main | grep foo
    12: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    14: 00000000000007e8    82 FUNC    GLOBAL DEFAULT   14 func_foo
$ LD_DEBUG=bindings  LD_LIBRARY_PATH=. ./main 2>&1 | grep -E '(func|var)_foo'
    334915:     binding file ./libfoo.so [0] to ./main [0]: normal symbol `var_foo'
    334915:     binding file ./libfoo.so [0] to ./main [0]: normal symbol `func_foo'
    334915:     binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
- func_foo@0x55fb7d0b87e8
initialize var_foo@0x55fb7d2b9014
- func_foo@0x55fb7d0b87e8
```

### 1.2 dynamic load

bar: dynamic link foo

main: static link foo, dynamic load bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main -DDYN_LOAD main.c libfoo.a -ldl
```

```bash
$ ./main
-- main@0x5652e4a5279a
- func_foo@0x5652e4a52873
initialize var_foo@0x5652e4c53014
- func_bar@0x7f152096b67a
- func_foo@0x7f152076966a
initialize var_foo@0x7f152096a02c
$ ldd main
        linux-vdso.so.1 (0x00007ffe581ce000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f9947fa5000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f9947bb4000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f99483ab000)
$ readelf --dyn-syms main | grep foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336076:     binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
    336076:     binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
    336076:     binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
- func_foo@0x558cb6e52873
initialize var_foo@0x558cb7053014
- func_foo@0x7f22094e166a
initialize var_foo@0x7f22096e202c
```

### 2.1 dynamic link

bar: dynamic link foo

main: dynamic link foo, dynamic link bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -o main main.c -Wl,-rpath,. -L. -lbar -lfoo
```

```bash
$ ./main
-- main@0x564fb0cef7aa
- func_foo@0x7f39087e366a
initialize var_foo@0x7f39089e402c
- func_bar@0x7f39089e567a
- func_foo@0x7f39087e366a
$ ldd main
        linux-vdso.so.1 (0x00007ffecb16d000)
        libbar.so => ./libbar.so (0x00007f53524b0000)
        libfoo.so => ./libfoo.so (0x00007f53522ae000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f5351ebd000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f53528b4000)
$ readelf --dyn-syms main | grep foo
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336124:     binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
    336124:     binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
    336124:     binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
    336124:     binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
- func_foo@0x7fa4af4cc66a
initialize var_foo@0x7fa4af6cd02c
- func_foo@0x7fa4af4cc66a
```

### 2.2 dynamic load

bar: dynamic link foo

main: dynamic link foo, dynamic load bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -o main -DDYN_LOAD main.c -Wl,-rpath,. -L. -lfoo -ldl
```

```bash
$ ./main
-- main@0x55b8928458aa
- func_foo@0x7f969761866a
initialize var_foo@0x7f969781902c
- func_bar@0x7f9696e2167a
- func_foo@0x7f969761866a
$ ldd main
        linux-vdso.so.1 (0x00007ffefe4d2000)
        libfoo.so => ./libfoo.so (0x00007fea85e96000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fea85c92000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fea858a1000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fea8629a000)
$ readelf --dyn-syms main | grep foo
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336168:     binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
    336168:     binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
    336168:     binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
    336168:     binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
- func_foo@0x7fb916fde66a
initialize var_foo@0x7fb9171df02c
- func_foo@0x7fb916fde66a
```

### 3.1 dynamic link

foo: build with `-Bsymbolic`

bar: dynamic link foo

main: static link foo, dynamic link bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main main.c libfoo.a -Wl,-rpath-link,. -L. -lbar
```

```bash
$ LD_LIBRARY_PATH=. ./main
-- main@0x5560d85a57aa
- func_foo@0x5560d85a57e8
initialize var_foo@0x5560d87a6014
- func_bar@0x7f078f97e67a
- func_foo@0x5560d85a57e8
$ ldd main
        linux-vdso.so.1 (0x00007ffdea142000)
        libbar.so => not found
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f4e2c53a000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f4e2cb2d000)
$ readelf --dyn-syms main | grep foo
    12: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    14: 00000000000007e8    82 FUNC    GLOBAL DEFAULT   14 func_foo
$ LD_DEBUG=bindings LD_LIBRARY_PATH=. ./main 2>&1 | grep -E '(func|var)_foo'
    336211:     binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
- func_foo@0x56513f3e67e8
initialize var_foo@0x56513f5e7014
- func_foo@0x56513f3e67e8
```

### 3.2 dynamic load

foo: build with `-Bsymbolic`

bar: dynamic link foo

main: static link foo, dynamic load bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main -DDYN_LOAD main.c libfoo.a -ldl
```

```bash
$ ./main
-- main@0x559f98a3779a
- func_foo@0x559f98a37873
initialize var_foo@0x559f98c38014
- func_bar@0x7faf7c34567a
- func_foo@0x7faf7c14363a
initialize var_foo@0x7faf7c34402c
$ ldd main
        linux-vdso.so.1 (0x00007ffc414d9000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f492f382000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f492ef91000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f492f788000)
$ readelf --dyn-syms main | grep foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336255:     binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
- func_foo@0x559810185873
initialize var_foo@0x559810386014
- func_foo@0x7fb06467d63a
initialize var_foo@0x7fb06487e02c
```

### 4.1 dynamic link

foo: build with `-Bsymbolic`

bar: dynamic link foo

main: dynamic link foo, dynamic link bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath=. -L. -lfoo
gcc -o main main.c -Wl,-rpath=. -L. -lbar -lfoo
```

```bash
$ ./main
-- main@0x561e6b95a7aa
- func_foo@0x7f702464763a
initialize var_foo@0x7f702484802c
- func_bar@0x7f702484967a
- func_foo@0x7f702464763a
$ ldd main
        linux-vdso.so.1 (0x00007ffcddd0b000)
        libbar.so => ./libbar.so (0x00007f97a9a55000)
        libfoo.so => ./libfoo.so (0x00007f97a9853000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f97a9462000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f97a9e59000)
$ readelf --dyn-syms main | grep foo
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336294:     binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
    336294:     binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
- func_foo@0x7f162e65b63a
initialize var_foo@0x7f162e85c02c
- func_foo@0x7f162e65b63a
```

### 4.2 dynamic load

foo: build with `-Bsymbolic`

bar: dynamic link foo

main: dynamic link foo, dynamic load bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath=. -L. -lfoo
gcc -o main -DDYN_LOAD main.c -Wl,-rpath=. -L. -lfoo -ldl
```

```bash
$ ./main
-- main@0x5600c70068aa
- func_foo@0x7f9c82e3863a
initialize var_foo@0x7f9c8303902c
- func_bar@0x7f9c8264167a
- func_foo@0x7f9c82e3863a
$ ldd main
        linux-vdso.so.1 (0x00007fff0c5a7000)
        libfoo.so => ./libfoo.so (0x00007f9f1e394000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f9f1e190000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f9f1dd9f000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f9f1e798000)
$ readelf --dyn-syms main | grep foo
     2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336333:     binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
    336333:     binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
- func_foo@0x7fe21dae763a
initialize var_foo@0x7fe21dce802c
- func_foo@0x7fe21dae763a
```

### 5.1 dynamic link

bar: dynamic link foo

main: static link foo, dynamic link bar, build with `--export-dynamic`

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main main.c libfoo.a -Wl,-rpath,. -L. -lbar -Wl,--export-dynamic
```

```bash
$ ./main
-- main@0x55851c1af8ba
- func_foo@0x55851c1af8f8
initialize var_foo@0x55851c3b0014
- func_bar@0x7fe548b4d67a
- func_foo@0x55851c1af8f8
$ ldd main
        linux-vdso.so.1 (0x00007ffd2c9cb000)
        libbar.so => ./libbar.so (0x00007fec2d563000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fec2d172000)
        libfoo.so => ./libfoo.so (0x00007fec2cf70000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fec2d967000)
$ readelf --dyn-syms main | grep foo
    19: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    21: 00000000000008f8    82 FUNC    GLOBAL DEFAULT   14 func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336401:     binding file ./libfoo.so [0] to ./main [0]: normal symbol `var_foo'
    336401:     binding file ./libfoo.so [0] to ./main [0]: normal symbol `func_foo'
    336401:     binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
- func_foo@0x55b21679e8f8
initialize var_foo@0x55b21699f014
- func_foo@0x55b21679e8f8
```

### 5.2 dynamic load

bar: dynamic link foo

main: static link foo, dynamic load bar, build with `--export-dynamic`

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main -DDYN_LOAD main.c libfoo.a -Wl,--export-dynamic -ldl
```

```bash
$ ./main
-- main@0x55645f3be9aa
- func_foo@0x55645f3bea83
initialize var_foo@0x55645f5bf014
- func_bar@0x7f5e972ef67a
- func_foo@0x55645f3bea83
$ ldd main
        linux-vdso.so.1 (0x00007ffc10394000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f8a35468000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8a35077000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f8a3586e000)
$ readelf --dyn-syms main | grep foo
    22: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    24: 0000000000000a83    82 FUNC    GLOBAL DEFAULT   14 func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336532:     binding file ./libfoo.so [0] to ./main [0]: normal symbol `var_foo'
    336532:     binding file ./libfoo.so [0] to ./main [0]: normal symbol `func_foo'
    336532:     binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
- func_foo@0x56098d6b9a83
initialize var_foo@0x56098d8ba014
- func_foo@0x56098d6b9a83
```

### 6.1 dynamic link

foo: build with `-Bsymbolic`

bar: dynamic link foo

main: static link foo, dynamic link bar, build with `--export-dynamic`

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main main.c libfoo.a -Wl,-rpath,. -L. -lbar -Wl,--export-dynamic
```

```bash
$ ./main
-- main@0x56480d7868ba
- func_foo@0x56480d7868f8
initialize var_foo@0x56480d987014
- func_bar@0x7f7668ed067a
- func_foo@0x56480d7868f8
$ ldd main
        linux-vdso.so.1 (0x00007ffe4bdf5000)
        libbar.so => ./libbar.so (0x00007f718e741000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f718e350000)
        libfoo.so => ./libfoo.so (0x00007f718e14e000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f718eb45000)
$ readelf --dyn-syms main | grep foo
    19: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    21: 00000000000008f8    82 FUNC    GLOBAL DEFAULT   14 func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336579:     binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
- func_foo@0x5585706348f8
initialize var_foo@0x558570835014
- func_foo@0x5585706348f8
```

### 6.2 dynamic load

foo: build with `-Bsymbolic`

bar: dynamic link foo

main: static link foo, dynamic load bar, build with `--export-dynamic`

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o main -DDYN_LOAD main.c libfoo.a -Wl,--export-dynamic -ldl
```

```bash
$ ./main
-- main@0x5652548209aa
- func_foo@0x565254820a83
initialize var_foo@0x565254a21014
- func_bar@0x7f6cebd3f67a
- func_foo@0x565254820a83
$ ldd main
        linux-vdso.so.1 (0x00007fe18065f000)
        libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fe180034000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fe17fc43000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fe18043a000)
$ readelf --dyn-syms main | grep foo
    22: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    24: 0000000000000a83    82 FUNC    GLOBAL DEFAULT   14 func_foo
$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
    336620:     binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
- func_foo@0x563545d14a83
initialize var_foo@0x563545f15014
- func_foo@0x563545d14a83
```

## references

ld - The GNU linker

http://man7.org/linux/man-pages/man1/ld.1.html

ld.so, ld-linux.so - dynamic linker/loader

http://man7.org/linux/man-pages/man8/ld.so.8.html

dlopen - open a symbol table handle

https://pubs.opengroup.org/onlinepubs/9699919799/nframe.html

Better understanding Linux secondary dependencies solving with examples

http://www.kaizou.org/2015/01/linux-libraries/
