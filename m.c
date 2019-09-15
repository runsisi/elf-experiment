/*
 * m.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>
#include <dlfcn.h>

extern void func_foo();

typedef void (*func_bar_t)();

int main() {
  printf("-- called in main:\n");
  func_foo();

  void *handle = dlopen("./libbar.so", RTLD_LAZY);
  if (!handle) {
    char* err = dlerror();
    printf("dlopen error: %s\n", err);
    return -1;
  }

  func_bar_t func_bar = (func_bar_t)dlsym(handle, "func_bar");
  if (!func_bar) {
    char* err = dlerror();
    printf("dlsym error: %s\n", err);
    return -1;
  }

  func_bar();

  dlclose(handle);

  return 0;
}


