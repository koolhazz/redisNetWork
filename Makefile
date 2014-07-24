TARGET = libredis.a 
BIN = redis_net

CFLAGS= -g -O2 -Wall 

OBJECT = ae.o ae_epoll.o anet.o 

TARGET:${OBJECT}
	#gcc ${OBJECT} -o ${TARGET}
	ar -rcu	${TARGET} ${OBJECT}
	gcc main.c -o ${BIN} ${CFLAGS} ${TARGET}
ae.o:ae.c ae.h
ae_epoll.o:ae_epoll.c
anet.o:anet.c anet.h

clean:
	rm -rf *.o ${TARGET}
