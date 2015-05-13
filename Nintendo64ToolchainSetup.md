# Introduction #
This tutorial will cover the essentials for utilizing the N64 Toolchain (aka, GNU toolchain targeting the MIPS [R4300](https://code.google.com/p/gzrt/source/detail?r=4300)) provided on this project page. It's for Windows users, obviously - not because I use Windows myself, but because everyone else does!

# What you'll need #

  * Properly configured MinGW + MSYS setup
  * The toolchain ([download here](http://gzrt.googlecode.com/files/n64tree-win32-v1.0.7z))
  * If you want to follow the turorial, the current hacks ([download here](http://gzrt.googlecode.com/files/n64hacks-rev1.7z))


# Tutorial #

First of all, download the toolchain package, and extract it to C:\. Next, download the current hacks, and extract the folders within the hacks-rev1 folder to C:\n64tree-win32.

![http://64.vg/files/n64tree-tutorial/02.png](http://64.vg/files/n64tree-tutorial/02.png)

Now, just to clear up some executable location issues:
```
cd /c/n64tree-win32/root/bin
export PATH=$PATH:`pwd`
```
![http://64.vg/files/n64tree-tutorial/03.png](http://64.vg/files/n64tree-tutorial/03.png)

That should be everything. Time to build an example - the Zelda 64 Actor Spawner:
```
cd ../../z64actor
```
![http://64.vg/files/n64tree-tutorial/04.png](http://64.vg/files/n64tree-tutorial/04.png)

All you have to do to build this hack is to run the command 'make':

![http://64.vg/files/n64tree-tutorial/05.png](http://64.vg/files/n64tree-tutorial/05.png)

![http://64.vg/files/n64tree-tutorial/06.png](http://64.vg/files/n64tree-tutorial/06.png)

If the output you receive is the same as the one above, you're good. The code is now ready to use. Open the 'final.txt' in Notepad - it is a Nemu-compatible representation of the final code (and the hook):

![http://64.vg/files/n64tree-tutorial/07.png](http://64.vg/files/n64tree-tutorial/07.png)

![http://64.vg/files/n64tree-tutorial/08.png](http://64.vg/files/n64tree-tutorial/08.png)

As you can see above, the default cheat number for the code is '1' - you can change that easily in the Makefile, though (the part that involves the 'nemucheat' command).

## Running the example code ##

After getting the code into the Cheats.ini of Nemu - whichever way you choose to do it - boot up Ocarina of Time v1.0 US and get to anywhere in the game where you have control of Link. Then, activate Link and set a PC breakpoint on 0x803FF000. The reason we're doing this is just so that Nemu's dynarec can recognize the code change.

After the breakpoint is triggered, just deactivate it and continue gameplay. The code should now be in effect. The controls can be found in z64actor/z64as.c