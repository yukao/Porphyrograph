from KineticsKit import *
import visual

print """Try to click at a Mass to grip it, click again to release it.
Zooming / rotating of the scene is done by holding the left / right
mouse button down while moving the mouse around."""

rate = 30
count = 11 # number of masses

system = System(timestep=1./rate, gravity=5.0, viscosity=0.1)

# generate some masses and insert them into the system
dx = 2.0 / count
for x in visual.arange(-1, 1+dx, dx):
    system.insertMass(Mass(m=0.1, pos=(x, x*0.7, 0.0)))

# an objects property can also be changed via the reference inserted into the system
system.masses[0].fixed = 1
system.masses[-1].fixed = 1

# connect the masses with springs
for i in range(len(system.masses)-1):
    system.insertSpring(CylinderSpring(m0=system.masses[i], m1=system.masses[i+1], k=10., damping=0.0))

system.display.forward = (-0.5, -0.4, -0.2)

#start the mainloop (convenience function)
system.mainloop()
