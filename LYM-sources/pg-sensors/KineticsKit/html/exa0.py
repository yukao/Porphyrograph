# used in the 'Usage' section of the documentation

from KineticsKit import System, Mass, CylinderSpring
system = System(timestep=0.04)

## generate some masses
mass1 = Mass(m=0.1, pos=(0.0, 0.0, 0.0))
mass2 = Mass(m=0.1, pos=(0.0, 0.5, 0.0))
## insert them into the system
system.insertMass(mass1)
system.insertMass(mass2)

## connect the masses with a spring
spring1 = CylinderSpring(m0=mass1, m1=mass2, k=1)
## insert the spring into the system
system.insertSpring(spring1)

## start the mainloop (convenience function)
system.mainloop()
