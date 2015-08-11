# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3
import math

class NodeFactory(pycd3.INodeFactory):
    def __init__(self, node):
        pycd3.INodeFactory.__init__(self)
        self.node = node
        print "NodeFactory.__init__"
        
    def getNodeName(self):
        print "NodeFactory.getName"
        return self.node.__name__
        
    def createNode(self):
        print "NodeFactory.createNode"
        n = self.node()
        n.__disown__()
        print "NodeFactory.disowned"
        return n
        
    def getSource(self):
        print "NodeFactory.getSource"
        return "Practice.py"

class Horton(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.rain = pycd3.Flow()
        self.evapo = pycd3.Flow()
        self.infiltr = pycd3.Flow()
        self.actuell_infiltr = pycd3.Flow()
        self.runoff = pycd3.Flow()
        #dir (self.inf)
        print "init node"
        self.addInPort("rain", self.rain)
        self.addInPort("evapo", self.evapo)
        self.addOutPort("infiltr", self.infiltr)
        self.addOutPort("actuell_infiltr", self.actuell_infiltr)
        self.addOutPort("runoff", self.runoff)
        
        self.initial_cap = pycd3.Double(0.9)
        self.addParameter("initial_cap[m/h]", self.initial_cap)
        
        self.final_cap = pycd3.Double(0.29)
        self.addParameter("final_cap[m/h]", self.final_cap)
        
        self.decay_constant = pycd3.Double(2.0)
        self.addParameter("decay_constant[1/min]", self.decay_constant)
        
        
        self.time=-1.0
        
    def init(self, start, stop, dt):
        print start
        print stop
        print dt
        return True
        
    def f(self, current, dt):
        
        
        self.time+=1.0
        self.infiltr[0] = self.final_cap/10 + (self.initial_cap/10 - self.final_cap/10) * math.exp(-1*self.decay_constant * 6 * self.time)
        
        

        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "Horton"

def register(nr):
    for c in pycd3.Node.__subclasses__():
        nf = NodeFactory(c)
        nf.__disown__()
        nr.addNodeFactory(nf)
        
# def test():
#     nr = pycd3.NodeRegistry()
#     nf = NodeFactory(Household).__disown__()
#     nr.addNodeFactory(nf)
#     node = nr.createNode("Household")
    
#test()
