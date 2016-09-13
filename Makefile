CC	:= cc
SRC	:= gol.c
CFLAGS	:= -Wall -Wextra -pedantic-errors -ansi -std=gnu99 -lncurses -lm
DFLAGS	:= -g -pg -O0
OUT	:= gol.out

all:	std

std:	$(SRC)
	$(CC) -O3 -o $(OUT) $? $(CFLAGS) $(DFLAGS)

clean:	$(SRC)
	rm -f $(OUT) gmon.out

debug:	$(SRC)
	$(CC) $(CFLAGS) $(DFLAGS) -o $(OUT) $?

install: std
	install $(OUT) $(INSTALL)
	strip $(INSTALL)
