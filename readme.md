## 1. build & run 1st

bar: dynamic link foo

m: static link foo, dynamic load bar

### 1.1 build

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o m m.c libfoo.a -ldl
```

### 1.2 run

```bash
$ ./m
-- called in main:
func_foo@0x55ee6e02e8a0
init var_foo@0x55ee6e22f014
-- called in func_bar:
func_foo@0x7f6d92e2b66a
init var_foo@0x7f6d9302c02c

$ ldd m
    linux-vdso.so.1 (0x00007ffc71cdc000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fbaee68a000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fbaee299000)
    /lib64/ld-linux-x86-64.so.2 (0x00007fbaeea90000)

$ readelf --dyn-syms m | grep foo

$ LD_DEBUG=bindings ./m 2>&1 | grep -E '(func|var)_foo'
      5288: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
      5288: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
      5288: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x56018a6398a0
init var_foo@0x56018a83a014
func_foo@0x7f39dbbee66a
init var_foo@0x7f39dbdef02c
```

## 2. build & run 2nd

bar: dynamic link foo

m: dynamic link foo, dynamic load bar

### 2.1 build

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -o m m.c -Wl,-rpath,. -L. -lfoo -ldl
```

## 2.2 run

```bash
$ ./m
-- called in main:
func_foo@0x7f959be6a66a
init var_foo@0x7f959c06b02c
-- called in func_bar:
func_foo@0x7f959be6a66a

$ ldd m
    linux-vdso.so.1 (0x00007ffed5c2c000)
    libfoo.so => ./libfoo.so (0x00007fc6f3b98000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fc6f3994000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fc6f35a3000)
    /lib64/ld-linux-x86-64.so.2 (0x00007fc6f3f9c000)

$ readelf --dyn-syms m | grep foo
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND func_foo

s LD_DEBUG=bindings ./m 2>&1 | grep -E '(func|var)_foo'
      5462: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `var_foo'
      5462: binding file ./libfoo.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
      5462: binding file ./m [0] to ./libfoo.so [0]: normal symbol `func_foo'
      5462: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x7f8e49bff66a
init var_foo@0x7f8e49e0002c
func_foo@0x7f8e49bff66a
```

## 3. build & run 3rd

bar: dynamic link foo

m: static link foo, dynamic load bar, build with `--export-dynamic`

### 3.1 build

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o m m.c libfoo.a -Wl,--export-dynamic -ldl
```

## 3.2 run

```bash
$ ./m
-- called in main:
func_foo@0x564203541ac0
init var_foo@0x564203742014
-- called in func_bar:
func_foo@0x564203541ac0

$ ldd m
    linux-vdso.so.1 (0x00007fffc93cd000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f2b63608000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f2b63217000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f2b63a0e000)

$ readelf --dyn-syms m | grep foo
    23: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    25: 0000000000000ac0    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings ./m 2>&1 | grep -E '(func|var)_foo'
      5594: binding file ./libfoo.so [0] to ./m [0]: normal symbol `var_foo'
      5594: binding file ./libfoo.so [0] to ./m [0]: normal symbol `func_foo'
      5594: binding file ./libbar.so [0] to ./m [0]: normal symbol `func_foo'
func_foo@0x55aaf455fac0
init var_foo@0x55aaf4760014
func_foo@0x55aaf455fac0
```

## 4. build & run 4th

foo: build with `-Bsymbolic`

bar: dynamic link foo

m: static link foo, dynamic load bar, build with `--export-dynamic`

### 4.1 build

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o m m.c libfoo.a -Wl,--export-dynamic -ldl
```

### 4.2 run

```bash
$ ./m
-- called in main:
func_foo@0x5565dd314ac0
init var_foo@0x5565dd515014
-- called in func_bar:
func_foo@0x5565dd314ac0

$ ldd m
    linux-vdso.so.1 (0x00007ffd5959f000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fe48712a000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fe486d39000)
    /lib64/ld-linux-x86-64.so.2 (0x00007fe487530000)

$ readelf --dyn-syms m | grep foo
    23: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo
    25: 0000000000000ac0    75 FUNC    GLOBAL DEFAULT   14 func_foo

$ LD_DEBUG=bindings ./m 2>&1 | grep -E '(func|var)_foo'
      5719: binding file ./libbar.so [0] to ./m [0]: normal symbol `func_foo'
func_foo@0x55fca40e3ac0
init var_foo@0x55fca42e4014
func_foo@0x55fca40e3ac0
```

## 5. build & run 4th

foo: build with `-Bsymbolic`

bar: dynamic link foo

m: static link foo, dynamic load bar

### 5.1 build

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
gcc -o libbar.so -shared bar.o -Wl,-rpath,. -L. -lfoo
gcc -c foo.c
ar rcs libfoo.a foo.o
gcc -o m m.c libfoo.a -ldl
```

### 5.2 run

```bash
$ ./m
-- called in main:
func_foo@0x563b49c0a8a0
init var_foo@0x563b49e0b014
-- called in func_bar:
func_foo@0x7fa1a725563a
init var_foo@0x7fa1a745602c

$ ldd m
    linux-vdso.so.1 (0x00007fff2119c000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f0520cd1000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f05208e0000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f05210d7000)

$ readelf --dyn-syms m | grep foo

$ LD_DEBUG=bindings ./m 2>&1 | grep -E '(func|var)_foo'
      5772: binding file ./libbar.so [0] to ./libfoo.so [0]: normal symbol `func_foo'
func_foo@0x55cfc24508a0
init var_foo@0x55cfc2651014
func_foo@0x7fa8389ee63a
init var_foo@0x7fa838bef02c
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
