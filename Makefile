LINUX_CC	:= cc
LINUX_SRC	:= gol_linux.c
LINUX_CFLAGS	:= -Wall -Wextra -pedantic-errors -ansi -std=gnu99 -lncurses -lm -O3
LINUX_OUT	:= gol.out

APPLE2_CL	:= $(CC65_HOME)/bin/cl65
APPLE2_CC	:= $(CC65_HOME)/bin/cc65
APPLE2_SRC	:= gol_apple2.c
APPLE2_MAP  := gol_apple2.map
APPLE2_CFLAGS	:= -Oirs -v -t apple2
APPLE2_OUT	:= gol.a2

all:	linux apple2

linux:	$(LINUX_SRC)
	$(LINUX_CC) -o $(LINUX_OUT) $? $(LINUX_CFLAGS)

apple2: $(APPLE2_SRC)
		$(APPLE2_CL) -m $(APPLE2_MAP) -o $(APPLE2_OUT) $? $(APPLE2_CFLAGS)

apple2-asm: $(APPLE2_SRC)
				$(APPLE2_CC) $(APPLE2_CFLAGS) -r -T $?

clean:	$(SRC)
	rm -f $(LINUX_OUT) $(APPLE2_OUT) $(APPLE2_MAP) *.o *.s gmon.out
