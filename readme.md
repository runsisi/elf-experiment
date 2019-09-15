## build 1st

bar: dynamic link foo

m: static link foo, dynamic load bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -L. -lfoo
gcc -c m.c foo.c
gcc -o m m.o foo.o -ldl
```

## run 1st

```bash
$ export LD_LIBRARY_PATH=$PWD

$ ./m
called in main
init var_foo@0x56355bdfe014
called in bar
init var_foo@0x7faf74e7802c

$ ldd m
    linux-vdso.so.1 (0x00007ffe42e3e000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f9eea1da000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f9ee9de9000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f9eea5e0000)

$ readelf --dyn-syms m | grep var_foo

$ LD_DEBUG=bindings ./m 2>&1 | grep var_foo
      5815: binding file /home/runsisi/.../libfoo.so [0] to /home/runsisi/.../libfoo.so [0]: normal symbol `var_foo'
init var_foo@0x55e9a3f85014
init var_foo@0x7fcf7e83902c
```

## build 2nd

bar: dynamic link foo

m: dynamic link foo, dynamic load bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -L. -lfoo
gcc -c m.c
gcc -o m m.o -L. -lfoo -Wl,-rpath-link=$(pwd) -ldl
```

## run 2nd

```bash
$ export LD_LIBRARY_PATH=$PWD

$ ./m
called in main
init var_foo@0x7f18869bf02c
called in bar

$ ldd m
    linux-vdso.so.1 (0x00007ffd21b28000)
    libfoo.so => /home/runsisi/working/eclipse-workspace/test/static/libfoo.so (0x00007f192841e000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f192821a000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f1927e29000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f1928822000)

$ readelf --dyn-syms m | grep var_foo

$ LD_DEBUG=bindings ./m 2>&1 | grep var_foo
      5770: binding file /home/runsisi/.../libfoo.so [0] to /home/runsisi/.../libfoo.so [0]: normal symbol `var_foo'
init var_foo@0x7f6e167b602c
```

## build 3rd

bar: dynamic link foo

m: static link foo, dynamic load bar

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -L. -lfoo
gcc -c m.c foo.c
gcc -o m m.o foo.o -Wl,-rpath-link=$(pwd) -Wl,--export-dynamic -ldl
```

## run 3rd

```bash
$ export LD_LIBRARY_PATH=$PWD

$ ./m
called in main
init var_foo@0x559c2e6b6014
called in bar

$ ldd m
    linux-vdso.so.1 (0x00007ffca3caf000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f717a06c000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f7179c7b000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f717a472000)

$ readelf --dyn-syms m | grep var_foo
    24: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo

$ LD_DEBUG=bindings ./m 2>&1 | grep var_foo
      5707: binding file /home/runsisi/.../libfoo.so [0] to ./m [0]: normal symbol `var_foo'
init var_foo@0x556563dae014
```

## build 4th

foo: build with -Bsymbolic

bar: dynamic link foo

m: static link foo, dynamic load bar

```bash
$ gcc -c -fPIC foo.c
$ gcc -c -fPIC bar.c
$ gcc -o libfoo.so -shared foo.o -Wl,-Bsymbolic
$ gcc -o libbar.so -shared bar.o -L. -lfoo
$ gcc -c m.c foo.c
$ gcc -o m m.o foo.o -Wl,-rpath-link=$(pwd) -Wl,--export-dynamic -ldl
```

## run 4th

```bash
$ ./m
called in main
init var_foo@0x55fd99d20014
called in bar

$ ldd m
    linux-vdso.so.1 (0x00007ffce81f9000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007fb1f8bb3000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fb1f87c2000)
    /lib64/ld-linux-x86-64.so.2 (0x00007fb1f8fb9000)

$ readelf --dyn-syms m | grep var_foo
    24: 0000000000201014     4 OBJECT  GLOBAL DEFAULT   24 var_foo

$ LD_DEBUG=bindings ./m 2>&1 | grep var_foo
init var_foo@0x5651e856a014
```

## references

ld - The GNU linker

http://man7.org/linux/man-pages/man1/ld.1.html

ld.so, ld-linux.so - dynamic linker/loader

http://man7.org/linux/man-pages/man8/ld.so.8.html
