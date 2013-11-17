#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (* z_test)(void *, char *);

typedef struct Z_OPS {
	char * name;
	int (*cb)(void *, char *);
} z_ops;

#endif
