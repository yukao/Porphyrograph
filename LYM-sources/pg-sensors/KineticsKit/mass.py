import math
import visual


class Mass:
    """Physical model and visual representation of a mass.
    """

    #rho = 8900 # Cu
    rho = 23.8732414637845 # for backwards compatibility
    factor = 3. / (4 * math.pi * rho)

    def __init__(self, m, pos, r=None, fixed=0, pickable=1, v=visual.vector(0., 0., 0.), color=(0.8, 0.2, 0.4), **keywords):
        """Construct a mass.
        """
        if not r:
            # rho = m / V; V = 4 * PI * r^3 / 3
            r = math.pow(Mass.factor * m, 1./3)
        self.sphere = visual.sphere(radius=r, color=color, **keywords)
        self.sphere.mass = self # necessary to access the mass in the DnD loop
        self.m = float(m)
        self.sphere.pos = visual.vector(pos)
        self.fixed = fixed
        self.pickable = pickable
        self.v = visual.vector(v)
        self.F = visual.vector(0., 0., 0.)

    def calcGravityForce(self, g):
        """Calculate the gravity force.
        """
        # gforce = m * g
        self.F -= visual.vector(0, self.m * g, 0)

    def calcViscosityForce(self, viscosity):
        """Calculate the viscosity force.
        """
        # Fviscosity = - v * viscosityFactor
        self.F -= self.v * viscosity

    def calcNewLocation(self, dt):
        """Calculate the new location of the mass.
        """
        # F = m * a = m * dv / dt  =>  dv = F * dt / m
        dv = self.F * dt / self.m
        self.v += dv
        # v = dx / dt  =>  dx = v * dt
        self.sphere.pos += self.v * dt

    def clearForce(self):
        """Clear the Force.
        """
        self.F = visual.vector(0., 0., 0.)
