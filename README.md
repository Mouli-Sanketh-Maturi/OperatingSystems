#Authors - Mouli Sanketh Maturi & Miray Dalgin
This project is an example implementation of building an Operating System from scratch.

**Things that work:**
 - Printing to a Screen.
 - Reading from Keyboard (even with Backspace).
 - Read a sector from disk.
 - Own Custom Interrupt.
 - printString, readString, and readSector interrupt calls.
 - Interactive shell.
 - Shell commands
    - type (to output contents of a file like 'cat')
    - exec (to execute a program)
    - execb (to execute a program in background)
    - kill (to kill a process that's running)
    - create (to create and enter contents to a file like 'touch' or 'vim')
    - del (to delete a file)
    - copy (to copy a file)
    
**Compiling**

```
./compileOS.sh
```

This will handle the compiling and assembling the OS into a disc image. Upon which you can use the diskc.img to run it in a simulator
