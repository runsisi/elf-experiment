/*
 * bar.c
 *
 *  Created on: Sep 15, 2019
 *      Author: runsisi
 */

#include <stdio.h>

extern void foo();

void bar() {
  printf("called in bar\n");
  foo();
}
