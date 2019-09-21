/*
 * bar.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>

extern void func_foo();

void func_bar() {
  printf("- %s@%p:\n", __func__, func_bar);
  func_foo();
}
