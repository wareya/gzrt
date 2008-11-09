chdir exepath
open cons for output as #2

#include once "GL/gl.bi"
#include once "GL/glu.bi"
#include once "crt.bi"          '' scanf is used to parse data file
#include once "fbgfx.bi"        '' for Scan code constants
#include once "zev2.l.bas"


dim shared as ubyte abyte
dim shared as ubyte tbyte

dim shared as ubyte all
dim shared as string tstring

dim shared as uinteger len01
dim shared as ulongint spot01

dim shared as ubyte tri051
dim shared as ubyte tri052
dim shared as ubyte tri053

dim shared as ubyte tri061
dim shared as ubyte tri062
dim shared as ubyte tri063

dim shared as uinteger curt = 0
dim shared as uinteger curv = 0

redim shared as ubyte tris(0 to 1)

redim shared as short xverts(0 to 1)
redim shared as short yverts(0 to 1)
redim shared as short zverts(0 to 1)
redim shared as short rverts(0 to 1)
redim shared as short gverts(0 to 1)
redim shared as short bverts(0 to 1)
redim shared as short averts(0 to 1)

dim shared as ulongint address
dim shared as ulongint taddress
dim shared as ulongint filesize
dim shared as uinteger trialo = 0

dim shared as ubyte dlist = 0
dim shared as uinteger entry = 0
dim shared as uinteger curentry = 0
dim shared as ubyte bank06 = 0

dim shared as string filename
redim shared as ubyte file(1)


'' Setup our booleans
const false = 0
const true  = not false

const piover180 = 0.0174532925f
''------------------------------------------------------------------------------
'' Types used by the model
type VERTEX                      '' Build Our Vertex Structure called VERTEX
	x as short                  '' 3D Coordinates (x, y, z)
	y as short
	z as short
    r as ubyte
    g as ubyte
    b as ubyte
    a as ubyte
    u as single
    v as single
end type

type TRIANGLE                    '' Build Our Triangle Structure called TRIANGLE
	v(0 to 2) as byte          '' Array Of Three Vertices
end type

redim shared as TRIANGLE triangles(0 to 1)



''------------------------------------------------------------------------------
declare sub Rend()
declare sub Decode()

	dim shared heading as single
	dim shared xpos as single                 '' X position
	dim shared ypos as single                 '' Y position
	dim shared zpos as single                 '' Z position
	
	dim shared yrot as single
    dim shared lookupdown as single
	
	dim shared x_m as single
	dim shared y_m as single
	dim shared z_m as single
    
    dim shared as single camspeed = 6.0
    dim shared as byte pressedm = 0
    dim shared as byte pressedp = 0
    dim shared as byte pressedu = 0
    dim shared as byte pressedl = 0
    dim shared as byte pressedd = 0
    dim shared as byte pressedc = 0
    
    dim shared r_m as single
	dim shared g_m as single
	dim shared b_m as single
	dim shared a_m as single
	
	dim shared xtrans as single         '' Used For Player Translation
	dim shared ztrans as single         '' Used For Player Translation
	dim shared ytrans as single         '' Used For Bouncing Motion Up And Down
	dim shared sceneroty as single      '' 360 Degree Angle For Player Direction

	dim shared as integer numtriangles  '' Integer To Hold The Number Of Triangles
	dim shared as integer loop_m        '' Loop counter
    

    dim shared as byte start = 0
Start:
    start = 0
    Load()
	screen 18, 16, , 2.
	'' ReSizeGLScene
	glViewport 0, 0, 640, 480                      '' Reset The Current Viewport
	glMatrixMode GL_PROJECTION                     '' Select The Projection Matrix
	glLoadIdentity                                 '' Reset The Projection Matrixy
	gluPerspective 45.0f, 640.0/480.0, 1.0f, 999999'' Calculate The Aspect Ratio Of The Window
	glMatrixMode GL_MODELVIEW                      '' Select The Modelview Matrix
	glLoadIdentity                                 '' Reset The Modelview Matrix

	'' All Setup For OpenGL Goes Here
    glEnable GL_BLEND
    glBlendFunc GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
	glShadeModel GL_SMOOTH                         '' Enable Smooth Shading
	glClearColor 0.4, 0.6, 0.8, 0.5                       '' Black Background
	glClearDepth 1.0                               '' Depth Buffer Setup
	glEnable GL_DEPTH_TEST                         '' Enables Depth Testing
	glDepthFunc GL_LEQUAL                          '' The Type Of Depth Testing To Do
	glHint GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST    '' Really Nice Perspective Calculations


dim as integer per10 = 0
dim as double FrameTmr
dim as integer framesec = 0
do
    framesec += 1
    if (framesec = 60) then
        framesec = 0
    endif
    
    if inkey = chr(255) + "k" then
        end
    end if
    
    ''Wait fps                        !!@@Courtesy of Mysoft on Freebasic.net
    if abs(timer-FrameTmr) > 1/60 then
        FrameTmr = timer
    else
        while (timer-FrameTmr) < 1/60
            sleep 1
        wend
        Rend()
        if start = 1 then goto start
    end if 
loop
while inkey <> "": wend
end
sub Rend()
		glClear GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT      '' Clear Screen And Depth Buffer
		glLoadIdentity()                                        '' Reset The View
		
		xtrans = - xpos
		ztrans = - zpos
		ytrans = - ypos
		sceneroty = 360.0 - yrot
		
		glRotatef lookupdown, 1.0, 0,0
		glRotatef sceneroty, 0, 1.0, 0
		
		glTranslatef xtrans, ytrans, ztrans
		
        decode()
        
		'' Keyboard handlers
        if MULTIKEY(FB.SC_PLUS) then
            if (pressedu = 0) then
                camspeed += 2.5
                pressedu = 1
            end if
        else
            pressedu = 0
        end if
        
        if MULTIKEY(FB.SC_MINUS) then
            if (pressedl = 0) then
                camspeed -= 2.5
                pressedl = 1
            end if
        else
            pressedl = 0
        end if
        
        if MULTIKEY(FB.SC_MULTIPLY) then
            if (pressedm = 0) then
                camspeed *= 2
                pressedm = 1
            end if
        else
            pressedm = 0
        end if
        
        if MULTIKEY(FB.SC_SLASH) then
            if (pressedd = 0) then
                camspeed /= 2
                pressedd = 1
            end if
        else
            pressedd = 0
        end if
        
        if MULTIKEY(FB.SC_PERIOD) then
            if (pressedp = 0) then
                camspeed = 2.0
                pressedp = 1
            end if
        else
            pressedp = 0
        end if
        
        if (camspeed <= 0) then camspeed =.5 end if
        
		if MULTIKEY(FB.SC_W) then
			xpos = xpos - sin(heading*piover180) * camspeed
			zpos = zpos - cos(heading*piover180) * camspeed
        end if
	
		if MULTIKEY(FB.SC_S) then
			xpos = xpos + sin(heading*piover180) * camspeed
			zpos = zpos + cos(heading*piover180) * camspeed
		end if
	
		if MULTIKEY(FB.SC_A) then
			xpos = xpos - sin(heading*piover180+(3.141592654/2)) * camspeed
			zpos = zpos - cos(heading*piover180+(3.141592654/2)) * camspeed
 		end if
	
		if MULTIKEY(FB.SC_D) then
			xpos = xpos + sin(heading*piover180+(3.141592654/2)) * camspeed
			zpos = zpos + cos(heading*piover180+(3.141592654/2)) * camspeed
 		end if
        
        if MULTIKEY(FB.SC_R) then
			ypos = ypos + camspeed
 		end if
        
        if MULTIKEY(FB.SC_F) then
			ypos = ypos - camspeed
        end if


		if MULTIKEY(FB.SC_UP) then
			lookupdown = lookupdown - 2.5  '' look up
		end if

		if MULTIKEY(FB.SC_DOWN) then
			lookupdown = lookupdown + 2.5  '' look down
		end if
        
        if (lookupdown > 90) then lookupdown = 90
        if (lookupdown < -90) then lookupdown = -90
        
        
		if MULTIKEY(FB.SC_RIGHT) then
			heading = heading - 2.5
			yrot = heading
		end if

		if MULTIKEY(FB.SC_LEFT) then
			heading = heading + 2.5
			yrot = heading
		end if
        
        if MULTIKEY(FB.SC_COMMA) then
            if (pressedc = 0) then
                lookupdown = 0
                heading = 0
                yrot = 0
                xpos = 0
                ypos = 0
                zpos = 0
                pressedc = 1
            end if
        else
            pressedc = 0
        end if
                
		flip


end sub

sub decode()

chdir exepath

sleep 1

sleep 1

redim triangles ( 0 to ( filesize / 16 ) )

sleep 1

dlist = 0
curt = 0

address = 1
taddress = address

while ( address < filesize )
    
    abyte = file(address)
    select case abyte
    case &hE7
            taddress += 1
            tbyte = file(taddress)
            select case tbyte
            case 0
            
            taddress += 1
            tbyte = file(taddress)
            select case tbyte
            case 0
            
            taddress += 1
            tbyte = file(taddress)
            select case tbyte
            case 0
            
            taddress += 1
            tbyte = file(taddress)
            select case tbyte
            case 0
            
            taddress += 1
            tbyte = file(taddress)
            select case tbyte
            case 0
            
            taddress += 1
            tbyte = file(taddress)
            select case tbyte
            case 0
            
            taddress += 1
            tbyte = file(taddress)
            select case tbyte
            
            case 0
            curentry += 1
            
            if dlist = 0 then
            if (curentry >= entry) then
            dlist = 1
            end if
            end if
        
            end select
            end select
            end select
            end select
            end select
            end select
            end select
    case &hDF
            if dlist = 1 then
                dlist = 0
                if all = 0 then
                    exit while
                end if
            end if
    
    case 1
        dim as integer which = 0
        if dlist = 1 then
            taddress += 4
            tbyte = file(taddress)
            if tbyte = 6 then
            bank06=1
            taddress -= 3
            tbyte = file(taddress)
            len01 = tbyte * 256
            taddress += 1
            tbyte = file(taddress)
            len01 += tbyte
            
            taddress += 3
            tbyte = file(taddress)
            spot01 = tbyte * &h10000
            taddress += 1
            tbyte = file(taddress)
            spot01 += tbyte * 256
            taddress += 1
            tbyte = file(taddress)
            spot01 += tbyte
            
            
            spot01 /= 2
            
            redim xverts(0 to len01)
            redim yverts(0 to len01)
            redim zverts(0 to len01)
            redim rverts(0 to len01)
            redim gverts(0 to len01)
            redim bverts(0 to len01)
            redim averts(0 to len01)
            redim tris(0 to len01/16*3)
            taddress = spot01 + 1
            which = 0
            
            while taddress < spot01 + len01
                
                tbyte = file(taddress)
                xverts(which) = tbyte * 256
                taddress += 1
                tbyte = file(taddress)
                xverts(which) += tbyte
                taddress += 1
                
                
                tbyte = file(taddress)
                yverts(which) = tbyte * 256
                taddress += 1
                tbyte = file(taddress)
                yverts(which) += tbyte
                taddress += 1
                
                
                tbyte = file(taddress)
                zverts(which) = tbyte * 256
                taddress += 1
                tbyte = file(taddress)
                zverts(which) += tbyte
                taddress += 7
                
                tbyte = file(taddress)
                rverts(which) = tbyte
                taddress += 1
                tbyte = file(taddress)
                gverts(which) = tbyte
                taddress += 1
                tbyte = file(taddress)
                bverts(which) = tbyte
                taddress += 1
                tbyte = file(taddress)
                averts(which) = tbyte
                taddress += 1
                
                
                
                which += 1
            wend
            else
                taddress += 4
                bank06=0
            endif
        end if
    case 6
        if dlist = 1 then
        if bank06 = 1 then
            taddress += 1
            
            glBegin GL_TRIANGLES
            glNormal3f 0.0, 0.0, 1.0
            tri051 = file(taddress)
            glcolor4ub rverts(tri051 / 2), gverts(tri051 / 2), bverts(tri051 / 2), averts(tri051 / 2)
            glVertex3f xverts(tri051 / 2), yverts(tri051 / 2), zverts(tri051 / 2)
            taddress += 1
            curv = 1
            
            tri052 = file(taddress)
            glcolor4ub rverts(tri052 / 2), gverts(tri052 / 2), bverts(tri052 / 2), averts(tri052 / 2)
            glVertex3f xverts(tri052 / 2), yverts(tri052 / 2), zverts(tri052 / 2)
            taddress += 1
            curv = 2
            
            tri053 = file(taddress)
            glcolor4ub rverts(tri053 / 2), gverts(tri053 / 2), bverts(tri053 / 2), averts(tri053 / 2)
            glVertex3f xverts(tri053 / 2), yverts(tri053 / 2), zverts(tri053 / 2)
            glEnd
            
            
            taddress += 2
            curv = 0
            curt += 1
            
            glBegin GL_TRIANGLES
            glNormal3f 0.0, 0.0, 1.0
            tri061 = file(taddress)
            glcolor4ub rverts(tri061 / 2), gverts(tri061 / 2), bverts(tri061 / 2), averts(tri061 / 2)
            glVertex3f xverts(tri061 / 2), yverts(tri061 / 2), zverts(tri061 / 2)
            taddress += 1
            curv = 1
            
            tri062 = file(taddress)
            glcolor4ub rverts(tri062 / 2), gverts(tri062 / 2), bverts(tri052 / 2), averts(tri062 / 2)
            glVertex3f xverts(tri062 / 2), yverts(tri062 / 2), zverts(tri052 / 2)
            taddress += 1
            curv = 2
            
            tri063 = file(taddress)
            glcolor4ub rverts(tri063 / 2), gverts(tri063 / 2), bverts(tri063 / 2), averts(tri063 / 2)
            glVertex3f xverts(tri063 / 2), yverts(tri063 / 2), zverts(tri063 / 2)
            glEnd

            
            curv = 0
            curt += 1
        end if
        end if
        
        
    case 5
        if dlist = 1 then
        if bank06 = 1 then
            taddress += 1
            
            glBegin GL_TRIANGLES
            glNormal3f 0.0, 0.0, 1.0
            tri051=file(taddress)
            glcolor4ub rverts(tri051 / 2), gverts(tri051 / 2), bverts(tri051 / 2), averts(tri051 / 2)
            glVertex3f xverts(tri051 / 2), yverts(tri051 / 2), zverts(tri051 / 2)
            taddress += 1
            curv = 1
            
            tri052 =file(taddress)
            glcolor4ub rverts(tri052 / 2), gverts(tri052 / 2), bverts(tri052 / 2), averts(tri052 / 2)
            glVertex3f xverts(tri052 / 2), yverts(tri052 / 2), zverts(tri052 / 2)
            taddress += 1
            curv = 2
            
            tri053 = file(taddress)
            glcolor4ub rverts(tri053 / 2), gverts(tri053 / 2), bverts(tri053 / 2), averts(tri053 / 2)
            glVertex3f xverts(tri053 / 2), yverts(tri053 / 2), zverts(tri053 / 2)
            glEnd
            
            
            taddress += 5
            curv = 0
            curt += 1
        end if
        end if
    end select
    
    address += 8
    taddress = address
    
wend

end sub


