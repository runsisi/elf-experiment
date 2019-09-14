## build 1st

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -L. -lfoo
gcc -c m.c foo.c
gcc -o m m.o foo.o -L. -lbar -Wl,-rpath-link=$(pwd) -ldl
```

## run 1st

```bash
$ ./m
called in main
init var_foo@0x55f8cee00014
called in bar
init var_foo@0x7fe26c0e802c
```

## build 2nd

```bash
gcc -c -fPIC foo.c
gcc -c -fPIC bar.c
gcc -o libfoo.so -shared foo.o
gcc -o libbar.so -shared bar.o -L. -lfoo
gcc -c m.c
gcc -o m m.o -L. -lbar -lfoo -Wl,-rpath-link=$(pwd) -ldl
```

## run 2nd

```bash
$ ./m
called in main
init var_foo@0x7f737304102c
called in bar
```