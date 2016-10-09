This is an Apple II port of **Conway's Game of Life**.

It should build with no modification on Linux, macOs and Windows 10 (using the bash).

Two binaries will be produced.
1. *gol.out* is a Linux executable.
2. *gol.a2* is an Apple II executable

**Prerequisite in order to build:**
* The [cc65 compiler suite](https://github.com/cc65/cc65), with the environment variable *CC65_HOME* set to its folder

**How to make a disc image**
In order to embed the Apple II executable into a disc image, you can use [*AppleCommander*](http://applecommander.sourceforge.net/).

>java -jar [APPLECOMMANDER_PATH] -cc65 disc.dsk GOL BIN < gol.a2

For more information, you can refer to these posts on [my blog](https://www.xtof/info/blog/).
* [Coding in C for a 8 bit 6502 CPU](https://www.xtof.info/blog/?p=714)
* [Coding in Assembly for an Apple II](https://www.xtof.info/blog/?p=745)
