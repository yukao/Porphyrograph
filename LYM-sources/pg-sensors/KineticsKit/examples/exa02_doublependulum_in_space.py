from KineticsKit import *
from visual import vector

print """Try to click at a Mass to grip it, click again to release it.
Zooming / rotating of the scene is done by holding the left / right
mouse button down while moving the mouse around."""

rate = 30
system = System(timestep=1./rate, oversample=1, gravity=0.0, viscosity=0.0)

# generate some masses
mass1 = Mass(m=0.2, pos=(0.0, -0.5, 0.0))
mass2 = Mass(m=0.1, pos=(0.0, 0.0, 0.0))
mass3 = Mass(m=0.1, pos=(0.0, 0.5, 0.0))
# insert them into the system
system.insertMass(mass1)
system.insertMass(mass2)
system.insertMass(mass3)

# give the system some initial energy
mass1.v = vector(-0.5, 0, 0)
mass2.v = vector(1.0, 0, 0.5)
mass3.v = vector(0.0, 0, -0.5)

# connect the masses with springs
spring1 = CylinderSpring(m0=mass1, m1=mass2, k=1., damping=0.1)
spring2 = CylinderSpring(m0=mass2, m1=mass3, k=1., damping=0.1)
# insert the springs into the system
system.insertSpring(spring1)
system.insertSpring(spring2)

#start the mainloop (convenience function)
system.mainloop()
