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
-- called in main:
func_foo@0x55b685477815
init var_foo@0x55b685678014
-- called in func_bar:
func_foo@0x55b685477815

$ ldd main
    linux-vdso.so.1 (0x00007fffc2ffd000)
    libbar.so => not found
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f7e2aeab000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f7e2b49e000)

$ readelf --dyn-syms main | grep foo
    13: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    15: 0000000000000815    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings  LD_LIBRARY_PATH=. ./main 2>&1 | grep -E '(func|var)_foo'
      6968: binding file ./libfoo.so [0] to ./main [0]: normal symbol `var_foo'
      6968: binding file ./libfoo.so [0] to ./main [0]: normal symbol `func_foo'
      6968: binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
func_foo@0x5580112c4815
init var_foo@0x5580114c5014
func_foo@0x5580112c4815
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
-- called in main:
func_foo@0x561a812bb8a0
init var_foo@0x561a814bc014
-- called in func_bar:
func_foo@0x7f8395a8966a
init var_foo@0x7f8395c8a02c

$ ldd main
    linux-vdso.so.1 (0x00007ffce7f9f000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f5409aa2000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f54096b1000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f5409ea8000)

$ readelf --dyn-syms main | grep foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      7315: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
      7315: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
      7315: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x56021b4618a0
init var_foo@0x56021b662014
func_foo@0x7f275733766a
init var_foo@0x7f275753802c
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
-- called in main:
func_foo@0x7f87d7e8866a
init var_foo@0x7f87d808902c
-- called in func_bar:
func_foo@0x7f87d7e8866a

$ ldd main
    linux-vdso.so.1 (0x00007ffd1a2ff000)
    libbar.so => ./libbar.so (0x00007f4c9c820000)
    libfoo.so => ./libfoo.so (0x00007f4c9c61e000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f4c9c22d000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f4c9cc24000)

$ readelf --dyn-syms main | grep foo
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      7197: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
      7197: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
      7197: binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
      7197: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x7f99bc3a766a
init var_foo@0x7f99bc5a802c
func_foo@0x7f99bc3a766a
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
-- called in main:
func_foo@0x7f8c4c38d66a
init var_foo@0x7f8c4c58e02c
-- called in func_bar:
func_foo@0x7f8c4c38d66a

$ ldd main
    linux-vdso.so.1 (0x00007ffeb2184000)
    libfoo.so => ./libfoo.so (0x00007f75576a1000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f755749d000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f75570ac000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f7557aa5000)

$ readelf --dyn-syms main | grep foo
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      7350: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
      7350: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
      7350: binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
      7350: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x7fe0068df66a
init var_foo@0x7fe006ae002c
func_foo@0x7fe0068df66a
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
-- called in main:
func_foo@0x562d80b5c815
init var_foo@0x562d80d5d014
-- called in func_bar:
func_foo@0x562d80b5c815

$ ldd main
    linux-vdso.so.1 (0x00007fffd4546000)
    libbar.so => not found
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8155812000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f8155e05000)

$ readelf --dyn-syms main | grep foo
    13: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    15: 0000000000000815    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings LD_LIBRARY_PATH=. ./main 2>&1 | grep -E '(func|var)_foo'
      7239: binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
func_foo@0x557c884ed815
init var_foo@0x557c886ee014
func_foo@0x557c884ed815
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
-- called in main:
func_foo@0x563b49c0a8a0
init var_foo@0x563b49e0b014
-- called in func_bar:
func_foo@0x7fa1a725563a
init var_foo@0x7fa1a745602c

$ ldd main
    linux-vdso.so.1 (0x00007fff2119c000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f0520cd1000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f05208e0000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f05210d7000)

$ readelf --dyn-syms main | grep foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      5772: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x55cfc24508a0
init var_foo@0x55cfc2651014
func_foo@0x7fa8389ee63a
init var_foo@0x7fa838bef02c
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
-- called in main:
func_foo@0x7fc24c14d63a
init var_foo@0x7fc24c34e02c
-- called in func_bar:
func_foo@0x7fc24c14d63a

$ ldd main
    linux-vdso.so.1 (0x00007ffe3fdc9000)
    libbar.so => ./libbar.so (0x00007f4f95a08000)
    libfoo.so => ./libfoo.so (0x00007f4f95806000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f4f95415000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f4f95e0c000)

$ readelf --dyn-syms main | grep foo
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      7273: binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
      7273: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x7f513e4c463a
init var_foo@0x7f513e6c502c
func_foo@0x7f513e4c463a
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
-- called in main:
func_foo@0x7fc24c14d63a
init var_foo@0x7fc24c34e02c
-- called in func_bar:
func_foo@0x7fc24c14d63a

$ ldd main
    linux-vdso.so.1 (0x00007ffe3fdc9000)
    libbar.so => ./libbar.so (0x00007f4f95a08000)
    libfoo.so => ./libfoo.so (0x00007f4f95806000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f4f95415000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f4f95e0c000)

$ readelf --dyn-syms main | grep foo
     4: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      7273: binding file ./main [0] to ./libfoo.so [0]: normal symbol `func_foo'
      7273: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x7f513e4c463a
init var_foo@0x7f513e6c502c
func_foo@0x7f513e4c463a
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
-- called in main:
func_foo@0x564203541ac0
init var_foo@0x564203742014
-- called in func_bar:
func_foo@0x564203541ac0

$ ldd main
    linux-vdso.so.1 (0x00007fffc93cd000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f2b63608000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f2b63217000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f2b63a0e000)

$ readelf --dyn-syms main | grep foo
    23: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    25: 0000000000000ac0    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      5594: binding file ./libfoo.so [0] to ./main [0]: normal symbol `var_foo'
      5594: binding file ./libfoo.so [0] to ./main [0]: normal symbol `func_foo'
      5594: binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
func_foo@0x55aaf455fac0
init var_foo@0x55aaf4760014
func_foo@0x55aaf455fac0
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
-- called in main:
func_foo@0x564203541ac0
init var_foo@0x564203742014
-- called in func_bar:
func_foo@0x564203541ac0

$ ldd main
    linux-vdso.so.1 (0x00007fffc93cd000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f2b63608000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f2b63217000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f2b63a0e000)

$ readelf --dyn-syms main | grep foo
    23: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    25: 0000000000000ac0    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      5594: binding file ./libfoo.so [0] to ./main [0]: normal symbol `var_foo'
      5594: binding file ./libfoo.so [0] to ./main [0]: normal symbol `func_foo'
      5594: binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
func_foo@0x55aaf455fac0
init var_foo@0x55aaf4760014
func_foo@0x55aaf455fac0
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
-- called in main:
func_foo@0x5565dd314ac0
init var_foo@0x5565dd515014
-- called in func_bar:
func_foo@0x5565dd314ac0

$ ldd main
    linux-vdso.so.1 (0x00007ffd5959f000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fe48712a000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fe486d39000)
    /lib64/ld-linux-x86-64.so.2 (0x00007fe487530000)

$ readelf --dyn-syms main | grep foo
    23: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    25: 0000000000000ac0    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      5719: binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
func_foo@0x55fca40e3ac0
init var_foo@0x55fca42e4014
func_foo@0x55fca40e3ac0
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
-- called in main:
func_foo@0x5565dd314ac0
init var_foo@0x5565dd515014
-- called in func_bar:
func_foo@0x5565dd314ac0

$ ldd main
    linux-vdso.so.1 (0x00007ffd5959f000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fe48712a000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fe486d39000)
    /lib64/ld-linux-x86-64.so.2 (0x00007fe487530000)

$ readelf --dyn-syms main | grep foo
    23: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    25: 0000000000000ac0    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings ./main 2>&1 | grep -E '(func|var)_foo'
      5719: binding file ./libbar.so [0] to ./main [0]: normal symbol `func_foo'
func_foo@0x55fca40e3ac0
init var_foo@0x55fca42e4014
func_foo@0x55fca40e3ac0
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
