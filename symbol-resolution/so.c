#include <stdio.h>

#define EXPORT __attribute__((visibility("default")))

EXPORT long g_so_long = 1;

EXPORT void common_func() {
    printf("%s@%p from .so\n", __func__, common_func);
}

EXPORT
int so_hello() {
    printf("- %s@%p:\n", __func__, so_hello);
    common_func();
    printf("g_so_long@%p = %ld\n", &g_so_long, g_so_long);
    return 's';
}
