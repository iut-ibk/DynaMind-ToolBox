# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3

#class NodeFactoryDistributor(pycd3.INodeFactory):
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

class Distributor(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.inflow = pycd3.Flow()
        self.numberof_out_ports = pycd3.Integer(2)
        self.addParameter("Number_of_Outports", self.numberof_out_ports)            
        
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
#        print "init node"
        
        self.addInPort("Inport", self.inflow)

        for i in range(self.numberof_out_ports):
            exec 'self.Out'+str(i+1)+'=pycd3.Flow()'
            exec 'self.addOutPort("Outport_'+str(i+1)+'", self.Out'+str(i+1)+')'        
         
        return True
        
    def f(self, current, dt):
              
        for i in range(self.numberof_out_ports):
            exec 'self.Out'+str(i+1)+'[0]=self.inflow[0]'
          
        return dt
    
    def getClassName(self):
#        print "getClassName"
        return "Distributor"

#def register(nr):
#    for c in pycd3.Node.__subclasses__():
#        nf = NodeFactoryDistributor(c)
#        nf.__disown__()
#        nr.addNodeFactory(nf)
#        
