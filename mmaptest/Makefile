CC = gcc
CFLAGS = -g

all: a

a: a.c common.o
	${CC} ${CFLAGS} a.c common.o -o a.out

# b: b.c common.o
	# ${CC} ${CFLAGS} b.c common.o -o b.out

common.o: common.c common.h
	${CC} ${CFLAGS} -c common.c