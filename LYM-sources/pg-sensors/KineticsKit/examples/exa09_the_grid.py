from KineticsKit import *
import visual
import math

xmax = 15
zmax = 15
interactive = 1

def xz(x, z):
    return z * xmax + x

system = System(timestep=1./25,
                oversample=1,
                gravity=None,
                viscosity=0.05,
                width=320,
                height=240,
                fov=visual.pi/6.)
if interactive:
    print """Try to click at a Mass to grip it, click again to release it.
Zooming / rotating of the scene is done by holding the left / right
mouse button down while moving the mouse around."""

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
            radius = 0.01
            color = (0.5, 0.5, 0.5)
        else:
            fixed = 0
            radius = 0.015
            color = (0.647059, 0.164706, 0.164706)
        mass = Mass(m=0.05, pos=visual.vector(xcoord, 0, zcoord), r=radius, fixed=fixed, color=color)
        ## texture {pigment {color %s} finish {specular 0.5 metallic}}
        system.insertMass(mass)

# springs
for z in range(zmax):
    for x in range(xmax-1):
        m0 = system.masses[xz(x, z)]
        m1 = system.masses[xz(x+1, z)]
        if z == 0 or z == zmax-1:
            spring = CylinderSpring(m0=m0, m1=m1, k=10, damping=0.00, segments=1, radius1=0.01, color=(0.5, 0.5, 0.5))
        else:
            spring = CylinderSpring(m0=m0, m1=m1, k=10, damping=0.00, segments=1, radius1=0.007, color=(1.0, 0.5, 0.0))
        system.insertSpring(spring)
for x in range(xmax):
    for z in range(zmax-1):
        m0 = system.masses[xz(x, z)]
        m1 = system.masses[xz(x, z+1)]
        if x == 0 or x == xmax-1:
            spring = CylinderSpring(m0=m0, m1=m1, k=10, damping=0.00, segments=1, radius1=0.01, color=(0.5, 0.5, 0.5))
        else:
            spring = CylinderSpring(m0=m0, m1=m1, k=10, damping=0.00, segments=1, radius1=0.007, color=(1.0, 0.5, 0.0))
        system.insertSpring(spring)

# tense springs
for spring in system.springs:
    spring.l0 = spring.l0 / 10.

# place camera
system.display.forward = (0.5, -0.5, 1.0)

system.display.scale *= 1.3
system.display.autoscale = 0

if interactive:
    system.mainloop()
else:
    system.display.userzoom = 0
    system.display.userspin = 0
    frames = 25 #* 10
    coslength = 25
    import string, math, time
    time.sleep(2)
    for frame in range(frames):
        if frame <= coslength:
            arg = frame / float(coslength) * 2 * math.pi
            system.masses[xz(xmax/2, zmax/2)].sphere.y = (1 - math.cos(arg)) * 0.1
            system.masses[xz(xmax/2, zmax/2)].fixed = 1
        elif frame <= 100:
            system.masses[xz(xmax/2, zmax/2)].sphere.y = 0
            system.masses[xz(xmax/2, zmax/2)].fixed = 1
        else:
            system.masses[xz(xmax/2, zmax/2)].fixed = 0
        print 'frame', frame+1, 'of', frames
        filename = 'frm' + string.zfill(frame, 4) + '.pov'
        print '  advancing ...',
        system.advance()
        print 'done'
        print '  writing system ...',
        system.povexport(filename=filename, custom_text='\nglobal_settings { assumed_gamma 2.2 }\n')
        print 'done'
    system.display.visible = 0
