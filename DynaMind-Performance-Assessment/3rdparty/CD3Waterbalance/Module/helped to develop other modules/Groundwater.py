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

class Groundwater(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.groundw_in = pycd3.Flow()
        #self.groundw_evapo = pycd3.Flow()
        self.outdoor_use = pycd3.Flow()
        #self.pF_Value = pycd3.Double()
        self.check_storage = pycd3.Flow()
        #dir (self.inf)
        print "init node"
        self.addInPort("groundw_in", self.groundw_in)
        #self.addInPort("groundw_evapo", self.groundw_evapo)
        #self.addOutPort("pF-Value", self.pF_Value)
        self.addOutPort("outdoor_use", self.outdoor_use)
        self.addOutPort("check_storage", self.check_storage)
        
        
        self.area_property = pycd3.Double(1.0)
        self.addParameter("area_property", self.area_property)
        self.evapotr = pycd3.Double(0.00002113)
        self.addParameter("evapotr [m/(6 min)]", self.evapotr)
        
        #self.water_content = pycd3.Double(0.4)
        #self.addParameter("initial_w_content", self.water_content)
        
        #self.soil_type = pycd3.String()
        #self.addParameter("soil_type", self.soil_type)
        
        self.current_Volume= 0.0
        #total_volume = 15.5*10**6*10
        
        #self.addOutPort("gw", self.gw)
        #self.addInPort("in", self.inf)
        
    def init(self, start, stop, dt):
        print start
        print stop
        print dt
        return True
        
    def f(self, current, dt):
        self.current_Volume += self.groundw_in[0]-self.evapotr
        if self.current_Volume < 0.0:
            self.outdoor_use[0] = self.current_Volume*self.area_property
            self.current_Volume = 0.0
            self.check_storage[0] = self.current_Volume
        elif self.current_Volume < 0.1:            
            self.outdoor_use[0] = 0.0
            self.check_storage[0] = self.current_Volume
        else:
            self.current_Volume = 0.1
            self.outdoor_use[0] = 0.0
            self.check_storage[0] = self.current_Volume

        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "Groundwater"

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
