from KineticsKit import *
import visual
import math

xmax = 10
zmax = 10
interactive = 1

def xz(x, z):
    return z * xmax + x

system = System(timestep=1./25,
                oversample=1,
                gravity=0,
                viscosity=0.05,
                fov=visual.pi/4.
                )
if interactive:
    print """Try to click at a Mass to grip it, click again to release it.
Zooming / rotating of the scene is done by holding the left / right
mouse button down while moving the mouse around."""
    system.display.background = (0,0.2,0.2)
else:
    system.display.background = (1.0, 1.0, 1.0)

system.display.lights = [visual.vector(-0.2,  0.5,  0.5),
                         visual.vector(-0.2,  0.5, -0.3),
                         visual.vector( 0.2, -0.5,  0.3)]

# masses
for z in range(zmax):
    for x in range(xmax):
        xcoord = (x / float(xmax-1) - 0.5) / 2.
        zcoord = (z / float(zmax-1) - 0.5) / 2.
        if x == 0 or x == xmax-1 or z == 0 or z == zmax-1:
            fixed = 1
            radius = 0.015
            color = (0.4, 0.6, 0.8)
        else:
            fixed = 0
            radius = 0.02
            color=(0.8, 0.5, 0.3)
        ycoord = (zcoord*1.2)**2 - (xcoord*1.2)**2
        mass = Mass(m=0.1, pos=visual.vector(xcoord, ycoord, zcoord), r=radius, fixed=fixed, color=color)
        system.insertMass(mass)

# springs
for z in range(zmax):
    for x in range(xmax-1):
        m0 = system.masses[xz(x, z)]
        m1 = system.masses[xz(x+1, z)]
        if z == 0 or z == zmax-1:
            spring = CylinderSpring(m0=m0, m1=m1, k=1, damping=0.00, segments=1, radius1=0.015, color=(0.4, 0.6, 0.8))
        else:
            spring = CylinderSpring(m0=m0, m1=m1, k=1, damping=0.00, segments=1, radius1=0.01, color=(0.5, 0.7, 0.2))
        system.insertSpring(spring)
for x in range(xmax):
    for z in range(zmax-1):
        m0 = system.masses[xz(x, z)]
        m1 = system.masses[xz(x, z+1)]
        if x == 0 or x == xmax-1:
            spring = CylinderSpring(m0=m0, m1=m1, k=1, damping=0.00, segments=1, radius1=0.015, color=(0.4, 0.6, 0.8))
        else:
            spring = CylinderSpring(m0=m0, m1=m1, k=1, damping=0.00, segments=1, radius1=0.01, color=(0.5, 0.7, 0.2))
        system.insertSpring(spring)

# tense springs
for spring in system.springs:
    spring.l0 = spring.l0 / 10.

# place camera
system.display.forward = (-0.3, -0.3, -0.1)

if interactive:
    system.mainloop()
else:
    system.display.userzoom = 0
    system.display.userspin = 0

    width = 64
    height = 40
    writepov = 1
    frames = 25

    inifile = """\
[%(w)sx%(h)s, AA]
Width=%(w)s
Height=%(h)s
Antialias=On
Sampling_Method=2 ; adaptive and recursive super-sampling method
Antialias_Depth=3
Antialias_Threshold=0.1
""" % {'w':width, 'h':height}

    f = open('animgrid.ini', 'wt')
    f.write(inifile)
    f.close()

    import string, math, time
    for frame in range(frames):
        arg = frame / float(frames) * math.pi
        fwdx = math.sin(arg) / 2.
        fwdz = math.cos(arg) / 2.
        camx = math.sin(arg+math.pi/4.) / 2.
        camz = math.cos(arg+math.pi/4.) / 2.
        system.display.forward = (-fwdx*2, -0.5, -fwdz*2)
        system.display.lights = [visual.vector(camx,  0.5, camz),
                                 visual.vector(fwdx,  0.0, fwdz)]
        print 'frame', frame+1, 'of', frames
        filename = 'frm' + string.zfill(frame, 2) + '.pov'
        if writepov:
            time.sleep(2)
            print '  writing system ...',
            system.povexport(filename=filename, xy_ratio=width/float(height), shadowless=1)
            print 'done'
    system.display.visible = 0
