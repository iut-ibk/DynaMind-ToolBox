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

class Stormwaterreservoir(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.stormwaterin = pycd3.Flow()
        self.stormwaterout = pycd3.Flow()
        self.check_storage = pycd3.Flow()
        self.Additional_Demand = pycd3.Flow()
        self.wastewater = pycd3.Flow()
        self.overflow = pycd3.Flow()
         
        #dir (self.inf)
#        print "init node"
        self.addInPort("Stormwater_In", self.stormwaterin)
        self.addInPort("Stormwater_Out", self.stormwaterout)
        self.addOutPort("Current_Volume",self.check_storage)
        self.addOutPort("Additional_Demand",self.Additional_Demand)
        self.addOutPort("Wastewater",self.wastewater)
        self.addOutPort("Overflow",self.overflow)
        self.myyield = pycd3.Double(0.9)
        self.addParameter("Yield_of_Treatment [-]", self.myyield)
        self.storage_volume = pycd3.Double(100)
        self.addParameter("Storage_Volume_[m^3]", self.storage_volume)
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
        self.current_volume += self.stormwaterin[0]*self.myyield - self.stormwaterout[0]
              
        if self.current_volume >= self.storage_volume:
            if self.current_volume_t_minus_one < self.storage_volume:
                self.rest =self.storage_volume - self.current_volume_t_minus_one
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
     
        if self.current_volume >= self.storage_volume:
            self.Additional_Demand[0] = 0.0
            self.current_volume = self.storage_volume
            self.wastewater[0] = self.stormwaterin[0]*(1-self.myyield)
            self.overflow[0] = self.stormwaterin[0]*self.myyield - self.stormwaterout[0] - self.rest
        else:    
            if self.current_volume >= 0:
                self.Additional_Demand[0] = 0.0
                self.wastewater[0] = self.stormwaterin[0]*(1-self.myyield)
            
            else:
                self.Additional_Demand[0] = self.stormwaterout[0] - self.stormwaterin[0]*self.myyield -self.rest2
                self.current_volume = 0.0
                self.wastewater[0] = self.stormwaterin[0]*(1-self.myyield) 
       
            self.overflow[0] = 0.0
            
        self.check_storage[0] = self.current_volume
        
        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "Stormwaterreservoir"

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
