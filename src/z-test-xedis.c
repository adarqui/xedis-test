#include "z-test.h"

#include "redis.h"
#include "cluster.h"
#include "slowlog.h"
#include "bio.h"

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


int z_set(void *cptr, char * key, char * val) {
	redisClient *c = (redisClient *) cptr;
	robj *rkey, *rval;
	rkey = createStringObject(key, strlen(key));
	rval = createStringObject(val, strlen(val));

	setGenericCommand(c, 0, rkey, rval, NULL, 0, NULL, NULL);

	freeStringObject(rkey);
	freeStringObject(rval);

	return 0;
}

int z_sadd(void *cptr, char * name, char * val) {
	redisClient *c = (redisClient *)cptr;
	robj *argv[10], *sadd, *rkey, *rval;

	sadd = createStringObject("SADD", 4);
	rkey = createStringObject(name, strlen(name));
	rval = createStringObject(val, strlen(val));

	argv[0] = sadd;
	argv[1] = rkey;
	argv[2] = rval;

	c->argv = argv;
	c->argc = 3;

	saddCommand(c);

	c->argv = NULL;
	c->argc = 0;

	freeStringObject(sadd);
	freeStringObject(rkey);
	// FAILS! tryObjectEncoding() in t_set.c .. freeStringObject(rval);

	return 0;
}


int z_flushall(void *cptr, char * arg) {
	redisClient *c = (redisClient *) cptr;
	printf("z_flushall: %s\n", arg);
	flushallCommand(c);
}

int z_set_incremental(void *cptr, char *arg) {
	redisClient *c = (redisClient *)cptr;
	char buf[10];
	int i, iterator = 1000;

	printf("z_set_incremental: %s\n", arg);
	
	if(arg) {
		iterator = atoi(arg);
	}

	for(i = 0; i < iterator; i++) {
		snprintf(buf,sizeof(buf)-1,"%i",i);
		z_set(c, buf, "world");
	}

	return 0;
}

int z_sadd_incremental(void *cptr, char *arg) {
	redisClient *c = (redisClient *)cptr;
	int i, iterator_x = 0, iterator_y = 1000;
	char *ptr_name = NULL, *ptr_x = NULL, *ptr_y = NULL;
	static char buf[10];

	printf("z_sadd_incremental: %s\n", arg);

	if(arg) {

		ptr_name = strtok(arg, ",");
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
		snprintf(buf,sizeof(buf)-1,"%i",i);
		z_sadd(c, ptr_name, buf);
	}

	return 0;
}

int z_sunion(void *cptr, char *arg) {
	redisClient *c = (redisClient *)cptr;
	robj * argv[3], *sunion, *set1, *set2;
	char *ptr_set1, *ptr_set2;

	printf("z_sunion: %s\n", arg);

	if(!arg) return -1;

	ptr_set1 = strtok(arg, ",");
	if(!ptr_set1) return -1;

	ptr_set2 = strtok(NULL, "");
	if(!ptr_set2) return -1;

	sunion = createStringObject("SUNION", 6);
	set1 = createStringObject(ptr_set1, strlen(ptr_set1));
	set2 = createStringObject(ptr_set2, strlen(ptr_set2));

	argv[0] = sunion;
	argv[1] = set1;
	argv[2] = set2;

	c->argv = argv;
	c->argc = 3;

	sunionCommand(c);

	c->argv = NULL;
	c->argc = 0;

	freeStringObject(sunion);
	freeStringObject(set1);
	freeStringObject(set2);

	return 0;
}


int z_keys_star(void *cptr, char *arg) {
	redisClient *c = (redisClient *)cptr;
	robj *argv[2], *keys, *star;

	printf("z_keys_star: %s\n", arg);

	keys = createStringObject("KEYS", 4);
	star = createStringObject("*", 1);

	argv[0] = keys;
	argv[1] = star;

	c->argv = argv;
	c->argc = 2;

	keysCommand(c);

	freeStringObject(keys);
	freeStringObject(star);

	c->argv = NULL;
	c->argc = 0;

	return 0;
}

int z_incr_key(void *cptr, char *arg) {
	redisClient *c = (redisClient *)cptr;
	robj *argv[2], *cmd, *key;
	int i, increment_max = 1000;

	printf("z_incr_key: %s\n", arg);

	z_set(c, "incr", "1");

	cmd = createStringObject("INCR", 4);
	key = createStringObject("incr", 4);

	argv[0] = cmd;
	argv[1] = key;
	
	c->argv = argv;
	c->argc = 2;

	if(arg) {
		increment_max = atoi(arg);
	}

	for(i = 0; i < increment_max; i++) {
		incrDecrCommand(c, 1);
	}

	freeStringObject(cmd);
	freeStringObject(key);

	c->argv = NULL;
	c->argc = 0;

	return 0;
}


z_ops ops[7] = {
	{ "FLUSHALL", z_flushall },
	{ "SET [0:ARG] 1", z_set_incremental },
	{ "KEYS *", z_keys_star },
	{ "INCR incr ::: [x] amount", z_incr_key },
	{ "SADD <key> <val>", z_sadd_incremental },
	{ "SUNION <key> <val>", z_sunion },
	{ NULL, NULL },
};


int main(int argc, char **argv) {
	redisClient * c = NULL;
	int i, j, iterator_max;

	z_usage(argc, argv);

	xmain(argc, argv);
	xmain_noOptions(argc, argv);
	c = createClient(0);
	if(!c) errx(-1, "main:createClient:Failed");

	iterator_max = atoi(argv[1]);
	for(i = 0; i < iterator_max; i++) {
		
		for(j = 2; j < argc; j++) {
			z_apply_op(c, i, j, ops, (sizeof(ops)/sizeof(z_ops))-1, argv[j]);
		}
	}

	saveCommand(c);
}
