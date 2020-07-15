from KineticsKit import *
import visual

print """Try to click at a Mass to grip it, click again to release it.
Zooming / rotating of the scene is done by holding the left / right
mouse button down while moving the mouse around."""

rate = 30
count = 4 # number of systems

systems = [None] * count
for i in range(count):
    visc = i * 0.1 # each system should have a different viscosity
    systems[i] = System(timestep=1./rate, gravity=9.84, viscosity=visc, name='viscosity='+str(visc),
                        x = i * 200,
                        y = 0,
                        width = 200,
                        height = 400,
                        )

    # generate some masses
    mass1 = Mass(m=0.1, pos=(0.0, 0.0, 0.0))
    mass2 = Mass(m=0.1, pos=(0.01, 0.5, 0.0))
    mass3 = Mass(m=0.1, pos=(0.0, 1.0, 0.0))
    # insert them into the system
    systems[i].insertMass(mass1)
    systems[i].insertMass(mass2)
    systems[i].insertMass(mass3)

    # an objects property can be changed like this
    mass1.fixed = 1

    # connect the masses with springs
    spring1 = CylinderSpring(m0=mass1, m1=mass2, k=10., damping=0.01)
    spring2 = CylinderSpring(m0=mass2, m1=mass3, k=10., damping=0.01)
    # insert the springs into the system
    systems[i].insertSpring(spring1)
    systems[i].insertSpring(spring2)

    systems[i].display.autoscale=0

# manual mainloop
while 1:
    for system in systems:
        system.dispatchDnD()
        system.advance()
    visual.rate(rate)
