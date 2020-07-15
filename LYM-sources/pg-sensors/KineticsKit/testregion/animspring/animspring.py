from KineticsKit import *
import visual
import math, string, time

width = 50
height = 10
writepov = 1

inifile = """\
[%(w)sx%(h)s, AA]
Width=%(w)s
Height=%(h)s
Antialias=On
Sampling_Method=2 ; adaptive and recursive super-sampling method
Antialias_Depth=3
Antialias_Threshold=0.1
""" % {'w':width, 'h':height}

f = open('animspring.ini', 'wt')
f.write(inifile)
f.close()

system = System(timestep=0.04,
                width=500,
                height=110,
                background=(1,1,1),
                ambient=0.4,
                lights = [visual.vector(  0.3,  0.1, 0.4),
                          visual.vector( -0.3, -0.1, 0.4)],
                fov=visual.pi/6.,
                range=(0.6, 0.6, 0.6),
                autoscale=0,
                userzoom=0,
                userspin=0)

m1 = Mass(m=0.1, pos=(-0.3, 0, 0), fixed=1, color=(1.0, 0.6, 0.2))
m2 = Mass(m=0.1, pos=(+0.3, 0, 0), fixed=1, color=(1.0, 0.6, 0.2))
system.insertMassList([m1, m2])

s = SingleHelixSpring(m1, m2, k=1, spc=20, color=(0.6, 0.8, 0.2))
system.insertSpring(s)

frames = 20
time.sleep(1)
for i in range(frames):
    t = float(i) / frames * 2 * math.pi
    dx = 0.12 * math.sin(t)
    m1.sphere.x = -0.3 - dx
    m2.sphere.x =  0.3 + dx
    filename = 'frame' + string.zfill(i, 2) + '.pov'
    print filename
    system.advance()
    if writepov:
        system.povexport(filename, xy_ratio=width/float(height), shadowless=1)
system.display.visible = 0
