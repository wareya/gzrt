declare sub lbyte()
declare sub l2byte()
declare sub l16byte()
declare sub load()

extern tbyte as ubyte
extern address as ulongint
extern taddress as ulongint

extern filesize as ulongint
extern filename as string

extern file() as ubyte


sub load

filename = Command(1)

open filename for Binary as #1

sleep 1
filesize = Lof ( 1 )
redim file(1 to filesize)

sleep 1
print "Loading..."
print
print

address = 1
while ( address < filesize )
    
    taddress = address
    l16byte()
    address += 16
    
wend    
print
print "Finished loading!"
print ""
print "Zev version 2.0a"
close #1

end sub




sub lbyte()
    get #1, taddress, file(taddress)
    if (file(taddress) <= 15) then print "0"; end if
    print hex(file(taddress));
end sub




sub l2byte()
    lbyte()
    taddress += 1
    
    lbyte()
    print " ";
    taddress += 1
end sub




sub l16byte()
    l2byte()
    l2byte()
    l2byte()
    l2byte()
    l2byte()
    l2byte()
    l2byte()
    l2byte()
    print
end sub