import math
import visual


class _Spring:
    """Physical model of a spring.
    """

    def __init__(self, m0, m1, k, l0=None, damping=None):
        """Construct a spring.
        """
        self.m0 = m0
        self.m1 = m1
        self.k = k
        if l0:
            self.l0 = l0
        else:
            self.l0 = visual.mag(self.m1.sphere.pos - self.m0.sphere.pos)
        self.damping = damping
        self.e = visual.vector(0., 0., 0.)

    def calcSpringForce(self):
        """Calculate the spring force.
        """
        delta = self.m1.sphere.pos - self.m0.sphere.pos
        l = visual.mag(delta)
        self.e = visual.norm(delta)
        # Fspring = (l - l0) * k
        Fspring = (l - self.l0) * self.k * self.e
        self.m0.F += Fspring
        self.m1.F -= Fspring

    def calcDampingForce(self):
        """Calculate the damping force.
        """
        # Fdamping = v in e * dampingFactor
        Fdamping = visual.dot((self.m1.v - self.m0.v), self.e) * self.damping * self.e
        self.m0.F += Fdamping
        self.m1.F -= Fdamping


class CylinderSpring(_Spring):
    """Visual representation of a spring using multiple cylinders.
    """

    def __init__(self, m0, m1, k, l0=None, damping=None, radius1=None, radius2=None, segments=2, color=(0.8, 0.8, 0.2), **keywords):
        _Spring.__init__(self, m0, m1, k, l0, damping)
        """Construct a cylinder spring.
        """
        if not radius1:
            radius1 = (self.m0.sphere.radius + self.m1.sphere.radius) * 0.2
        self.frame = visual.frame()
        self.frame.pos = self.m0.sphere.pos
        self.frame.axis = self.m1.sphere.pos - self.m0.sphere.pos
        length = visual.mag(self.frame.axis)
        dx = length / segments
        if not radius2:
            radius2 = radius1 * 0.8
        i = 0
        r = None
        self.tube = []
        for x in visual.arange(0, length - dx/2., dx):
            if i % 2 == 0:
                r = radius1
            else:
                r = radius2
            self.tube.append(visual.cylinder(frame=self.frame, pos=(x, 0, 0), axis=(dx, 0, 0), radius=r, color=color, **keywords))
            i += 1
        self.count = i

    def update(self):
        """Update the visual representation of the spring.
        """
        self.frame.pos = self.m0.sphere.pos
        self.frame.axis = self.m1.sphere.pos - self.m0.sphere.pos
        dx = visual.mag(self.frame.axis) / self.count
        for index in range(self.count):
            #self.tube[index].x = index * dx
            ## cylinder.x was buggy in previous visual modules, therefore:
            self.tube[index].pos[0] = index * dx
            self.tube[index].axis = visual.vector(dx, 0, 0)


class RubberSpring(_Spring):
    """Visual representation of a spring using a single cylinder with variable radius.
    """

    def __init__(self, m0, m1, k, l0=None, damping=None, radius=None, color=(0.8, 0.2, 0.8), **keywords):
        _Spring.__init__(self, m0, m1, k, l0, damping)
        """Construct a rubber spring.
        """
        if not radius:
            radius = (self.m0.sphere.radius + self.m1.sphere.radius) * 0.2
        self.r0 = radius
        self.cylinder = visual.cylinder(pos=self.m0.sphere.pos, axis=self.m1.sphere.pos-self.m0.sphere.pos, radius=radius, color=color, **keywords)

    def update(self):
        """Update the visual representation of the spring.
        """
        self.cylinder.pos = self.m0.sphere.pos
        self.cylinder.axis = self.m1.sphere.pos - self.m0.sphere.pos
        fact = self.l0 / visual.mag(self.cylinder.axis)
        self.cylinder.radius = self.r0 * fact
        fact = 0.5 + visual.mag(self.cylinder.axis) - self.l0
        self.cylinder.color = (fact, fact, fact)


class DoubleHelixSpring(_Spring):
    """Visual representation of a spring using a double helix.
    """

    def __init__(self, m0, m1, k, l0=None, damping=None, radius=None, coils=2, wireradius=None, spc=10, color=(0.8, 0.8, 0.2), **keywords):
        _Spring.__init__(self, m0, m1, k, l0, damping)
        """Construct a double helix spring.
        """
        if not radius:
            radius = (self.m0.sphere.radius + self.m1.sphere.radius) * 0.2
        self.frame = visual.frame()
        self.frame.pos = self.m0.sphere.pos
        self.frame.axis = self.m1.sphere.pos - self.m0.sphere.pos
        length = visual.mag(self.frame.axis)
        if not wireradius:
            wireradius = radius * 0.5
        k = coils * (2. * math.pi / length)
        dx = (length / coils) / spc
        self.helix1 = visual.curve(frame=self.frame, radius=wireradius, color=color, **keywords)
        self.helix2 = visual.curve(frame=self.frame, radius=wireradius, color=color)
        for x in visual.arange(0, length + dx/2., dx):
            self.helix1.append( pos=(x, radius*math.sin(k*x), radius*math.cos(k*x)) )
            self.helix2.append( pos=(x, -radius*math.sin(k*x), -radius*math.cos(k*x)) )
        self.count = len(self.helix1.pos)

    def update(self):
        """Update the visual representation of the spring.
        """
        self.frame.pos = self.m0.sphere.pos
        self.frame.axis = self.m1.sphere.pos - self.m0.sphere.pos
        dx = visual.mag(self.frame.axis) / (self.count-1)
        for index in range(self.count):
            self.helix1.x[index] = index * dx
            self.helix2.x[index] = index * dx


class SingleHelixSpring(_Spring):
    """Visual representation of a spring using a single helix.
    Problematic, because the up-vector is not treated at all.
    """

    def __init__(self, m0, m1, k, l0=None, damping=None, radius=None, coils=4, wireradius=None, spc=10, color=(0.8, 0.8, 0.2), **keywords):
        """Construct a single helix spring.
        """
        _Spring.__init__(self, m0, m1, k, l0, damping)
        if not radius:
            radius = (self.m0.sphere.radius + self.m1.sphere.radius) * 0.2
        self.frame = visual.frame()
        self.frame.pos = self.m0.sphere.pos
        self.frame.axis = self.m1.sphere.pos - self.m0.sphere.pos
        length = visual.mag(self.frame.axis)
        if not wireradius:
            wireradius = radius * 0.5
        k = coils * (2. * math.pi / length)
        dx = (length / coils) / spc
        self.helix = visual.curve(frame=self.frame, radius=wireradius, color=color, **keywords)
        for x in visual.arange(0, length + dx/2., dx):
            self.helix.append( pos=(x, radius*math.sin(k*x), radius*math.cos(k*x)) )
        self.count = len(self.helix.pos)

    def update(self):
        """Update the visual representation of the spring.
        """
        self.frame.pos = self.m0.sphere.pos
        self.frame.axis = self.m1.sphere.pos - self.m0.sphere.pos
        dx = visual.mag(self.frame.axis) / (self.count-1)
        for index in range(self.count):
            self.helix.x[index] = index * dx
