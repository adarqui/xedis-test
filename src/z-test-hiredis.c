#include "z-test.h"

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <locale.h>

#include <hiredis/hiredis.h>

int z_flushall(void *cptr, char * arg) {
	redisContext *c = (redisContext *) cptr;
	redisReply *rr;

	printf("z_flushall: %s\n", arg);

	rr = redisCommand(c, "FLUSHALL");
	if(rr)
		freeReplyObject(rr);

	return 0;
}

int z_set_incremental(void *cptr, char *arg) {
	redisContext *c = (redisContext *) cptr;
	redisReply *rr;
	int i, iterator = 1000;
	char buf[10];

	printf("z_set_incremental: %s\n", arg);

	if(arg != NULL) {
		iterator = atoi(arg);
	}

	for(i = 0; i < iterator; i++) {
		snprintf(buf,sizeof(buf)-1,"%i",i);
		rr = redisCommand(c, "SET %s %s", buf, "hello");
		if(rr)
			freeReplyObject(rr);
	}

	return 0;
}

int z_sadd_incremental(void *cptr, char *arg) {
	redisContext *c = (redisContext *) cptr;
	redisReply *rr;
	int i, iterator_x = 0, iterator_y = 1000;
	char * ptr_name = NULL, *ptr_x = NULL, *ptr_y = NULL, *dup=NULL;

	printf("z_sadd_incremental: %s\n", arg);

	if(arg) {
		dup = strdup(arg);
		ptr_name = strtok(dup, ",");
		if(!ptr_name) return -1;

		ptr_x = strtok(NULL, ",");
		if(!ptr_x) return -1;

		ptr_y = strtok(NULL, "");
		if(!ptr_y) return -1;

		iterator_x = atoi(ptr_x);
		iterator_y = atoi(ptr_y);
	}

	if(!ptr_name || !ptr_x || !ptr_y) return -1;

	for(i = iterator_x; i < iterator_y; i++) {
		rr = redisCommand(c, "SADD %s %i", ptr_name, i);
		if(rr) freeReplyObject(rr);
	}

	if(dup) free(dup);
	return 0;
}


int z_keys_star(void *cptr, char *arg) {
	redisContext *c = (redisContext *) cptr;
	redisReply *rr;
	
	printf("z_keys_star: %s\n", arg);

	rr = redisCommand(c, "KEYS %s", "*");
	if(rr)
		freeReplyObject(rr);

	return 0;
}


int z_incr_key(void *cptr, char *arg) {
	redisContext *c = (redisContext *) cptr;
	redisReply *rr;
	int i, increment_max = 1000;

	printf("z_incr_key: %s\n", arg);

	if(arg) {
		increment_max = atoi(arg);
	}

	for(i = 0; i < increment_max; i++) {
		rr = redisCommand(c, "incr %s", "incr");
		if(rr)
			freeReplyObject(rr);
	}

	return 0;
}



z_ops ops[6] = {
	{ "FLUSHALL", z_flushall },
	{ "SET [0:ARG] 1", z_set_incremental },
	{ "KEYS *", z_keys_star },
	{ "INCR incr ::: [x] amount", z_incr_key },
	{ "SADDR <key> <val>", z_sadd_incremental },
	{ NULL, NULL },
};


int main(int argc, char **argv) {
	redisContext * c = NULL;
	int i, j, iterator_max;

	z_usage(argc, argv);

	c = redisConnectUnix("/tmp/redis.sock");
	if(!c) errx(-1, "main:redisConnect:Failed");
	else if(c && c->err) errx(-1, "main:redisConnect:Failed:%i", c->err);

	iterator_max = atoi(argv[1]);
	for(i = 0; i < iterator_max; i++) {
		
		for(j = 2; j < argc; j++) {
			z_apply_op(c, i, j, ops, (sizeof(ops)/sizeof(z_ops))-1, argv[j]);
		}
	}

	redisFree(c);
}
