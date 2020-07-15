import visual


class System:
    """The system.
    """

    def __init__(self, timestep, oversample=1, gravity=None, viscosity=None, name='KineticsKit', **keywords):
        """Construct a system.
        """
        self.timestep = timestep
        self.rate = 1.0 / timestep
        self.oversample = oversample
        self.gravity = gravity
        self.viscosity = viscosity

        self.masses = []
        self.springs = []

        self.display = visual.display(title=name, **keywords)
        self.display.select()
        self.dragObject = None
        self.click = None
        self.distance = None

        self.rememberFixed = None
        self.rememberColor = None

    def insertMass(self, mass):
        """Insert mass into the system.
        """
        self.masses.append(mass)

    def insertMassList(self, masslist):
        """Insert all Masses in masslist into the system.
        """
        map(self.insertMass, masslist)

    def insertSpring(self, spring):
        """Insert spring into the system.
        """
        self.springs.append(spring)

    def insertSpringList(self, springlist):
        """Insert all Springs in springlist into the system.
        """
        map(self.insertSpring, springlist)

    def advance(self):
        """Perform one Iteration of the system by advancing one timestep.
        """
        microstep = self.timestep / self.oversample

        for i in range(self.oversample):
            for spring in self.springs:
                spring.calcSpringForce()
                if spring.damping:
                    spring.calcDampingForce()

            for mass in self.masses:
                if not mass.fixed:
                    if self.gravity:
                        mass.calcGravityForce(self.gravity)
                    if self.viscosity:
                        mass.calcViscosityForce(self.viscosity)
                    mass.calcNewLocation(microstep)
                mass.clearForce()

            for spring in self.springs:
                spring.update()

    def dispatchDnD(self):
        """Process the drag and drop interaction from the mouse.
        """
        if self.display.mouse.clicked:
            self.click = self.display.mouse.getclick()
            if self.dragObject: # drop the selected object
                # restore original attributes
                self.dragObject.mass.fixed = self.rememberFixed
                self.dragObject.color = self.rememberColor
                # no initial velocity after dragging
                self.dragObject.mass.v = visual.vector(0., 0., 0.)
                self.dragObject = None
            elif self.click.pick and self.click.pick.__class__ == visual.sphere and self.click.pick.mass.pickable: # pick up the object (but only masses)
                self.dragObject = self.click.pick
                self.distance = visual.dot(self.display.forward, self.dragObject.pos)
                # save original attributes and overwrite them
                self.rememberFixed = self.dragObject.mass.fixed
                self.dragObject.mass.fixed = 1
                self.rememberColor = self.dragObject.color
                self.dragObject.color = (self.rememberColor[0] * 1.5,
                                         self.rememberColor[1] * 1.5,
                                         self.rememberColor[2] * 1.5)
        if self.dragObject:
            self.dragObject.pos = self.display.mouse.project(normal=self.display.forward, d=self.distance)

    def step(self):
        """Perform one step.  This is a convenience method.
        It actually calls dispatchDnD() and advance().
        """
        self.dispatchDnD()
        self.advance()
        visual.rate(self.rate) # best when placed after advance() and before dispatchDnD()

    def mainloop(self):
        """Start the mainloop, which means that step() is
        called in an infinite loop.
        """
        self.display.autoscale=0
        while 1:
            self.step()

    def povexport(self, filename=None, **keywords):
        """Export the system to a POV-Ray file.  If no filename
        is given, the name of the system is used.
        """
        import povexport
        povexport.export(display=self.display, filename=filename, **keywords)
