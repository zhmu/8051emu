TARGET:		sim8051

OBJS =		main.o p8051.o memory.o d8051.o
CFLAGS +=	-ansi -pedantic -Wall
CFLAGS +=	-g
CC =		c++
LIBS =

sim8051:	$(OBJS)
		$(CC) -o sim8051 $(OBJS) $(LIBS)

.cc.o:
		$(CC) -c $(CFLAGS) -o $@ $<

clean:
		rm -f sim8051 $(OBJS)
