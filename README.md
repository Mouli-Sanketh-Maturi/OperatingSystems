This project is an example implementation of building an Operating System from scratch.

**Things that work:**
 - Printing to a Screen.
 - Reading from Keyboard (even with Backspace).
 - Read a sector from disk.
 - Own Custom Interrupt.
 - printString, readString, and readSector interrupt calls.
 - Interactive shell.
 - Shell commands type (to output contents of a file like 'cat') and exec (to execute a program)

**Compiling**

```
./compileOS.sh
```

This will handle the compiling and assembling the OS into a disc image. Upon which you can use the diskc.img to run it in a simulator
