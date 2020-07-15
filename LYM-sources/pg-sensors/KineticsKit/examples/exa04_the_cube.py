from KineticsKit import *
from visual import vector

print """Try to click at a Mass to grip it, click again to release it.
Zooming / rotating of the scene is done by holding the left / right
mouse button down while moving the mouse around."""

rate = 30
system = System(timestep=1./rate, gravity=0.0, viscosity=0.0)

# generate some masses
masses = []
masses.append(Mass(m=1.0, pos=(-1.0, -1.0, -1.0)))
masses.append(Mass(m=1.0, pos=(1.0, -1.0, -1.0)))
masses.append(Mass(m=1.0, pos=(-1.0, 1.0, -1.0)))
masses.append(Mass(m=1.0, pos=(1.0, 1.0, -1.0)))
masses.append(Mass(m=1.0, pos=(-1.0, -1.0, 1.0)))
masses.append(Mass(m=1.0, pos=(1.0, -1.0, 1.0)))
masses.append(Mass(m=1.0, pos=(-1.0, 1.0, 1.0)))
masses.append(Mass(m=1.0, pos=(1.0, 1.0, 1.0)))
masses.append(Mass(m=1.0, pos=(0.0, 0.0, 0.0), fixed=1))
# insert them into the system
for mass in masses:
    system.insertMass(mass)

# connect the masses with springs
springs = []
springs.append(CylinderSpring(m0=masses[0], m1=masses[1], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[1], m1=masses[3], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[3], m1=masses[2], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[2], m1=masses[0], k=10., damping=0.05))

springs.append(CylinderSpring(m0=masses[4], m1=masses[5], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[5], m1=masses[7], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[7], m1=masses[6], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[6], m1=masses[4], k=10., damping=0.05))

springs.append(CylinderSpring(m0=masses[0], m1=masses[4], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[1], m1=masses[5], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[3], m1=masses[7], k=10., damping=0.05))
springs.append(CylinderSpring(m0=masses[2], m1=masses[6], k=10., damping=0.05))

for mass in masses:
    if mass is not masses[-1]:
        springs.append(CylinderSpring(m0=masses[-1], m1=mass, k=10., damping=0.05))

for spring in springs:
    system.insertSpring(spring)

masses[0].v = vector(1.5, 0.7, 0.3)

#start the mainloop (convenience function)
system.mainloop()
