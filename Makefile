APPLE2_CL	:= $(CC65_HOME)/bin/cl65
APPLE2_CC	:= $(CC65_HOME)/bin/cc65
APPLE2_SRC	:= src/gol_main.c src/gol_optimized.asm src/gfx.asm src/rnd_colors.asm src/file_io.c src/mli.asm src/music.asm
APPLE2_MAP  := gol.map
APPLE2_CFLAGS	:= -Oirs -v -t apple2
APPLE2_OUT	:= bin/gol.a2

all:	directories apple2

directories:
	    mkdir -p bin

apple2: $(APPLE2_SRC)
		$(APPLE2_CL) -m $(APPLE2_MAP) -o $(APPLE2_OUT) $? $(APPLE2_CFLAGS) -C src/game-of-life.cfg

apple2-asm: $(APPLE2_SRC)
				$(APPLE2_CC) $(APPLE2_CFLAGS) -r -T $?

clean:	$(SRC)
	rm -f $(APPLE2_MAP) src/*.o src/*.s gmon.out & rm -r bin/
