from visual import *
import povexport

print """Instructions: zoom and rotate to get the viewing angle
you want.  Click the mouse to generate POV-Ray code.
By default code will be written to a file whose name
is the display window title, or you can specify a filename."""

scene.x=scene.y=0
scene.width=640
scene.height=480
##scene.background = (0,.2,.2)

ihat=vector(1,0,0)
jhat=vector(0,1,0)
khat=vector(0,0,1)

axes=curve(pos=[(-4,0,0),(4,0,0),(0,0,0),(0,4,0),(0,-4,0),(0,0,0),(0,0,4),
                (0,0,-4)], color=(.5,.5,.5), radius=0.05, pov_texture = 'T_Wood10')


f=frame(axis=(-1,-1,1))
xxx = f.axis
print xxx

cone001 = cone(pos=(-3,-2,-2),axis = (0,3,0),radius=2.5, color=color.yellow, pov_texture = 'T_Wood11')
cone001.rotate(axis=khat, angle=-pi/5.0)

a = arrow(pos=(0,0,0), axis=ihat*3, color=color.cyan, frame=f, pov_texture = 'T_Wood13')

s = sphere(pos = ihat*3, radius=0.1, color=color.green, pov_texture = 'T_Wood19')
r = ring(frame=f, radius=1, thickness=0.2, color=color.red,
         pov_texture = 'T_Wood20')

b=box(pos=(0,0,-3), size=(1.0, 0.5, 0.25), color=color.magenta,
      frame=f, pov_texture = 'T_Wood19')

x = arange(0,4,0.4)
helix = curve (x = x, y = 0.5*sin(2*x), z = 2+0.5*cos(2*x),
               radius = 0.1, color=color.magenta,
               frame = f, pov_texture = 'T_Wood3')

inclist = ['colors.inc', 'stones.inc', 'woods.inc', 'metals.inc']

scene.mouse.getclick()  ## position camera somewhere, then click
print povexport.version()
print "Begin exporting pov file"
povexport.export(filename = "woodexample.pov", display=scene, include_list = inclist)
print "Export finished"
