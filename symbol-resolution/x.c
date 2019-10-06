#include <stdio.h>

char g_x_char = 'a';

void common_func() {
    printf("%s@%p from x.o\n", __func__, common_func);
}

int x_hello() {
    printf("- %s@%p\n", __func__, x_hello);
    common_func();
    printf("g_x_char@%p = %c\n", &g_x_char, g_x_char);
    return 'x';
}
