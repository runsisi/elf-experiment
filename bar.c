/*
 * bar.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>

extern void func_foo();

void func_bar() {
  printf("-- called in func_bar:\n");
  func_foo();
}
