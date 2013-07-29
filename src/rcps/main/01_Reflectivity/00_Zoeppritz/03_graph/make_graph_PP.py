#!/usr/bin/env python
 
import numpy
import rsf.api as rsf
import sys, pylab
import matplotlib.pyplot as plt



# Zoeppritz data
input      = rsf.Input('RPPtrace.rsf')
n1         = input.int("n1")
Zoepp_data  = numpy.zeros((n1,1),'f')
input.read(Zoepp_data)

# Approximation data
Approx_data = numpy.zeros((40,4))
Approx_data = numpy.genfromtxt('/home/gino/RCPsuite/data/01_Reflectivity/Approx_RPP_RPS_g3.asc')

# Graph settings
x   =  numpy.arange(0.,80.,2.)
y1  =  Zoepp_data
y2  =  abs(Approx_data[:,2])
y3  =  Approx_data[:,2]

# Graph plotting
la = plt.plot(x,y1,'b-',label='Zoeppritz RPP')
lb = plt.plot(x,y2,'r--',label='abs(RPP Approx)')
lc = plt.plot(x,y3,'g--',label='RPP Approx')
ax = plt.axis([0, 50, -1.0, 1.0])
grd = plt.grid(True)
ll = plt.legend(loc='upper left')
lx = plt.xlabel('Incidence angle [degrees]')
ly = plt.ylabel('Amplitude')
ttl = plt.title('Fatti Approximation X Zoeppritz')
plt.show()


