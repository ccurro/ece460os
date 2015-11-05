CC = gcc
CFLAGS = -g

all: a b c d e f

tas: common.o tasTest.c tas64.S
	${CC} ${CFLAGS} -o ../tasTest tasTest.c common.o tas64.S

common.o: common.c common.h
	${CC} ${CFLAGS} -c common.c