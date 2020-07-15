"""
   Modified May 4, 2000: swapped order of self/other in cross product
   Modified Aug 31 1999:
     removed dependence on NumPy to facilitate port to JPython
     added matrixmultiply class to replace NumPy's
   Modified Aug 29 1999:
     fixed bug in cross product method (a term had wrong sign)
     add rounding to angle method to prevent Math.acos overflow
   Modified Aug 28 1999:
     changed polar format:  reversed angles to conform to textbook norm
     added rotation about arbitrary vector axis (rotaxis method)
     Incorporated Tom Ace's normalization to 0 and quadray matrix,
     dot product, methods, plus rewrote length() as per Tom's norm0()
"""

import math

root2 = 2.0**0.5
deg2rad = math.pi/180
rad2deg = 180/math.pi

class Vector:

   def __init__(self, quad=(), xyz=(), polar=()):
       # constructor accepts 4-tuple/list or xyz 3-tuple/list
       if len(quad)==4:  self.data=tuple(self.norm(quad))
       if len(xyz)==3:   self.data=self.setxyz(xyz)
       if len(polar)==3: self.data=self.setpolar(polar)

   def __add__(self,pVector):
       # add parameter quadray to self, return new quadray
       if type(pVector)==type(self):
           outlist = []
           for i in range(4): outlist.append(self.data[i]+pVector.data[i])
       else: outlist = self.data
       return Vector(quad=outlist)

   def __sub__(self,pVector):
       # subtract parameter quadray from self, return new quadray
       if type(pVector)==type(self):
           outlist = []
           for i in range(4): outlist.append(self.data[i]-pVector.data[i])
       else: outlist = self.data
       return Vector(quad=outlist)

   def __mul__(self,scalar):
       outlist=[]
       for i in range(4): outlist.append(scalar*self.data[i])
       return Vector(quad=outlist)

   __rmul__ = __mul__

   def __div__(self,scalar):
       return self.__mul__(1/scalar)

   def __neg__(self):
       outlist=[]
       for i in range(4): outlist.append(-self.data[i])
       return Vector(quad=outlist)

   def __repr__(self):
       return self.__class__.__name__ + str(self.data)

   def length(self):
       # using normalization around 0
       tmp  = map((lambda x: x**2),self.norm0())
       return root2 * reduce(lambda x,y: x+y, tmp)**0.5

   def length1(self):
       # using native normalization (lowest is 0)
       a=self.data[0]
       b=self.data[1]
       c=self.data[2]
       d=self.data[3]
       return ((3.0/2.0)*(a*a+b*b+c*c+d*d)-(a*b+a*c+a*d+b*d+c*d))**0.5

   def quad(self):
       return self.data

   def norm(self,plist):
       if type(plist)==type(()): plist = list(plist)
       outlist=map(lambda x, y=min(plist): x-y, plist)
       return tuple(outlist)

   def norm0(self):
       # k = (a+b+c+d)/4
       k=reduce(lambda x,y: x+y, self.data)/4.0
       outlist=map(lambda x, y=k: x-y, self.data)
       return tuple(outlist)

   def norm1(self):
       outlist = map(lambda x, y=0.25: x+y, self.norm0())
       return tuple(outlist)

   def xyz(self):
       s = self.data
       x = (1/root2) * (s[0] - s[1] - s[2] + s[3])
       y = (1/root2) * (s[0] - s[1] + s[2] - s[3])
       z = (1/root2) * (s[0] + s[1] - s[2] - s[3])
       return (x,y,z)

   def polar(self):
       x,y,z  = self.xyz()
       radius = (x*x + y*y + z*z) ** 0.5
       theta = alpha = 0

       if x!=0:
          if x>0: theta = math.atan(y/x)
          else:   theta = math.atan(y/x) + math.pi
       else:
          if y!=0:
             if y>0: theta = math.pi/2
             else:   theta = -math.pi/2

       if z!=0:
          xydist = math.hypot(x,y)
          if xydist<>0.0:
             alpha  = math.atan(z/xydist)
          elif z>0: alpha = math.pi/2
          elif z<0: alpha = -math.pi/2

       return tuple(map(lambda x: round(x,8),[radius,alpha*rad2deg,theta*rad2deg]))

   def setxyz(self,pdata=(0,0,0)):
       x=pdata[0]; y=pdata[1]; z=pdata[2]
       a = (1/root2) * ((x>=0)*x   + (y>=0)*y   + (z>=0)*z)
       b = (1/root2) * ((x<0)*(-x) + (y<0)*(-y) + (z>=0)*z)
       c = (1/root2) * ((x<0)*(-x) + (y>=0)*y   + (z<0)*(-z))
       d = (1/root2) * ((x>=0)*x   + (y<0)*(-y) + (z<0)*(-z))
       return tuple(self.norm([a,b,c,d]))

   def setpolar(self,pdata=(0,0,0)):
       radius = pdata[0]
       alpha  = pdata[1]*deg2rad
       theta  = pdata[2]*deg2rad
       xydist = radius * math.cos(alpha)
       x = xydist * math.cos(theta)
       y = xydist * math.sin(theta)
       z = radius * math.sin(alpha)
       x,y,z=map(lambda x: round(x,8),[x,y,z])
       return self.setxyz((x,y,z))

   def rotate(self,plist):
       if len(plist)==3:
           self.newdata=self.xyz()
           if plist[0]<>0: self.rotx(plist[0])
           if plist[1]<>0: self.roty(plist[1])
           if plist[2]<>0: self.rotz(plist[2])
           return Vector(xyz=self.newdata)
       if len(plist)==4:
           self.newdata=self.data
           if plist[0]<>0: self.rota(plist[0])
           if plist[1]<>0: self.rotb(plist[1])
           if plist[2]<>0: self.rotc(plist[2])
           if plist[3]<>0: self.rotd(plist[3])
           return Vector(quad=self.newdata)

   def rotaxis(self,pvector,deg):
       alpha = pvector.polar()[1]
       beta  = pvector.polar()[2]
       newvector = self.rotate((0,0,-beta)).rotate((0,-alpha,0))
       newvector = newvector.rotate((deg,0,0))
       newvector = newvector.rotate((0,alpha,0)).rotate((0,0,beta))
       return newvector

   def rotx(self,deg):
       rad = deg*deg2rad
       newy=math.cos(rad)*self.newdata[1] - math.sin(rad)*self.newdata[2]
       newz=math.sin(rad)*self.newdata[1] + math.cos(rad)*self.newdata[2]
       self.newdata=map(lambda x: round(x,8),(self.newdata[0],newy,newz))

   def roty(self,deg):
       rad = deg*deg2rad
       newx=math.cos(rad)*self.newdata[0] - math.sin(rad)*self.newdata[2]
       newz=math.sin(rad)*self.newdata[0] + math.cos(rad)*self.newdata[2]
       self.newdata=map(lambda x: round(x,8),(newx,self.newdata[1],newz))

   def rotz(self,deg):
       rad = deg*deg2rad
       newx=math.cos(rad)*self.newdata[0] - math.sin(rad)*self.newdata[1]
       newy=math.sin(rad)*self.newdata[0] + math.cos(rad)*self.newdata[1]
       self.newdata=map(lambda x: round(x,8),(newx,newy,self.newdata[2]))

   def rota(self,deg):
       self.rotconst(deg)
       m = ([1,     0,     0,     0],
            [0,self.F,self.H,self.G],
            [0,self.G,self.F,self.H],
            [0,self.H,self.G,self.F])
       self.newdata = map(lambda x: round(x,8),self.matrixmultiply(m,self.newdata))

   def rotb(self,deg):
       self.rotconst(deg)
       m = ([self.F,0,self.H,self.G],
            [0     ,1,     0,     0],
            [self.G,0,self.F,self.H],
            [self.H,0,self.G,self.F])
       self.newdata = map(lambda x: round(x,8),self.matrixmultiply(m,self.newdata))

   def rotc(self,deg):
       self.rotconst(deg)
       m = ([self.F,self.H,0,self.G],
            [self.G,self.F,0,self.H],
            [0   ,       0,1,0],
            [self.H,self.G,0,self.F])
       self.newdata = map(lambda x: round(x,8),self.matrixmultiply(m,self.newdata))

   def rotd(self,deg):
       self.rotconst(deg)
       m = ([self.F,self.H,self.G,0],
            [self.G,self.F,self.H,0],
            [self.H,self.G,self.F,0],
            [0     ,     0,     0,1])
       self.newdata = map(lambda x: round(x,8),self.matrixmultiply(m,self.newdata))

   def rotconst(self,deg):
       rad = deg*deg2rad
       self.F = (2*math.cos(rad)+1)/3.0
       self.G = (2*math.cos(rad-120*deg2rad)+1)/3.0
       self.H = (2*math.cos(rad+120*deg2rad)+1)/3.0

   def dot(self,vector):
       result = 0
       v1 = self.norm0()
       v2 = vector.norm0()
       for i in range(4): result = result + v1[i]*v2[i]
       return round(2*result,15)

   def angle(self,vector):
       costheta = round(self.dot(vector)/(self.length() * vector.length()),8)
       theta = math.acos(costheta) * rad2deg
       return round(theta,5)


   def cross(self,vector):
       A=Vector(quad=(1,0,0,0))
       B=Vector(quad=(0,1,0,0))
       C=Vector(quad=(0,0,1,0))
       D=Vector(quad=(0,0,0,1))
       a1,b1,c1,d1 = vector.data
       a2,b2,c2,d2 = self.data
       k= (2.0**0.5)/2.0
       sum =   (A*c1*d2 - A*d1*c2 - A*b1*d2 + A*b1*c2
              + A*b2*d1 - A*b2*c1 - B*c1*d2 + B*d1*c2
              + b1*C*d2 - b1*D*c2 - b2*C*d1 + b2*D*c1
              + a1*B*d2 - a1*B*c2 - a1*C*d2 + a1*D*c2
              + a1*b2*C - a1*b2*D - a2*B*d1 + a2*B*c1
              + a2*C*d1 - a2*D*c1 - a2*b1*C + a2*b1*D)
       return k*sum

   def matrixmultiply(self,m1,m2):
       outdata=[]
       for row in m1:
           term = 0.0
           for j in range(len(row)):
              term = term + row[j]*m2[j]
           outdata.append(term)
       return outdata
