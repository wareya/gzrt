from visual import*
import hexhelp
import sys
s = open(sys.argv[1],'rb')
f = open(sys.argv[1],'rb')
#cfini = open('collision_fixer.ini', 'w')
scene.background = (.28,.28,.95)
scene.width=800
scene.height=600
scene.title = 'Zelda Veiwer'
sa = 0
hs = 0
meshcount = 0
ccvcount = 0
mesh = [meshcount]
cxverts = [ccvcount]
cyverts = [ccvcount]
czverts = [ccvcount]
coffs = [ccvcount]
save = 0
address = 0
dlist = 0
mseek = 0
cseek = 0
ctcount = 0
collision = [ctcount]
wcollision = [ctcount]
currvtx = 1
scentx = 0
scenty = 0
scentz = 0
address = 0
edit = 1
print 'test'
if sys.argv[1][-4:] == 'zmap' or 'zobj':
    print 'test'
    #try:
    while 1:
        print 'test'
        byte0 = hexhelp.GetByte(f,address+0,0)
        byte1 = hexhelp.GetByte(f,address+1,0)
        byte2 = hexhelp.GetByte(f,address+2,0)
        byte3 = hexhelp.GetByte(f,address+3,0)
        byte4 = hexhelp.GetByte(f,address+4,0)
        byte5 = hexhelp.GetByte(f,address+5,0)
        byte6 = hexhelp.GetByte(f,address+6,0)
        byte7 = hexhelp.GetByte(f,address+7,0)
        byte8 = hexhelp.GetByte(f,address+8,0)
        byte9 = hexhelp.GetByte(f,address+9,0)
        byte10 = hexhelp.GetByte(f,address+10,0)
        byte11 = hexhelp.GetByte(f,address+11,0)
        byte12 = hexhelp.GetByte(f,address+12,0)
        byte13 = hexhelp.GetByte(f,address+13,0)
        byte14 = hexhelp.GetByte(f,address+14,0)
        byte15 = hexhelp.GetByte(f,address+15,0)
        byte16 = hexhelp.GetByte(f,address+16,0)
        byte17 = hexhelp.GetByte(f,address+17,0)
        byte18 = hexhelp.GetByte(f,address+18,0)
        byte19 = hexhelp.GetByte(f,address+19,0)
        byte20 = hexhelp.GetByte(f,address+20,0)
        byte21 = hexhelp.GetByte(f,address+21,0)
        byte22 = hexhelp.GetByte(f,address+22,0)
        byte23 = hexhelp.GetByte(f,address+23,0)
        offset = hexhelp.GetOff(f,address+5,1)
        offset2 = hexhelp.GetOff(f,address+9,1)
        address2 = str(hex(address))
        addrlstchr = len(address2)-1
        print hex(f.tell())
        if dlist == 1:
            if byte0 == '0x1':
                noverts = str(byte1)+str(byte2)[2:]
                #zobjdata.write('01 command at offset 0x%x (%s verts, bank %s, loads from 0x%x)\n' % (address, noverts, byte4, offset))
                #print '01 command at offset 0x%x (%s verts, bank %s, loads from 0x%x)' % (address, noverts, byte4, offset)
                fverts = 1
                ccaddr = address+8
                while fverts == 1:
                    currcommand = hexhelp.GetByte(f, ccaddr,0)
                    if currcommand == '0x6':
                        t1p1 = hexhelp.GetByte(f,ccaddr+1,1)
                        t1p2 = hexhelp.GetByte(f,ccaddr+2,1)
                        t1p3 = hexhelp.GetByte(f,ccaddr+3,1)
                        t2p4 = hexhelp.GetByte(f,ccaddr+5,1)
                        t2p5 = hexhelp.GetByte(f,ccaddr+6,1)
                        t2p6 = hexhelp.GetByte(f,ccaddr+7,1)
                        t1p1x = hexhelp.GetHwordN(f,offset+t1p1*0x8+0,1)
                        t1p1y = hexhelp.GetHwordN(f,offset+t1p1*0x8+2,1)
                        t1p1z = hexhelp.GetHwordN(f,offset+t1p1*0x8+4,1)
                        t1p2x = hexhelp.GetHwordN(f,offset+t1p2*0x8+0,1)
                        t1p2y = hexhelp.GetHwordN(f,offset+t1p2*0x8+2,1)
                        t1p2z = hexhelp.GetHwordN(f,offset+t1p2*0x8+4,1)
                        t1p3x = hexhelp.GetHwordN(f,offset+t1p3*0x8+0,1)
                        t1p3y = hexhelp.GetHwordN(f,offset+t1p3*0x8+2,1)
                        t1p3z = hexhelp.GetHwordN(f,offset+t1p3*0x8+4,1)
                        t2p4x = hexhelp.GetHwordN(f,offset+t2p4*0x8+0,1)
                        t2p4y = hexhelp.GetHwordN(f,offset+t2p4*0x8+2,1)
                        t2p4z = hexhelp.GetHwordN(f,offset+t2p4*0x8+4,1)
                        t2p5x = hexhelp.GetHwordN(f,offset+t2p5*0x8+0,1)
                        t2p5y = hexhelp.GetHwordN(f,offset+t2p5*0x8+2,1)
                        t2p5z = hexhelp.GetHwordN(f,offset+t2p5*0x8+4,1)
                        t2p6x = hexhelp.GetHwordN(f,offset+t2p6*0x8+0,1)
                        t2p6y = hexhelp.GetHwordN(f,offset+t2p6*0x8+2,1)
                        t2p6z = hexhelp.GetHwordN(f,offset+t2p6*0x8+4,1)
                        t1p1u = hexhelp.GetHwordN(f,offset+t1p1*0x8+8,1)
                        t1p1v = hexhelp.GetHwordN(f,offset+t1p1*0x8+10,1)
                        t1p2u = hexhelp.GetHwordN(f,offset+t1p2*0x8+8,1)
                        t1p2v = hexhelp.GetHwordN(f,offset+t1p2*0x8+10,1)
                        t1p3u = hexhelp.GetHwordN(f,offset+t1p3*0x8+8,1)
                        t1p3v = hexhelp.GetHwordN(f,offset+t1p3*0x8+10,1)
                        t2p4u = hexhelp.GetHwordN(f,offset+t2p4*0x8+8,1)
                        t2p4v = hexhelp.GetHwordN(f,offset+t2p4*0x8+10,1)
                        t2p5u = hexhelp.GetHwordN(f,offset+t2p5*0x8+8,1)
                        t2p5v = hexhelp.GetHwordN(f,offset+t2p5*0x8+10,1)
                        t2p6u = hexhelp.GetHwordN(f,offset+t2p6*0x8+8,1)
                        t2p6v = hexhelp.GetHwordN(f,offset+t2p6*0x8+10,1)
                        t1p1r = hexhelp.GetByte(f,offset+t1p1*0x8+12,1)
                        t1p1g = hexhelp.GetByte(f,offset+t1p1*0x8+13,1)
                        t1p1b = hexhelp.GetByte(f,offset+t1p1*0x8+14,1)
                        t1p1a = hexhelp.GetByte(f,offset+t1p1*0x8+15,1)
                        t1p2r = hexhelp.GetByte(f,offset+t1p2*0x8+12,1)
                        t1p2g = hexhelp.GetByte(f,offset+t1p2*0x8+13,1)
                        t1p2b = hexhelp.GetByte(f,offset+t1p2*0x8+14,1)
                        t1p2a = hexhelp.GetByte(f,offset+t1p2*0x8+15,1)
                        t1p3r = hexhelp.GetByte(f,offset+t1p3*0x8+12,1)
                        t1p3g = hexhelp.GetByte(f,offset+t1p3*0x8+13,1)
                        t1p3b = hexhelp.GetByte(f,offset+t1p3*0x8+14,1)
                        t1p3a = hexhelp.GetByte(f,offset+t1p3*0x8+15,1)
                        t2p4r = hexhelp.GetByte(f,offset+t2p4*0x8+12,1)
                        t2p4g = hexhelp.GetByte(f,offset+t2p4*0x8+13,1)
                        t2p4b = hexhelp.GetByte(f,offset+t2p4*0x8+14,1)
                        t2p4a = hexhelp.GetByte(f,offset+t2p4*0x8+15,1)
                        t2p5r = hexhelp.GetByte(f,offset+t2p5*0x8+12,1)
                        t2p5g = hexhelp.GetByte(f,offset+t2p5*0x8+13,1)
                        t2p5b = hexhelp.GetByte(f,offset+t2p5*0x8+14,1)
                        t2p5a = hexhelp.GetByte(f,offset+t2p5*0x8+15,1)
                        t2p6r = hexhelp.GetByte(f,offset+t2p6*0x8+12,1)
                        t2p6g = hexhelp.GetByte(f,offset+t2p6*0x8+13,1)
                        t2p6b = hexhelp.GetByte(f,offset+t2p6*0x8+14,1)
                        t2p6a = hexhelp.GetByte(f,offset+t2p6*0x8+15,1)
                        t1p1p = t1p1x,t1p1y,t1p1z
                        t1p2p = t1p2x,t1p2y,t1p2z
                        t1p3p = t1p3x,t1p3y,t1p3z
                        tri1 = faces(pos=[(t1p1p),(t1p2p),(t1p3p),(t1p1p)])
                        tri1.color[0] = t1p1r/47.8125,t1p1g/47.8125,t1p1b/47.8125
                        tri1.color[1] = t1p2r/47.8125,t1p2g/47.8125,t1p2b/47.8125
                        tri1.color[2] = t1p3r/47.8125,t1p3g/47.8125,t1p3b/47.8125
                        mesh.append(tri1)
                        t2p4p = t2p4x,t2p4y,t2p4z
                        t2p5p = t2p5x,t2p5y,t2p5z
                        t2p6p = t2p6x,t2p6y,t2p6z
                        tri2 = faces(pos=[(t2p4p),(t2p5p),(t2p6p),(t2p4p)])
                        tri2.color[0] = t2p4r/47.8125,t2p4g/47.8125,t2p4b/47.8125
                        tri2.color[1] = t2p5r/47.8125,t2p5g/47.8125,t2p5b/47.8125
                        tri2.color[2] = t2p6r/47.8125,t2p6g/47.8125,t2p6b/47.8125
                        mesh.append(tri2)
                        #print '    06 command found at offset 0x%x.\n        Triangle 1:\n            (%i, %i, %i)    (%i, %i, %i)    (%i, %i, %i)\n            U: %s  V: %s         U: %s  V: %s         U: %s  V: %s \n            R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s\n        Triangle 2:\n            (%i, %i, %i)    (%i, %i, %i)    (%i, %i, %i)\n            U: %s  V: %s         U: %s  V: %s         U: %s  V: %s \n            R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s' % (ccaddr,t1p1x,t1p1y,t1p1z,t1p2x,t1p2y,t1p2z,t1p3x,t1p3y,t1p3z,t1p1u,t1p1v,t1p2u,t1p2v,t1p3u,t1p3v,t1p1r,t1p1g,t1p1b,t1p1a,t1p2r,t1p2g,t1p2b,t1p2a,t1p3r,t1p3g,t1p3b,t1p3a,t2p4x,t2p4y,t2p4z,t2p5x,t2p5y,t2p5z,t2p6x,t2p6y,t2p6z,t2p4u,t2p4v,t2p5u,t2p5v,t2p6u,t2p6v,t2p4r,t2p4g,t2p4b,t2p4a,t2p5r,t2p5g,t2p5b,t2p5a,t2p6r,t2p6g,t2p6b,t2p6a)
                        #zobjdata.write('    06 command found at offset 0x%x.\n        Triangle 1:\n            (%i, %i, %i)    (%i, %i, %i)    (%i, %i, %i)\n            U: %s  V: %s         U: %s  V: %s         U: %s  V: %s \n            R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s\n        Triangle 2:\n            (%i, %i, %i)    (%i, %i, %i)    (%i, %i, %i)\n            U: %s  V: %s         U: %s  V: %s         U: %s  V: %s \n            R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s\n' % (ccaddr,t1p1x,t1p1y,t1p1z,t1p2x,t1p2y,t1p2z,t1p3x,t1p3y,t1p3z,t1p1u,t1p1v,t1p2u,t1p2v,t1p3u,t1p3v,t1p1r,t1p1g,t1p1b,t1p1a,t1p2r,t1p2g,t1p2b,t1p2a,t1p3r,t1p3g,t1p3b,t1p3a,t2p4x,t2p4y,t2p4z,t2p5x,t2p5y,t2p5z,t2p6x,t2p6y,t2p6z,t2p4u,t2p4v,t2p5u,t2p5v,t2p6u,t2p6v,t2p4r,t2p4g,t2p4b,t2p4a,t2p5r,t2p5g,t2p5b,t2p5a,t2p6r,t2p6g,t2p6b,t2p6a))
                        ccaddr = ccaddr+8                     
                    elif currcommand == '0x5':
                        t1p1 = hexhelp.GetByte(f,ccaddr+1,1)
                        t1p2 = hexhelp.GetByte(f,ccaddr+2,1)
                        t1p3 = hexhelp.GetByte(f,ccaddr+3,1)
                        t1p1x = hexhelp.GetHwordN(f,offset+t1p1*0x8+0,1)
                        t1p1y = hexhelp.GetHwordN(f,offset+t1p1*0x8+2,1)
                        t1p1z = hexhelp.GetHwordN(f,offset+t1p1*0x8+4,1)
                        t1p2x = hexhelp.GetHwordN(f,offset+t1p2*0x8+0,1)
                        t1p2y = hexhelp.GetHwordN(f,offset+t1p2*0x8+2,1)
                        t1p2z = hexhelp.GetHwordN(f,offset+t1p2*0x8+4,1)
                        t1p3x = hexhelp.GetHwordN(f,offset+t1p3*0x8+0,1)
                        t1p3y = hexhelp.GetHwordN(f,offset+t1p3*0x8+2,1)
                        t1p3z = hexhelp.GetHwordN(f,offset+t1p3*0x8+4,1)
                        t1p1u = hexhelp.GetHwordN(f,offset+t1p1*0x8+8,1)
                        t1p1v = hexhelp.GetHwordN(f,offset+t1p1*0x8+10,1)
                        t1p2u = hexhelp.GetHwordN(f,offset+t1p2*0x8+8,1)
                        t1p2v = hexhelp.GetHwordN(f,offset+t1p2*0x8+10,1)
                        t1p3u = hexhelp.GetHwordN(f,offset+t1p3*0x8+8,1)
                        t1p3v = hexhelp.GetHwordN(f,offset+t1p3*0x8+10,1)
                        t1p1r = hexhelp.GetByte(f,offset+t1p1*0x8+12,1)
                        t1p1g = hexhelp.GetByte(f,offset+t1p1*0x8+13,1)
                        t1p1b = hexhelp.GetByte(f,offset+t1p1*0x8+14,1)
                        t1p1a = hexhelp.GetByte(f,offset+t1p1*0x8+15,1)
                        t1p2r = hexhelp.GetByte(f,offset+t1p2*0x8+12,1)
                        t1p2g = hexhelp.GetByte(f,offset+t1p2*0x8+13,1)
                        t1p2b = hexhelp.GetByte(f,offset+t1p2*0x8+14,1)
                        t1p2a = hexhelp.GetByte(f,offset+t1p2*0x8+15,1)
                        t1p3r = hexhelp.GetByte(f,offset+t1p3*0x8+12,1)
                        t1p3g = hexhelp.GetByte(f,offset+t1p3*0x8+13,1)
                        t1p3b = hexhelp.GetByte(f,offset+t1p3*0x8+14,1)
                        t1p3a = hexhelp.GetByte(f,offset+t1p3*0x8+15,1)
                        t1p1p = t1p1x,t1p1y,t1p1z
                        t1p2p = t1p2x,t1p2y,t1p2z
                        t1p3p = t1p3x,t1p3y,t1p3z
                        tri1 = faces(pos=[(t1p1p),(t1p2p),(t1p3p),(t1p1p)])
                        tri1.color[0] = t1p1r/47.8125,t1p1g/47.8125,t1p1b/47.8125
                        tri1.color[1] = t1p2r/47.8125,t1p2g/47.8125,t1p2b/47.8125
                        tri1.color[2] = t1p3r/47.8125,t1p3g/47.8125,t1p3b/47.8125
                        mesh.append(tri1)
                        #print '    05 command found at offset 0x%x. \n       Triangle1: \n            (%i, %i, %i)    (%i, %i, %i)    (%i, %i, %i)\n            U: %s  V: %s         U: %s  V: %s         U: %s  V: %s \n            R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s' % (ccaddr,t1p1x,t1p1y,t1p1z,t1p2x,t1p2y,t1p2z,t1p3x,t1p3y,t1p3z,t1p1u,t1p1v,t1p2u,t1p2v,t1p3u,t1p3v,t1p1r,t1p1g,t1p1b,t1p1a,t1p2r,t1p2g,t1p2b,t1p2a,t1p3r,t1p3g,t1p3b,t1p3a)
                        #zobjdata.write('    05 command found at offset 0x%x. \n       Triangle1: \n            (%i, %i, %i)    (%i, %i, %i)    (%i, %i, %i)\n            U: %s  V: %s         U: %s  V: %s         U: %s  V: %s \n            R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s  R:%s G:%s B:%s A:%s\n' % (ccaddr,t1p1x,t1p1y,t1p1z,t1p2x,t1p2y,t1p2z,t1p3x,t1p3y,t1p3z,t1p1u,t1p1v,t1p2u,t1p2v,t1p3u,t1p3v,t1p1r,t1p1g,t1p1b,t1p1a,t1p2r,t1p2g,t1p2b,t1p2a,t1p3r,t1p3g,t1p3b,t1p3a))
                        ccaddr = ccaddr+8
                    else:
                        fverts = 0
            #elif byte0 == '0xfd' and byte2 == '0x0' and byte3 == '0x0' and address2[addrlstchr] == '0' and int(byte5,16) < 0x08 or byte0 == '0xfd' and byte2 == '0x0' and byte3 == '0x0' and address2[addrlstchr] == '8' and int(byte5,16) < 0x08:
                #zobjdata.write('FD command at offset 0x%x (texture type %s, bank %s, offset 0x%x)\n' %(address, byte1, byte4, offset))
                #print 'FD command at offset 0x%x (texture type %s, bank %s, offset 0x%x)' %(address, byte1, byte4, offset)
            elif byte0 == '0xdf' and byte1 =='0x0' and byte2 == '0x0' and byte3 == '0x0' and byte4 == '0x0' and byte5 == '0x0' and byte6 == '0x0' and byte7 == '0x0':
                #zobjdata.write('DF command found at offset 0x%x\n' % address)
                #print 'DF command found at offset 0x%x' % address
                dlist = 0
                idk =2
                for i in range (900000):
                    i = i*i/5
                while idk == 2:
                    try:
                        mesh[cccount].visible = 0
                        cccount = cccount+1
                    except:
                       IndexError
                       idk = 1
                       cccount = 1
            #elif byte0 == '0xd9' and address2[addrlstchr] == '0' or byte0 == '0xd9' and address2[addrlstchr] == '8':
                #zobjdata.write('D9 command found at offset 0x%x(Red: %s Blue: %s Green: %s Alpha: %s)\n' % (address, byte4, byte5, byte6, byte7))
                #print 'D9 command found at offset 0x%x(Red: %s Blue: %s Green: %s Alpha: %s)' % (address, byte4, byte5, byte6, byte7)
            #elif byte0 == '0xd8' and address2[addrlstchr] == '0' or byte0 == '0xd9' and address2[addrlstchr] == '8':
                #zobjdata.write('D8 command found at offset 0x%x(Red: %s Blue: %s Green: %s Alpha: %s)\n' % (address, byte4, byte5, byte6, byte7))
                #print 'D8 command found at offset 0x%x(Red: %s Blue: %s Green: %s Alpha: %s)' % (address, byte4, byte5, byte6, byte7)
        #if byte0 == '0x0' and byte2 == '0x0' and byte3 == '0x0' and byte4 == '0x6' and byte8 == '0x6' and byte12 == '0x0' and byte14 == '0x0' and byte15 == '0x0' or byte0 == '0x0' and byte2 == '0x0' and byte3 == '0x0' and byte4 == '0x4' and byte8 == '0x6' and byte12 == '0x0' and byte14 == '0x0' and byte15 == '0x0':
            #zobjdata.write('Animation header at offset 0x%x (0x%x keyframes, rotation/translation data at offset 0x%x, rotation/translation data references at 0x%x)\n' % (address, int(byte1,16), offset, offset2))
            #print 'Animation header at offset 0x%x (0x%x keyframes, rotation/translation data at offset 0x%x, rotation/translation data references at 0x%x)' % (address, int(byte1,16), offset, offset2)
        if byte0 == '0xe7' and byte1 == '0x0' and byte2 == '0x0' and byte3 == '0x0' and byte4 == '0x0' and byte5 == '0x0' and byte6 == '0x0' and byte7 == '0x0':
            #zobjdata.write('E7 command found at 0x%x\n' % address)
            #print 'E7 command found at 0x%x' % address
            dlist = 1
        if dlist == 0:
            address = address+0x4
        elif dlist == 1:
            address = address+0x8
    #except:
      #  TypeError
        #f.close()
        #zobjdata.close()
        #raw_input("Render all display lists...")
      #  cccount = 1
        #idk =2
       # while idk == 2:
       #     try:
       #         mesh[cccount].visible = 1
       #         cccount = cccount+1
       #     except:
        #        IndexError
        #        idk = 1
        #        cccount = 1  
    while 1:
        if scene.kb.getkey() == 'w':
            scene.center = scentx, scenty, scentz
            print scene.forward
            scentz= scentz-20
        elif scene.kb.getkey() == 's':
            scene.center = scentx, scenty, scentz+20
            scentz= scentz+20
        elif scene.kb.getkey() == 'a':
            scene.center = scentx-20, scenty, scentz 
            scentx= scentx-20
        elif scene.kb.getkey() == 'd':
            scene.center =scentx+20, scenty, scentz
            scentx= scentx+20
        elif scene.kb.getkey() == 'r':
            scene.center = scentx, scenty+20, scentz 
            scenty= scenty+20
        elif scene.kb.getkey() == 'f':
            scene.center =scentx, scenty-20, scentz
            scenty= scenty-20
        elif scene.kb.getkey() == 'q':
            scentx = 0
            scenty = 0
            secntz = 0
            scene.center =scentx, scenty, scentz
            scene.forward = 0,0,-1
elif sys.argv[1][-5:] == 'zscene':
    while hs == 0:
        sb = hexhelp.GetByte(s,sa,1)
        if sb == 0x3:
            #print '<spinout> ohneat'
            mdho = hexhelp.GetOff(s,sa+5,1)
            hs = 1
        elif sb == 0x14:
            #raw_input("<spinout> LoL no mesh data header\n")
            s.close()
            hs,save = 9000
        sa = sa+8
    while hs == 1:
        #FD58FFF8 FB500C80 019003E8 01490000 #xxxxyyyy zzzzx2x2 y2y2z2z2 vvvv0000
        #02004200 01A50000 02004AA0 02000244 #02oooooo tttt0000 02oooooo
        #02000204
        maxx = hexhelp.GetHwordN(s,mdho,1)
        maxy = hexhelp.GetHwordN(s,mdho+2,1)
        maxz = hexhelp.GetHwordN(s,mdho+4,1)
        minx = hexhelp.GetHwordN(s,mdho+6,1)
        miny = hexhelp.GetHwordN(s,mdho+8,1)
        minz = hexhelp.GetHwordN(s,mdho+10,1)
        ncverts = hexhelp.GetHword(s,mdho+12,1)
        cvoff = hexhelp.GetOff(s,mdho+17,1)
        nctris =  hexhelp.GetHword(s,mdho+20,1)
        ctoff = hexhelp.GetOff(s,mdho+25,1)
        #print maxx, maxy, maxz, minx, miny, minz, hex(ncverts), hex(cvoff), hex(nctris), hex(ctoff)
        #cfini.write("'6-byte\n%s\n%s\n'16-byte\n%s\n%s" % ('0'*(9-(len(hex(cvoff))))+hex(cvoff)[2:],
        #                                                   '0'*(9-(len(hex(cvoff+6*ncverts))))+hex(cvoff+6*ncverts)[2:],
        ##                                                   '0'*(9-(len(hex(ctoff))))+hex(ctoff)[2:],
          #                                                 '0'*(9-(len(hex(ctoff+0x10*nctris))))+hex(ctoff+0x10*nctris)[2:]))
        while ccvcount != ncverts:
            cxverts.append(hexhelp.GetHwordN(s,cvoff+(6*ccvcount),1))
            cyverts.append(hexhelp.GetHwordN(s,cvoff+(6*ccvcount+2),1))
            czverts.append(hexhelp.GetHwordN(s,cvoff+(6*ccvcount+4),1))
            coffs.append(cvoff+(6*ccvcount))
            ccvcount = ccvcount+1
        hs = 2
    while hs == 2:
        while ctcount != nctris:
            p1 = hexhelp.GetHword(s,ctoff+ctcount*0x10+2,0)[3:]
            p2 = hexhelp.GetHword(s,ctoff+ctcount*0x10+4,0)[3:]
            p3 = hexhelp.GetHword(s,ctoff+ctcount*0x10+6,0)[3:]
            p1x = cxverts[int(p1,16)+1]
            p1y = cyverts[int(p1,16)+1]
            p1z = czverts[int(p1,16)+1]
            p2x = cxverts[int(p2,16)+1]
            p2y = cyverts[int(p2,16)+1]
            p2z = czverts[int(p2,16)+1]
            p3x = cxverts[int(p3,16)+1]
            p3y = cyverts[int(p3,16)+1]
            p3z = czverts[int(p3,16)+1]
            cmtri = faces(pos=[(p1x,p1y,p1z),(p2x,p2y,p2z),(p3x,p3y,p3z), (p1x,p1y,p1z)], color = (0,0.8,0))
            #cwtri = curve(pos=[(p1x,p1y,p1z),(p2x,p2y,p2z),(p3x,p3y,p3z), (p1x,p1y,p1z)], color = (.5,.5,.5), radius = 1)
            collision.append(cmtri)
            #wcollision.append(cwtri)
            ctcount = ctcount+1
        hs = 3
        edit = 1
    while 1:
        if scene.kb.getkey() == 'w':
            scene.center = scentx, scenty, scentz
            print scene.forward
            scentz= scentz-20
        elif scene.kb.getkey() == 's':
            scene.center = scentx, scenty, scentz+20
            scentz= scentz+20
        elif scene.kb.getkey() == 'a':
            scene.center = scentx-20, scenty, scentz 
            scentx= scentx-20
        elif scene.kb.getkey() == 'd':
            scene.center =scentx+20, scenty, scentz
            scentx= scentx+20
        elif scene.kb.getkey() == 'r':
            scene.center = scentx, scenty+20, scentz 
            scenty= scenty+20
        elif scene.kb.getkey() == 'f':
            scene.center =scentx, scenty-20, scentz
            scenty= scenty-20
        elif scene.kb.getkey() == 'q':
            scentx = 0
            scenty = 0
            secntz = 0
            scene.center =scentx, scenty, scentz
            scene.forward = 0,0,-1





 














