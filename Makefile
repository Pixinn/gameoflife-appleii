# LINUX
LINUX_CC	    := cc
LINUX_CFLAGS	:= -Wall -Wextra -pedantic-errors -ansi -std=gnu99 -lncurses -lm
LINUX_DFLAGS	:= -g -pg -O0
LINUX_OUT	    := gol

# APPLE II


# COMMON
INCLUDES = ./
LFLAGS   = ./
LIBS     = -lptf
SRC	     = gol.c

all:	linux_release

linux_release:	$(SRC)
	$(LINUX_CC) -O3 -o $(LINUX_OUT) $? $(LINUX_CFLAGS)

linux_debug:	$(SRC)
	$(LINUX_CC) $(LINUX_CFLAGS) $(LINUX_DFLAGS) -o $(LINUX_OUT) $?    
    
clean:	$(SRC)
	rm -f $(LINUX_OUT) gmon.out


