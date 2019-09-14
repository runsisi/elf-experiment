/*
 * m.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>
#include <dlfcn.h>

extern void foo();

typedef void (*bar_t)();

int main() {
  printf("called in main\n");
  foo();

  void *handle = dlopen("./libbar.so", RTLD_LAZY);
  if (!handle) {
    char* err = dlerror();
    printf("dlopen error: %s\n", err);
    return -1;
  }

  bar_t bar = (bar_t)dlsym(handle, "bar");
  if (!bar) {
    char* err = dlerror();
    printf("dlsym error: %s\n", err);
    return -1;
  }

  bar();

  dlclose(handle);

  return 0;
}


