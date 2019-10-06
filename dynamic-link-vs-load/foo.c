/*
 * foo.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>

int var_foo = 0;

void func_foo() {
  printf("- %s@%p\n", __func__, func_foo);
  if (var_foo == 0) {
    var_foo = 3;
    printf("initialize var_foo@%p\n", &var_foo);
  }
}
