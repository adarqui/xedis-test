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

int main(int argc, char **argv) {
	redisClient * c;
	robj *key, *val;

	xmain(argc,argv);

	c = createClient(0);
	printf("%p\n", c);

	/*
	void setGenericCommand(redisClient *c, int flags, robj *key, robj *val, robj *expire, int unit, robj *ok_reply, robj *abort_reply) {
		 
	*/

/*
	memset(&key,0,sizeof(key));
	memset(&val,0,sizeof(val));
*/

	key = createStringObject("hello", 5);
	val = createStringObject("world", 5);

	setGenericCommand(c, 0, key, val, NULL, 0, NULL, NULL);
	saveCommand(c);
}
