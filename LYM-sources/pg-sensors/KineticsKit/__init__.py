"""Package containing some classes to simulate the behaviour
of a 3D mass-spring-system.
"""

__all__ = ['System',
           'Mass',
           'CylinderSpring',
           'RubberSpring',
           'DoubleHelixSpring',
           'SingleHelixSpring']

# for convenience
from system import System
from mass import Mass
from springs import CylinderSpring, RubberSpring, DoubleHelixSpring, SingleHelixSpring
