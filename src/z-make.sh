#!/bin/bash

#cp redis.o redis2.o
objcopy --redefine-sym main=xmain redis2.o

ld adlist.o ae.o anet.o dict.o sds.o zmalloc.o lzf_c.o lzf_d.o pqsort.o zipmap.o sha1.o ziplist.o release.o networking.o util.o object.o db.o replication.o rdb.o t_string.o t_list.o t_set.o t_zset.o t_hash.o config.o aof.o pubsub.o multi.o debug.o sort.o intset.o syncio.o cluster.o crc16.o endianconv.o slowlog.o scripting.o bio.o rio.o rand.o memtest.o crc64.o bitops.o sentinel.o notify.o setproctitle.o -shared redis2.o -o lib.so

#gcc z-test.c ./lib.so -o z-test

gcc z-test-xedis.c ./lib.so ../deps/lua/src/liblua.a ../deps/jemalloc/lib/libjemalloc.a -lpthread -lm ../deps/hiredis/libhiredis.a ./z-test.c -o z-test-xedis

gcc z-test.c z-test-hiredis.c -lhiredis -o z-test-hiredis
