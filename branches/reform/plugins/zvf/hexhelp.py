#HexHelp Module by spinout
#f = file
#o = offset to read from
#if s == 0:
#    return hex as string
#if s == 1:
#    return int
#if p == 0:
#    return byte identical
#if p == 1:
#    return negative if applicable
#GetBbyte(f,o,s)GetHwordN(f,o,p)GetHword(f,o,s)GetOff(f,o,s)GetWord(f,o,s)
def GetByte(f,o,s): #returns byte
    f.seek(o)
    b = f.read(1)
    b = hex(ord(b))
    if s == 0:
        return b
    if s == 1:
        return int(b,16)
def GetHwordN(f,o,p): #returns halfword (as int), negative or positive(Depends on p)
    f.seek(o)
    b1 = f.read(1)
    b1 = hex(ord(b1))
    if len(b1[2:]) == 1:
        b1 = "0x0" + b1[2:] 
    f.seek(o+1)
    b2 = f.read(1)
    b2 = hex(ord(b2))
    if len(b2[2:]) == 1:
        b2 = "0x0" + b2[2:]
    b = b1+b2[2:]
    if int(b,16) >= 0x8000 and p == 1:
        b = 0-(0x10000-int(b,16))
        return b
    else:
        return int(b,16)
def GetHword(f,o,s): #returns halfword
    f.seek(o)
    b1 = f.read(1)
    b1 = hex(ord(b1))
    if len(b1[2:]) == 1:
        b1 = "0x0" + b1[2:] 
    f.seek(o+1)
    b2 = f.read(1)
    b2 = hex(ord(b2))
    if len(b2[2:]) == 1:
        b2 = "0x0" + b2[2:]
    b = b1+b2[2:]
    if s == 0:
        return b
    if s == 1:
        return int(b,16)
def GetOff(f,o,s): #returns offset
    f.seek(o)
    off1 = f.read(1)
    off1 = hex(ord(off1))
    if len(off1[2:]) == 1:
        off1 = "0x0" + off1[2:] 
    f.seek(o+0x1)
    off2 = f.read(1)
    off2 = hex(ord(off2))
    if len(off2[2:]) == 1:
        off2 = "0x0" + off2[2:] 
    f.seek(o+0x2)
    off3 = f.read(1)
    off3 = hex(ord(off3))
    if len(off3[2:]) == 1:
        off3 = "0x0" + off3[2:] 
    off = off1+off2[2:]+off3[2:]
    if s == 0:
        return off
    if s == 1:
        return int(off,16)
def GetWord(f,o,s): #returns word
    f.seek(o)
    w1 = f.read(1)
    w1 = hex(ord(w1))
    f.seek(o+1)
    w2 = f.read(1)
    w2 = hex(ord(w2))
    if len(w2[2:]) == 1:
        w2 = '0x0'+w2[2]
    f.seek(o+2)
    w3 = f.read(1)
    w3 = hex(ord(w3))
    if len(w3[2:]) == 1:
        w3 = '0x0'+w3[2]
    f.seek(o+3)
    w4 = f.read(1)
    w4 = hex(ord(w4))
    if len(w4[2:]) == 1:
        w4 = '0x0'+w4[2]
    w = w1+w2[2:]+w3[2:]+w4[2:]
    if s == 0:
        return w
    if s == 1:
        return int(w,16)
