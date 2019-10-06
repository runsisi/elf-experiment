#include <stdio.h>

extern char g_x_char;
extern int x_hello();

extern long g_so_long;
extern int so_hello();

int main() {
    printf("-- %s@%p\n", __func__, main);
    x_hello();
    so_hello();

    printf("\n-- %s@%p\n", __func__, main);
    g_x_char = 'b';
    g_so_long = 2;

    printf("g_x_char@%p = %c\n", &g_x_char, g_x_char);
    printf("g_so_long%p = %ld\n", &g_so_long, g_so_long);

    printf("\n-- %s@%p\n", __func__, main);
    x_hello();
    so_hello();

    return 0;
}
