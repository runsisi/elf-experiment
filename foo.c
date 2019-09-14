/*
 * foo.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>

int var_foo = 0;

void foo() {
  if (var_foo == 0) {
    var_foo = 3;
    printf("init var_foo@%p\n", &var_foo);
  }
}
