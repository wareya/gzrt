Hi guys. This project has moved! Go to http://vg64tools.googlecode.com/ for the latest version.

---

_GZRT_ is a nifty utility for manipulating N64-based Zelda games. This tool covers everything verifying/ensuring ROM integrity to extracting and modifying the filesystem within the game. You may also extract the files contained within the ROM. Filenames - if present - will be used for the output. It is written in C.

Feature list:
  * File extraction (with filetype detection): treat the ROM as if it were an archive and extract the files. From there on, the files can be modified/analyzed in another application.
  * ROM decompression: by default, the regular Zelda 64 ROMs' files are compressed with Yaz0 (not the debugger's build, however). For the sake of making a ROM more easily modifiable, this application can decompress the contents of it and stick them back together.
  * Flexible plugin system (standard plugins are: model viewer & MIPS R4K disassembler)
  * General N64 ROM tidiness: can fix CRCs or byteswap ROM if necessary

Screenshots:


Windows:

![http://64.vg/files/0123.png](http://64.vg/files/0123.png)
![![](http://64.vg/files/gzrt-cur-th.jpg)](http://64.vg/files/gzrt-cur.png)

Linux:

![![](http://64.vg/files/snap1224604082.th.jpg)](http://64.vg/files/snap1224604082.png)


---

Current application status: v0.2