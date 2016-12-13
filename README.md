This is an Apple II port of **Conway's Game of Life**.

It should build with no modification on Linux, macOs and Windows 10 (using the bash).

---

Two binaries will be produced.

1. *gol.out* is a Linux executable.
2. *gol.a2* is an Apple II executable

**Prerequisite in order to build:**

* The [cc65 compiler suite](https://github.com/cc65/cc65), with the environment variable *CC65_HOME* set to its folder

**Prerequisite in order to produce the disk image**

* Java Runtime
* [*AppleCommander*](http://applecommander.sourceforge.net/)

**Embedding the Apple 2 binary into the disk image**

Run the *add-to-disk.sh* script.

**Title screen**

The title screen art is located in the *assets* folder. It has to be
manually loaded on the disk image using the *AppleCommander's GUI*, for the
command line loads a corrupted file!
This screen was converted from a PNG using [*Rgb2Hires*](https://github.com/Pixinn/Rgb2Hires).

---

For more information, you can refer to these posts on [my blog](https://www.xtof/info/blog/).
* [Coding in C for a 8 bit 6502 CPU](https://www.xtof.info/blog/?p=714)
* [Coding in Assembly for an Apple II](https://www.xtof.info/blog/?p=745)
