#include "z-test.h"

void z_usage(int argc, char **argv) {

	if(argc < 3) {
		puts("usage: ./z-test-xxx <iterations> <num> ... <numX>");

		puts(
			"\toptions:\n"
			"\t\t0 ::: executes FLUSHALL\n"
			"\t\t1:<num> ::: executes SET key:<i> where i from 0 to <num>\n"
			"\t\t2 ::: executes a keys *\n"
			"\t\t3:<num> ::: executes INCR key <num> times\n"
			"\t\t4:<num> ::: executes SADDR key <0 to num> times\n"
		);

		exit(-1);
	}

	return;
}


int z_apply_op(void *c, int iterator_max, int iterator_arg, z_ops o[], int ops_sz, char *arg) {

	int arg_index = -1, n;
	char * sep = NULL, *arg_ptr=NULL;
	int (*fn)(void *, char *);

	if(!arg) return -1;

	sep = strstr(arg, ":");
	if(sep) {
		sep = sep + 1;
	}
	else sep = arg;

	arg_index = atoi(arg);

	printf("apply_op: arg_index=%i ops_sz=%i\n", arg_index, ops_sz);

	if(arg_index < 0 || arg_index >= ops_sz) return -1;

	fn = o[arg_index].cb;
	if(!fn) return -1;

	arg_ptr = strdup(sep);
	n = fn(c, arg_ptr);
	free(arg_ptr);

	return n;
}
