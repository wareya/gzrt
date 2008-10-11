''K um, I might've used a lot of this from NeHe
''but I rewrote most of it due to incompatabilities
''with my reasons and formats.

chdir exepath

#include once "GL/gl.bi"
#include once "GL/glu.bi"
#include once "crt.bi"          '' scanf is used to parse data file
#include once "fbgfx.bi"        '' for Scan code constants


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

dim shared as string filename


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
	v(0 to 2) as VERTEX          '' Array Of Three Vertices
end type

redim shared as TRIANGLE triangles(0 to 1)



''------------------------------------------------------------------------------
declare sub Load()


dim shared filter as uinteger                  '' Which Filter To Use
dim shared texture(0 to 2) as uinteger         '' Storage For 3 Textures
	
	dim blend as integer                    '' Blending OFF/ON?
	dim fp as integer                       '' F Pressed?
	dim bp as integer                       '' B Pressed?

	dim heading as single              '' direction of movement
	dim xpos as single                 '' X position
	dim zpos as single                 '' Y position
	dim ypos as single                 '' Y position
	
	dim yrot as single                 '' Y Rotation = heading
	dim walkbias as single             '' used with walkbiasangle for bouncing effect
	dim walkbiasangle as single        '' used with walkbias for bouncing effect
	dim lookupdown as single           '' View direction
	
	dim x_m as single            '' Floating Point For Temp X, Y, Z, U And V Vertices
	dim y_m as single
	dim z_m as single
    
    dim as single camspeed = 1.0
    dim as byte pressedm = 0
    dim as byte pressedp = 0
    
    dim r_m as single
	dim g_m as single
	dim b_m as single
	dim a_m as single
	
	dim xtrans as single         '' Used For Player Translation
	dim ztrans as single         '' Used For Player Translation
	dim ytrans as single         '' Used For Bouncing Motion Up And Down
	dim sceneroty as single      '' 360 Degree Angle For Player Direction

	dim shared as integer numtriangles  '' Integer To Hold The Number Of Triangles
	dim as integer loop_m        '' Loop counter
    
    
    Load()
    
    
	screen 18, 16, , 2

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

	do
		glClear GL_COLOR_BUFFER_BIT or GL_DEPTH_BUFFER_BIT      '' Clear Screen And Depth Buffer
		glLoadIdentity()                                        '' Reset The View
		
		xtrans = - xpos
		ztrans = - zpos
		ytrans = - ypos                      '' Used For Bouncing Motion Up And Down
		sceneroty = 360.0 - yrot                        '' 360 Degree Angle For Player Direction
		
		glRotatef lookupdown, 1.0, 0,0                  '' Rotate Up And Down To Look Up And Down
		glRotatef sceneroty, 0, 1.0, 0                  '' Rotate Depending On Direction Player Is Facing
		
		glTranslatef xtrans, ytrans, ztrans             '' Translate The Scene Based On Player Position
		glBindTexture GL_TEXTURE_2D, texture(filter)    '' Select A Texture Based On filter
		
		' Process Each Triangle
		for loop_m = 0 to curt - 1
		
			glBegin GL_TRIANGLES                          '' Start Drawing Triangles
            
				glNormal3f 0.0, 0.0, 1.0
				r_m = triangles(loop_m).v(0).r
				g_m = triangles(loop_m).v(0).g
				b_m = triangles(loop_m).v(0).b
				a_m = triangles(loop_m).v(0).a
				x_m = triangles(loop_m).v(0).x
				y_m = triangles(loop_m).v(0).y
				z_m = triangles(loop_m).v(0).z
                glcolor4ub r_m, g_m, b_m, a_m
			    glVertex3f x_m, y_m, z_m
                
				r_m = triangles(loop_m).v(1).r
				g_m = triangles(loop_m).v(1).g
				b_m = triangles(loop_m).v(1).b
				a_m = triangles(loop_m).v(1).a
                glcolor4ub r_m, g_m, b_m, a_m
				x_m = triangles(loop_m).v(1).x
				y_m = triangles(loop_m).v(1).y
				z_m = triangles(loop_m).v(1).z
			    glVertex3f x_m, y_m, z_m
                
				r_m = triangles(loop_m).v(2).r
				g_m = triangles(loop_m).v(2).g
				b_m = triangles(loop_m).v(2).b
				a_m = triangles(loop_m).v(2).a
                glcolor4ub r_m, g_m, b_m, a_m
				x_m = triangles(loop_m).v(2).x
				y_m = triangles(loop_m).v(2).y
				z_m = triangles(loop_m).v(2).z
                glVertex3f x_m, y_m, z_m
			glEnd
		next
		'' Keyboard handlers
        if MULTIKEY(FB.SC_PLUS) then
            if (pressedp = 0) then
                camspeed += .25
                pressedp = 1
            end if
        else
            pressedp = 0
        end if
        
        if MULTIKEY(FB.SC_MINUS) then
            if (pressedm = 0) then
                camspeed -= .25
                pressedm = 1
            end if
        else
            pressedm = 0
        end if
        
        if (camspeed <= 0) then camspeed =.25 end if
        
		if MULTIKEY(FB.SC_W) then
			xpos = xpos - sin(heading*piover180) * camspeed    '' Move On The X-Plane Based On Player Direction
			zpos = zpos - cos(heading*piover180) * camspeed    '' Move On The Z-Plane Based On Player Direction
        end if
	
		if MULTIKEY(FB.SC_S) then
			xpos = xpos + sin(heading*piover180) * camspeed    '' Move On The X-Plane Based On Player Direction
			zpos = zpos + cos(heading*piover180) * camspeed    '' Move On The Z-Plane Based On Player Direction
		end if
	
		if MULTIKEY(FB.SC_A) then
			xpos = xpos - sin(heading*piover180+(3.141592654/2)) * camspeed    '' Move On The X-Plane Based On Player Direction
			zpos = zpos - cos(heading*piover180+(3.141592654/2)) * camspeed    '' Move On The Z-Plane Based On Player Direction
 		end if
	
		if MULTIKEY(FB.SC_D) then
			xpos = xpos + sin(heading*piover180+(3.141592654/2)) * camspeed    '' Move On The X-Plane Based On Player Direction
			zpos = zpos + cos(heading*piover180+(3.141592654/2)) * camspeed    '' Move On The Z-Plane Based On Player Direction
 		end if
        
        if MULTIKEY(FB.SC_R) then
			ypos = ypos + camspeed
 		end if
        
        if MULTIKEY(FB.SC_F) then
			ypos = ypos - camspeed
        end if


		if MULTIKEY(FB.SC_UP) then
			lookupdown = lookupdown - 0.25  '' look up
		end if

		if MULTIKEY(FB.SC_DOWN) then
			lookupdown = lookupdown + 0.25  '' look down
		end if
        
        
        if (lookupdown > 90) then lookupdown = 90
        if (lookupdown < -90) then lookupdown = -90
        
        
		if MULTIKEY(FB.SC_RIGHT) then
			heading = heading - 0.25        '' Rotate The Scene To The Left
			yrot = heading
		end if

		if MULTIKEY(FB.SC_LEFT) then
			heading = heading + 0.25        '' Rotate The Scene To The Right
			yrot = heading
		end if
        

		flip  '' flip or crash
		if inkey = chr(255)+"k" then exit do
	loop while MULTIKEY(FB.SC_ESCAPE) = 0

	'' Empty keyboard buffer
	while inkey <> "": wend
    
    end
    
sub load
chdir exepath

sleep 1

filename = Command(1)

dim as string strentry = Command(2)
entry = cast(integer,strentry)
entry -= cast(integer,"0")

tstring = Command(3)

if (tstring="yes") then all = 1 else all = 0

Open filename for Binary as #1

sleep 1

filesize = Lof ( 1 )

redim triangles ( 0 to ( filesize / 16 ) )

sleep 1

print "Loading..."

address = 1
taddress = address
while ( address < filesize )
    
    
    Get #1, address, abyte, 1
    select case abyte
    case &hE7
            taddress += 1
            Get #1, taddress, tbyte
            select case tbyte
            case 0
            
            taddress += 1
            Get #1, taddress, tbyte
            select case tbyte
            case 0
            
            taddress += 1
            Get #1, taddress, tbyte
            select case tbyte
            case 0
            
            taddress += 1
            Get #1, taddress, tbyte
            select case tbyte
            case 0
            
            taddress += 1
            Get #1, taddress, tbyte
            select case tbyte
            case 0
            
            taddress += 1
            Get #1, taddress, tbyte
            select case tbyte
            case 0
            
            taddress += 1
            Get #1, taddress, tbyte
            select case tbyte
            
            case 0
            curentry += 1
            
            if dlist = 0 then
            if (curentry >= entry) then
            dlist = 1
            print "Display List:"
            print
            print
            
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
                print "...End display list."
                if all = 0 then
                    exit while
                end if
            end if
            print
            print
    
    case 1
        dim as integer which = 0
        if dlist = 1 then
            
            taddress += 1
            Get #1, taddress, tbyte
            len01 = tbyte * 256
            taddress += 1
            Get #1, taddress, tbyte
            len01 += tbyte
            
            print "Length: "; hex(len01)
            print
            
            taddress += 3
            Get #1, taddress, tbyte
            spot01 = tbyte * &h10000
            taddress += 1
            Get #1, taddress, tbyte
            spot01 += tbyte * 256
            taddress += 1
            Get #1, taddress, tbyte
            spot01 += tbyte
            
            
            print "Spot: "; hex(spot01)
            print
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
                
                Get #1, taddress + spot01, tbyte
                xverts(which) = tbyte * 256
                taddress += 1
                Get #1, taddress + spot01, tbyte
                xverts(which) += tbyte
                taddress += 1
                
                
                Get #1, taddress + spot01, tbyte
                yverts(which) = tbyte * 256
                taddress += 1
                Get #1, taddress + spot01, tbyte
                yverts(which) += tbyte
                taddress += 1
                
                
                Get #1, taddress + spot01, tbyte
                zverts(which) = tbyte * 256
                taddress += 1
                Get #1, taddress + spot01, tbyte
                zverts(which) += tbyte
                taddress += 7
                
                Get #1, taddress + spot01, tbyte
                rverts(which) = tbyte
                taddress += 1
                Get #1, taddress + spot01, tbyte
                gverts(which) = tbyte
                taddress += 1
                Get #1, taddress + spot01, tbyte
                bverts(which) = tbyte
                taddress += 1
                Get #1, taddress + spot01, tbyte
                averts(which) = tbyte
                taddress += 1
                
                
                
                which += 1
            wend
        end if
    case 6
        if dlist = 1 then
            taddress += 1
            print
            Get #1, taddress, tri051
            Print "1: "; xverts(tri051 / 2);
            Print ", "; yverts(tri051 / 2);
            Print ", "; zverts(tri051 / 2);
            Print "; "; rverts(tri051 / 2);
            Print ", "; gverts(tri051 / 2);
            Print ", "; bverts(tri051 / 2);
            Print ", "; averts(tri051 / 2)
            triangles(curt).v(curv).x = xverts(tri051 / 2)
            triangles(curt).v(curv).y = yverts(tri051 / 2)
            triangles(curt).v(curv).z = zverts(tri051 / 2)
            triangles(curt).v(curv).r = rverts(tri051 / 2)
            triangles(curt).v(curv).g = gverts(tri051 / 2)
            triangles(curt).v(curv).b = bverts(tri051 / 2)
            triangles(curt).v(curv).a = averts(tri051 / 2)
            taddress += 1
            curv = 1
            
            Get #1, taddress, tri052
            Print "2: "; xverts(tri052 / 2);
            Print ", "; yverts(tri052 / 2);
            Print ", "; zverts(tri052 / 2);
            Print "; "; rverts(tri052 / 2);
            Print ", "; gverts(tri052 / 2);
            Print ", "; bverts(tri052 / 2);
            Print ", "; averts(tri052 / 2)
            triangles(curt).v(curv).x = xverts(tri052 / 2)
            triangles(curt).v(curv).y = yverts(tri052 / 2)
            triangles(curt).v(curv).z = zverts(tri052 / 2)
            triangles(curt).v(curv).r = rverts(tri052 / 2)
            triangles(curt).v(curv).g = gverts(tri052 / 2)
            triangles(curt).v(curv).b = bverts(tri052 / 2)
            triangles(curt).v(curv).a = averts(tri052 / 2)
            
            
            taddress += 1
            curv = 2
            
            Get #1, taddress, tri053
            Print "3: "; xverts(tri053 / 2);
            Print ", "; yverts(tri053 / 2);
            Print ", "; zverts(tri053 / 2);
            Print "; "; rverts(tri053 / 2);
            Print ", "; gverts(tri053 / 2);
            Print ", "; bverts(tri053 / 2);
            Print ", "; averts(tri053 / 2)
            triangles(curt).v(curv).x = xverts(tri053 / 2)
            triangles(curt).v(curv).y = yverts(tri053 / 2)
            triangles(curt).v(curv).z = zverts(tri053 / 2)
            triangles(curt).v(curv).r = rverts(tri053 / 2)
            triangles(curt).v(curv).g = gverts(tri053 / 2)
            triangles(curt).v(curv).b = bverts(tri053 / 2)
            triangles(curt).v(curv).a = averts(tri053 / 2)
            

            
            taddress += 2
            curv = 0
            curt += 1
            
            Get #1, taddress, tri061
            Print "4: "; xverts(tri061 / 2);
            Print ", "; yverts(tri061 / 2);
            Print ", "; zverts(tri061 / 2);
            Print "; "; rverts(tri061 / 2);
            Print ", "; gverts(tri061 / 2);
            Print ", "; bverts(tri061 / 2);
            Print ", "; averts(tri061 / 2)
            triangles(curt).v(curv).x = xverts(tri061 / 2)
            triangles(curt).v(curv).y = yverts(tri061 / 2)
            triangles(curt).v(curv).z = zverts(tri061 / 2)
            triangles(curt).v(curv).r = rverts(tri061 / 2)
            triangles(curt).v(curv).g = gverts(tri061 / 2)
            triangles(curt).v(curv).b = bverts(tri061 / 2)
            triangles(curt).v(curv).a = averts(tri061 / 2)
            
            
            taddress += 1
            curv = 1
            
            Get #1, taddress, tri062
            Print "5: "; xverts(tri062 / 2);
            Print ", "; yverts(tri062 / 2);
            Print ", "; zverts(tri062 / 2);
            Print "; "; rverts(tri062 / 2);
            Print ", "; gverts(tri062 / 2);
            Print ", "; bverts(tri062 / 2);
            Print ", "; averts(tri062 / 2)
            triangles(curt).v(curv).x = xverts(tri062 / 2)
            triangles(curt).v(curv).y = yverts(tri062 / 2)
            triangles(curt).v(curv).z = zverts(tri062 / 2)
            triangles(curt).v(curv).r = rverts(tri062 / 2)
            triangles(curt).v(curv).g = gverts(tri062 / 2)
            triangles(curt).v(curv).b = bverts(tri062 / 2)
            triangles(curt).v(curv).a = averts(tri062 / 2)
            
            
            taddress += 1
            curv = 2
            
            Get #1, taddress, tri063
            Print "6: "; xverts(tri063 / 2);
            Print ", "; yverts(tri063 / 2);
            Print ", "; zverts(tri063 / 2);
            Print "; "; rverts(tri063 / 2);
            Print ", "; gverts(tri063 / 2);
            Print ", "; bverts(tri063 / 2);
            Print ", "; averts(tri063 / 2)
            triangles(curt).v(curv).x = xverts(tri063 / 2)
            triangles(curt).v(curv).y = yverts(tri063 / 2)
            triangles(curt).v(curv).z = zverts(tri063 / 2)
            triangles(curt).v(curv).r = rverts(tri063 / 2)
            triangles(curt).v(curv).g = gverts(tri063 / 2)
            triangles(curt).v(curv).b = bverts(tri063 / 2)
            triangles(curt).v(curv).a = averts(tri063 / 2)

            
            curv = 0
            curt += 1
            
        end if
        
        
    case 5
        if dlist = 1 then
            taddress += 1
            
            Get #1, taddress, tri051
            Print "1: "; xverts(tri051 / 2);
            Print ", "; yverts(tri051 / 2);
            Print ", "; zverts(tri051 / 2)
            Print "; "; rverts(tri051 / 2);
            Print ", "; gverts(tri051 / 2);
            Print ", "; bverts(tri051 / 2);
            Print ", "; averts(tri051 / 2)
            triangles(curt).v(curv).x = xverts(tri051 / 2)
            triangles(curt).v(curv).y = yverts(tri051 / 2)
            triangles(curt).v(curv).z = zverts(tri051 / 2)
            triangles(curt).v(curv).r = rverts(tri051 / 2)
            triangles(curt).v(curv).g = gverts(tri051 / 2)
            triangles(curt).v(curv).b = bverts(tri051 / 2)
            triangles(curt).v(curv).a = averts(tri051 / 2)
            

            taddress += 1
            curv = 1
            
            Get #1, taddress, tri052
            Print "2: "; xverts(tri052 / 2);
            Print ", "; yverts(tri052 / 2);
            Print ", "; zverts(tri052 / 2)
            Print "; "; rverts(tri052 / 2);
            Print ", "; gverts(tri052 / 2);
            Print ", "; bverts(tri052 / 2);
            Print ", "; averts(tri052 / 2)
            triangles(curt).v(curv).x = xverts(tri052 / 2)
            triangles(curt).v(curv).y = yverts(tri052 / 2)
            triangles(curt).v(curv).z = zverts(tri052 / 2)
            triangles(curt).v(curv).r = rverts(tri052 / 2)
            triangles(curt).v(curv).g = gverts(tri052 / 2)
            triangles(curt).v(curv).b = bverts(tri052 / 2)
            triangles(curt).v(curv).a = averts(tri052 / 2)
            

            taddress += 1
            curv = 2
            
            Get #1, taddress, tri053
            Print "3: "; xverts(tri053 / 2);
            Print ", "; yverts(tri053 / 2);
            Print ", "; zverts(tri053 / 2);
            Print "; "; rverts(tri053 / 2);
            Print ", "; gverts(tri053 / 2);
            Print ", "; bverts(tri053 / 2);
            Print ", "; averts(tri053 / 2)
            triangles(curt).v(curv).x = xverts(tri053 / 2)
            triangles(curt).v(curv).y = yverts(tri053 / 2)
            triangles(curt).v(curv).z = zverts(tri053 / 2)
            triangles(curt).v(curv).r = rverts(tri053 / 2)
            triangles(curt).v(curv).g = gverts(tri053 / 2)
            triangles(curt).v(curv).b = bverts(tri053 / 2)
            triangles(curt).v(curv).a = averts(tri053 / 2)
            
            
            taddress += 1
            curv = 0
            curt += 1
        end if
    end select
    
    address += 8
    taddress = address
    
wend

print "Loaded."
print "Preliminary Vertex Alpha."
Print ""
Print "Version: GZRT Plug 0.8A"

end sub
