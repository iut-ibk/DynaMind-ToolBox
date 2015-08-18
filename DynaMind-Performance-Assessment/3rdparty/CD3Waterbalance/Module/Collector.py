# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3

#class NodeFactoryCollector(pycd3.INodeFactory):
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

class Collector(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.outflow = pycd3.Flow()
        self.numberof_in_ports = pycd3.Integer(2)
        self.addParameter("Number_of_Inports", self.numberof_in_ports)            
        
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
#        print "init node"
        
        for i in range(self.numberof_in_ports):
            exec 'self.Inport'+str(i+1)+'=pycd3.Flow()'
            exec 'self.addInPort("Inport_'+str(i+1)+'", self.Inport'+str(i+1)+')'        
        
        self.addOutPort("Outport", self.outflow)
        
        return True
        
    def f(self, current, dt):
        
        self.memory = 0.0     
        
        for i in range(self.numberof_in_ports):
            exec 'self.memory += self.Inport'+str(i+1)+'[0]'
          
        self.outflow[0] = self.memory
        
        return dt
    
    def getClassName(self):
#        print "getClassName"
        return "Collector"

#def register(nr):
#    for c in pycd3.Node.__subclasses__():
#        nf = NodeFactoryCollector(c)
#        nf.__disown__()
#        nr.addNodeFactory(nf)
#        
