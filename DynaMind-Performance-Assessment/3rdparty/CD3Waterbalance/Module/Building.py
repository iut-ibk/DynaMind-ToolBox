# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3

#class NodeFactoryHousehold(pycd3.INodeFactory):
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

class Building(pycd3.Node):
    def __init__(self):
#        print "init start hh"
        pycd3.Node.__init__(self)
        
        self.black_w = pycd3.Flow()
        self.grey_w = pycd3.Flow()
        self.pot_w = pycd3.Flow()
        self.nonpot_w = pycd3.Flow()
        self.outdoor_demand = pycd3.Flow()
        self.bath_tub = pycd3.Flow()
        self.shower = pycd3.Flow()
        self.toilet = pycd3.Flow()
        self.kitchen_tap = pycd3.Flow()
        self.handbasin_tap = pycd3.Flow()
        self.washing_machine = pycd3.Flow()
        self.dishwasher = pycd3.Flow() 
        self.evapcooler = pycd3.Flow() 
        #self.decision = pycd3.String("")
        
        self.addOutPort("Blackwater", self.black_w)
        self.addOutPort("Greywater", self.grey_w)
        self.addOutPort("Potable_Demand", self.pot_w)
        self.addOutPort("Non_Potable_Demand", self.nonpot_w)
        self.addInPort("Outdoor_Demand", self.outdoor_demand)
        self.addInPort("Bathtub", self.bath_tub)
        self.addInPort("Shower", self.shower)
        self.addInPort("Toilet", self.toilet)
        self.addInPort("Kitchen_Tap", self.kitchen_tap)
        self.addInPort("Handbasin_Tap", self.handbasin_tap)
        self.addInPort("Washing_Machine", self.washing_machine)
        self.addInPort("Dishwasher", self.dishwasher)
        self.addInPort("Evapcooler", self.evapcooler)   
#        
#        print "init node"
        
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
        return True
        
    def f(self, current, dt):
        
        self.pot_w[0] = (self.bath_tub[0]+self.shower[0]+self.kitchen_tap[0]+self.handbasin_tap[0]+self.dishwasher[0]+self.washing_machine[0] + self.evapcooler[0])
        self.nonpot_w[0] = (self.toilet[0])+self.outdoor_demand[0]
        self.black_w[0] = (self.kitchen_tap[0]+self.toilet[0])
        self.grey_w[0] = (self.bath_tub[0]+self.shower[0]+self.handbasin_tap[0]+self.washing_machine[0]+self.dishwasher[0] + self.evapcooler[0])

        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "Building"

#def register(nr):
#    for c in pycd3.Node.__subclasses__():
#        nf = NodeFactoryHousehold(c)
#        nf.__disown__()
#        nr.addNodeFactory(nf)
#        
# def test():
#     nr = pycd3.NodeRegistry()
#     nf = NodeFactory(Building).__disown__()
#     nr.addNodeFactory(nf)
#     node = nr.createNode("Building")
    
#test()
