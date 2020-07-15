from KineticsKit import *
import visual
import math

rate = 30
count = 5 # number of masses per dimension
#Spring = CylinderSpring
Spring = DoubleHelixSpring
dnd = 1 # 1 to drag and drop the masses; 0 to export to pov and exit

system = System(timestep=1./rate,
                viscosity=0.1,
                fov=math.pi/4.,
                ambient=0.5)
if dnd:
    print """Try to click at a Mass to grip it, click again to release it.
Zooming / rotating of the scene is done by holding the left / right
mouse button down while moving the mouse around."""
    system.display.background = (0,0.2,0.2)
    spc = 10
else:
    system.display.background = (1.0, 1.0, 1.0)
    spc = 20

# generate some masses
dx = dy = dz = 2.0 / (count-1)
for z in visual.arange(-1, 1+dz, dz):
    for y in visual.arange(-1, 1+dy, dy):
        for x in visual.arange(-1, 1+dx, dx):
            system.insertMass( Mass(m=0.1, pos=(x, y, z)) )

# connect the masses with springs
for z in range(count):
    for y in range(count):
        for x in range(count-1):
            m0 = system.masses[z*count*count + y*count + x]
            m1 = system.masses[z*count*count + y*count + (x+1)]
            system.insertSpring( Spring(m0=m0, m1=m1, k=10., spc=spc) )
for z in range(count):
    for y in range(count-1):
        for x in range(count):
            m0 = system.masses[z*count*count + y*count + x]
            m1 = system.masses[z*count*count + (y+1)*count + x]
            system.insertSpring( Spring(m0=m0, m1=m1, k=10., spc=spc) )
for z in range(count-1):
    for y in range(count):
        for x in range(count):
            m0 = system.masses[z*count*count + y*count + x]
            m1 = system.masses[(z+1)*count*count + y*count + x]
            system.insertSpring( Spring(m0=m0, m1=m1, k=10., spc=spc) )

# place camera
system.display.forward = (-0.1, -0.1, -2.0)
system.display.scale *= 1.1

if dnd:
    system.mainloop()
else:
    inifile = """\
[150x150, AA]
Width=150
Height=150
Antialias=On
Sampling_Method=2 ; adaptive and recursive super-sampling method
Antialias_Depth=3
Antialias_Threshold=0.1
"""

    f = open('lattice.ini', 'wt')
    f.write(inifile)
    f.close()

    import time
    time.sleep(5)
    start = time.time()
    system.display.visible = 0 # to speed up exporting
    print 'exporting ...',
    system.povexport(filename='lattice.pov', xy_ratio=1, shadowless=1)
    print 'took', time.time() - start, 'seconds'
