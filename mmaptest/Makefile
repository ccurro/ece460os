CC = gcc
CFLAGS = -g

all: a b c d e f

a: a.c common.o
	${CC} ${CFLAGS} a.c common.o -o a.out

b: b.c common.o
	${CC} ${CFLAGS} b.c common.o -o b.out

c: c.c common.o
	${CC} ${CFLAGS} c.c common.o -o c.out

d: d.c common.o
	${CC} ${CFLAGS} d.c common.o -o d.out

e: e.c common.o
	${CC} ${CFLAGS} e.c common.o -o e.out

f: f.c common.o
	${CC} ${CFLAGS} f.c common.o -o f.out

common.o: common.c common.h
	${CC} ${CFLAGS} -c common.c