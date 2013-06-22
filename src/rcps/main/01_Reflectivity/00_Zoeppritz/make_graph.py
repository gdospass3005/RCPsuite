#!/usr/bin/env python
 
import numpy
import rsf.api as rsf
import sys, pylab
import matplotlib.pyplot as plt


input  = rsf.Input('RPPtrace.rsf')
assert 'float' == input.type
 
n1 = input.int("n1")
assert n1

data = numpy.zeros((n1,1),'f')
input.read(data)

fdata = numpy.zeros((40,4))

fdata = numpy.genfromtxt('/home/gino/02_DATA/01_Reflectivity/output.txt')

x = numpy.arange(0.,80.,2.)
y1 = data
y2 = -fdata[:,2]

la = plt.plot(x,y1)
lb = plt.plot(x,y2)
plt.show()


