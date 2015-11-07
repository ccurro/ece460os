CC = gcc
CFLAGS = -g

all: tasTest acidTest

tasTest: common.o tasTest.c tas64.S
	${CC} ${CFLAGS} -o ../tasTest tasTest.c common.o tas64.S

sem.o: common.o sem.c sem.h tas64.S
	${CC} ${CFLAGS} -c sem.c

fifo.o: common.o fifo.h fifo.c
	${CC} ${CFLAGS} -c fifo.c

acidTest: fifo.o common.o sem.o fifoAcidTest.c
	${CC} ${CFLAGS} -o ../acid fifoAcidTest.c fifo.o common.o sem.o tas64.S

common.o: common.c common.h
	${CC} ${CFLAGS} -c common.c

clean: 
	rm *.o
