# LINUX
LINUX_CC	    := cc
LINUX_CFLAGS	:= -Wall -Wextra -pedantic-errors -ansi -std=gnu99 -lncurses -lm
LINUX_DFLAGS	:= -g -pg -O0
LINUX_OUT	    := gol
LINUX_SRC       := gol_linux.c

# APPLE II
APPLE_CC	    := $(CC65_HOME)/bin/cl65
APPLE_CFLAGS	:= -t apple2 -Oirs -v
APPLE_OUT	    := gol.a2
APPLE_SRC       := gol_apple.c


all:	linux apple

linux:	$(LINUX_SRC)
	$(LINUX_CC) -O3 -o $(LINUX_OUT) $? $(LINUX_CFLAGS)

apple: $(APPLE_SRC)
	$(APPLE_CC) $(APPLE_CFLAGS) $? -o $(APPLE_OUT)
    
    
    
clean:	$(SRC)
	rm -f $(LINUX_OUT) $(APPLE_OUT) gmon.out


