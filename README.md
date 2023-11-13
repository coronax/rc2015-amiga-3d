# Retrochallenge 2015 Amiga 3D Graphics Project

This project is a small 3D polygon graphics demo for the Commodore Amiga. It was originally written for the 2015 [Retrochallenge](https://retrochallenge.net) event. 

Read about the development process at [Coronax's Project Blog](https://coronax.wordpress.com/projects/retrochallenge-2015-07/).

![image](https://github.com/coronax/rc2015-amiga-3d/assets/12867087/9efca50f-2c05-4fc5-98d3-d88de58a69b8)

This program is almost entirely written in C using AmigaOS graphics.library functions, and is reasonably multitasking-friendly. That also means that it's quite a bit slower than what you may be used to seeing from the Amiga demoscene. It does demonstrate a few interesting features:

- Fixed-point math implemented with C macros. My target machine was the Amiga 500, which has no hardware floating-point math. This way, all 3D transformations are performed using integer instructions.
- Backface culling is, of course, performed in software.
- Bitplane and palette tricks create the appearance of transparency.
- Polygon fill is done by the Amiga's blitter. As mentioned, I'm using graphics.library to set up the draw instead of interacting with the blitter directly. 
- A custom copper list scrolls the background, and expands the palette options.

There's a lot of room for optimization: rewrite the math in assembly, bypass the OS and libraries, draw some better art, and so on.

## Building

I've tried building the code with two different compilters. 

On native Amiga hardware, it compiles with Manx Axtec C 5.2a. You'll need to download a version of GNU Make from Aminet in order to use the included Makefile; I used [this one](http://aminet.net/package/dev/c/make-3.75-bin).

I've also compiled it under a WSL Debian installation, using [Bebbo's GCC cross-compiler build](https://github.com/bebbo/amiga-gcc).

While the GCC version is quite a bit larger, the two executables run at almost exactly the same speed.
