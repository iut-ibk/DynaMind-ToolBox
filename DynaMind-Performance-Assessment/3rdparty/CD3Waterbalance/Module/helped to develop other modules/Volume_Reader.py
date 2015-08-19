# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3

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

class Demand_Model (pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)

        self.Flow1 = pycd3.Flow()
        self.Flow1 = pycd3.Flow()
        self.Flow1 = pycd3.Flow()
        
        # Inport and Outports
        print "init node"

        self.addOutPort("out", self.out)
        

        
    def init(self, start, stop, dt):
        print start
        print stop
        print dt
        
      
        return True
        
    def f(self, current, dt):
        
        Outport=
        

        return dt
    
    def getClassName(self):
        print "getClassName"
        return "Demand_Model"

def register(nr):
    for c in pycd3.Node.__subclasses__():
        nf = NodeFactory(c)
        nf.__disown__()
        nr.addNodeFactory(nf)
        
