# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3

#class NodeFactoryRaintank(pycd3.INodeFactory):
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

class Greywaterreservoir(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.greywaterin = pycd3.Flow()
        self.greywaterout = pycd3.Flow()
        self.check_storage = pycd3.Flow()
        self.Additional_Demand = pycd3.Flow()
        self.wastewater = pycd3.Flow()
        self.greywateroverflow = pycd3.Flow()
        
        #dir (self.inf)
#        print "init node"
        self.addInPort("Greywater_In", self.greywaterin)
        self.addInPort("Greywater_Out", self.greywaterout)
        self.addOutPort("Current_Volume",self.check_storage)
        self.addOutPort("Additional_Demand",self.Additional_Demand)
        self.addOutPort("Wastewater",self.wastewater)
        self.addOutPort("Greywater_Overflow",self.greywateroverflow)
        self.myyield = pycd3.Double(0.9)
        self.addParameter("Yield_of_Treatment_[-]", self.myyield)
        self.storage_v = pycd3.Double(40.0)
        self.addParameter("Storage_Volume_[m^3]", self.storage_v)
        self.current_volume = 0.0
        self.rest = 0.0
        self.rest2 = 0.0
     
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
        return True
        
    def f(self, current, dt):
        
        self.current_volume_t_minus_one = self.current_volume 
        self.current_volume += self.greywaterin[0]*self.myyield - self.greywaterout[0]
              
        if self.current_volume >= self.storage_v:
            if self.current_volume_t_minus_one < self.storage_v:
                self.rest =self.storage_v - self.current_volume_t_minus_one
            else:
                self.rest = 0.0
        elif self.current_volume <= 0.0:
            if self.current_volume_t_minus_one > 0.0:
                self.rest2 = self.current_volume_t_minus_one
            else:
                self.rest2 = 0.0
        else:
            self.rest2 = 0.0
            self.rest = 0.0
     
        if self.current_volume >= self.storage_v:
            self.Additional_Demand[0] = 0.0
            self.current_volume = self.storage_v
            self.wastewater[0] = self.greywaterin[0]*(1-self.myyield)
            self.greywateroverflow[0] = self.greywaterin[0]*self.myyield - self.greywaterout[0] - self.rest
        else:    
            
            if self.current_volume >= 0:
                self.Additional_Demand[0] = 0.0
                self.wastewater[0] = self.greywaterin[0]*(1-self.myyield)
            
            else:
                self.Additional_Demand[0] = self.greywaterout[0] - self.greywaterin[0]*self.myyield -self.rest2
                self.current_volume = 0.0
                self.wastewater[0] = self.greywaterin[0]*(1-self.myyield) 
       
            self.greywateroverflow[0] = 0.0
        self.check_storage[0] = self.current_volume
        
        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "Greywaterreservoir"

#def register(nr):
#    for c in pycd3.Node.__subclasses__():
#        nf = NodeFactoryRaintank(c)
#        nf.__disown__()
#        nr.addNodeFactory(nf)
        
# def test():
#     nr = pycd3.NodeRegistry()
#     nf = NodeFactory(Household).__disown__()
#     nr.addNodeFactory(nf)
#     node = nr.createNode("Household")
    
#test()
