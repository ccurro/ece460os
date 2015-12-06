CC = gcc
CFLAGS = -g

all: test stackTest

test: common.o sched.o adjstack.o test.c
	${CC} ${CFLAGS} -o ../test test.c common.o sched.o adjstack.o savectx64.S

stackTest: common.o sched.o adjstack.o stackTest.c
	${CC} ${CFLAGS} -o ../stackTest stackTest.c common.o sched.o adjstack.o savectx64.S

sched.o: sched.c sched.h
	${CC} ${CFLAGS} -c sched.c

common.o: common.c common.h
	${CC} ${CFLAGS} -c common.c savectx64.S

adjstack.o: adjstack.c
	${CC} ${CFLAGS} -c adjstack.c