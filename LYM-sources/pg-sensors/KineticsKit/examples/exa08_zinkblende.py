from KineticsKit import *
from vector import Vector # quadray Vector class by Kirby Urner
import visual

dnd = 0 # 1 to drag and drop the masses; 0 to export to pov after a mouseclick

# [ ((pos_tuple), color_index, fixed), ... ]
locations = [((0,0,0,0),   0, 1), #  0
             ((0,-1,0,0),  1, 0), #  1
             ((0,-1,1,0),  0, 1), #  2
             ((0,-2,1,0),  1, 0), #  3
             ((0,-2,2,0),  0, 1), #  4
             ((0,-3,2,0),  1, 1), #  5

             ((1,-1,-1,1), 0, 1), #  6
             ((1,-2,-1,1), 1, 1), #  7
             ((1,-2,0,1),  0, 0), #  8
             ((1,-3,0,1),  1, 1), #  9
             ((1,-3,1,1),  0, 0), # 10
             ((1,-4,1,1),  1, 1), # 11

             ((0,-1,0,1),  0, 1), # 12
             ((0,-2,0,1),  1, 0), # 13
             ((0,-2,0,2),  0, 1), # 14
             ((0,-3,0,2),  1, 1), # 15

             ((1,-1,1,-1), 0, 1), # 16
             ((1,-2,1,-1), 1, 1), # 17
             ((1,-2,1,0),  0, 0), # 18
             ((1,-3,1,0),  1, 1), # 19

             ((1,-1,0,0),  0, 1), # 20
             ((1,-2,0,0),  1, 0), # 21
             ((2,-2,0,0),  0, 1), # 22
             ((2,-3,0,0),  1, 1), # 23

             ((-1,-1,1,1), 0, 1), # 24
             ((-1,-2,1,1), 1, 1), # 25
             ((0,-2,1,1),  0, 0), # 26
             ((0,-3,1,1),  1, 1), # 27
             ]

# [ ((m1_index,m2_index), color_index ), ... ]
connections = [((0,1),   0),
               ((1,2),   0),
               ((2,3),   0),
               ((3,4),   0),
               ((4,5),   0),

               ((6,7),   0),
               ((7,8),   0),
               ((8,9),   0),
               ((9,10),  0),
               ((10,11), 0),

               ((1,12),  0),
               ((12,13), 0),
               ((13,14), 0),
               ((14,15), 0),

               ((16,17), 0),
               ((17,18), 0),
               ((18,19), 0),
               ((19,10), 0),

               ((1,20),  0),
               ((20,21), 0),
               ((21,22), 0),
               ((22,23), 0),

               ((24,25), 0),
               ((25,26), 0),
               ((26,27), 0),
               ((27,10), 0),

               ((1,12),  0),
               ((13,8),  0),
               ((8,21),  0),
               ((21,18), 0),
               ((18,3),  0),
               ((3,26),  0),
               ((26,13), 0),

               ((0,6),   1),
               ((6,14),  1),
               ((14,24), 1),
               ((24,0),  1),
               ((16,22), 1),
               ((22,10), 1),
               ((10,4),  1),
               ((4,16),  1),
               ((0,16),  1),
               ((6,22),  1),
               ((14,10), 1),
               ((24,4),  1),

               ((1,7),   2),
               ((7,15),  2),
               ((15,25), 2),
               ((25,1),  2),
               ((17,23), 2),
               ((23,11), 2),
               ((11,5),  2),
               ((5,17),  2),
               ((1,17),  2),
               ((7,23),  2),
               ((15,11), 2),
               ((25,5),  2),
               ]

rate = 30
center = ( Vector(quad=locations[0][0]) + Vector(quad=locations[11][0]) ) / 2.0
system = System(timestep=1./rate, viscosity=0.5, center=center.xyz(), name='Zinkblende',
                x=0,
                y=0,
                width=640,
                height=480,
                background = (0,0.2,0.2),
                ambient = 0.3,
                fov = visual.pi/3.
                )

masses = []
for loc, type, fixed in locations:
    if type == 0:
        masses.append( Mass(m=1, pos=Vector(quad=loc).xyz(), fixed=fixed, pickable=not fixed) )
    else:
        masses.append( Mass(m=3, pos=Vector(quad=loc).xyz(), fixed=fixed, pickable=not fixed, color = (0.2, 0.8, 0.2)) )

springs = []
for con, type in connections:
    if type == 0:
        springs.append( CylinderSpring(m0=masses[con[0]], m1=masses[con[1]], k=10, radius1=0.1, segments=1) )
    elif type == 1:
        springs.append( CylinderSpring(m0=masses[con[0]], m1=masses[con[1]], k=10, radius1=0.04, segments=1, color=(0.6,0.6,0.8)) )
    else:
        springs.append( CylinderSpring(m0=masses[con[0]], m1=masses[con[1]], k=10, radius1=0.04, segments=1, color=(0.8,0.6,0.8)) )

system.insertMassList(masses)
system.insertSpringList(springs)

if dnd:
    system.mainloop()
else:
    while 1:
        print 'position the camera somewhere, then click to export to pov'
        system.display.mouse.getclick()
        print 'exporting scene to file ...',
        system.povexport(shadowless=1)
        print 'done'
        print 'scene can now be rendered with POV-Ray\n'
