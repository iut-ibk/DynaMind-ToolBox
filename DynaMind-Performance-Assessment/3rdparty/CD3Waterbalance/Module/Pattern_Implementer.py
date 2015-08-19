# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3
from numpy import floor
from datetime import datetime
from matplotlib.dates import num2date, date2num
import numpy as np
from numpy import arange, mean, asarray, pi, size

#class NodeFactoryPatternImplementer(pycd3.INodeFactory):
#    def __init__(self, node):
#        pycd3.INodeFactory.__init__(self)
#        self.node = node
#        print "NodeFactory.__init__"
#        
#    def getNodeName(self):
#        print "NodeFactory.getName"
#        return self.node.__name__
#        
#    def createNode(self):
#        print "NodeFactory.createNode"
#        n = self.node()
#        n.__disown__()
#        print "NodeFactory.disowned"
#        return n
#        
#    def getSource(self):
#        print "NodeFactory.getSource"
#        return "Addons.py"

class Pattern_Impl(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.input = pycd3.Flow()
        self.output = pycd3.Flow()
#        print "init node"
        self.addInPort("Inport", self.input)
        self.addOutPort("Outport", self.output)
        
        self.zenith = pycd3.Double(13)
        self.addParameter("Sun_Zenith_[0-23h]", self.zenith)
        self.sundown = pycd3.Double(20.5)
        self.addParameter("Sundown_[0-23h]", self.sundown)
        
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
        #print type(start)
        #print start.to_datetime()
        #print date2num(datetime.strptime(str(start.to_datetime()),"%Y-%m-%d %H:%M:%S"))
        if dt <=3600:
            #creating gauss curve with the expectation value self.zenith and the sundown marks the value where the evapotr. is almost 0.0
            self.deviation = (self.sundown - self.zenith)/3./24.      
            self.xtime=arange(0,1.0-dt/24./3600.,dt/24./3600.)
            self.gauss_curve = 1/(self.deviation*np.sqrt(2*pi))*np.exp(-1/2.*((self.xtime-self.zenith/24.)/self.deviation)**2)
            #creating factor for implementation of pattern
            self.factor =self.gauss_curve/mean(self.gauss_curve)
            #pattern in [time,factor] configuration
            self.pattern=asarray([self.xtime,self.factor]).transpose().tolist()
            self.pattern.append([1.0, self.pattern[0][1]])        
            # getting the starting time of simulation
            self.time = date2num(datetime.strptime(str(start.to_datetime()),"%Y-%m-%d %H:%M:%S"))
        else:
            pass
        
        return True
        
    def f(self, current, dt):
        if dt <=3600:
            #looks for the right factor and mulitplies it with value at a certain point of time
            if self.time - floor(self.time) == 1.0:
                self.output[0]=self.input[0]*self.pattern[-1][1]  
            else:
                count_i = 0
                while (self.time - floor(self.time) > self.pattern[count_i][0]):
                    count_i+=1
                self.output[0]=self.input[0]*self.pattern[count_i][1]
                
            self.time+=dt/24./3600.
        else:
            self.output[0] = self.input[0]
        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "Pattern_Impl"

#def register(nr):
#    for c in pycd3.Node.__subclasses__():
#        nf = NodeFactoryPatternImplementer(c)
#        nf.__disown__()
#        nr.addNodeFactory(nf)
        
# def test():
#     nr = pycd3.NodeRegistry()
#     nf = NodeFactory(Household).__disown__()
#     nr.addNodeFactory(nf)
#     node = nr.createNode("Household")
    
#test()
