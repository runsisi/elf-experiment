/*
 * m.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>

extern void func_foo();

#ifdef DYN_LOAD
#include <dlfcn.h>
typedef void (*func_bar_t)();
#else
extern void func_bar();
#endif

int main() {
  printf("-- %s@%p\n", __func__, main);
  func_foo();

#ifdef DYN_LOAD
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
#else
  func_bar();
#endif

  return 0;
}


