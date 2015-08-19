
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3
from datetime import datetime
from matplotlib.dates import date2num
import random
from math import ceil,floor


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

class GardenWateringModel(pycd3.Node):
    def __init__(self):
#        print "init start hh"
        pycd3.Node.__init__(self)
        
        self.inport = pycd3.Flow()
        self.outport = pycd3.Flow()
        self.currentOutdoorDemand = pycd3.Flow()
        self.average = pycd3.Double(7)
        self.deviation = pycd3.Double(2)
        self.tap_flow_rate = pycd3.Double(18)
        self.smart_watering_time = pycd3.String("[18,6]")
        self.watering_method = pycd3.String('Smart_Watering')
        
        self.addInPort("Outdoor_Demand_In", self.inport)   
        self.addOutPort("Outdoor_Demand_Out", self.outport)
        self.addOutPort("Check_Storage", self.currentOutdoorDemand)
        self.addParameter("Watering_Method_(Normal_Watering_or_Smart_Watering)", self.watering_method)
        self.addParameter("Smart_Watering_Start_Time_End_Time_[hh,hh]", self.smart_watering_time)
        self.addParameter("Maximal_Watering_Flow_Rate_[l/min]", self.tap_flow_rate)
        self.addParameter("Average_Watering_Frequency_[d]", self.average)   
        self.addParameter("Deviation_of_Frequency_[d]", self.deviation)
        
        '''
        self.smart_watering_time: 0h not valid, has got to 24 h, start time smaller 24!
        '''
#        print "init node"
        
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
        
        #converting inputstrings to lists
        self.smart_watering_time = str(self.smart_watering_time)[1:-1].split(',')
        for i in range(len(self.smart_watering_time)):
            self.smart_watering_time[i] = float(self.smart_watering_time[i])/24.
        
        self.DemandStorage = 0.0
        self.average_watering_frequency_building = abs(random.gauss(self.average, self.deviation))
        self.to_add = self.average_watering_frequency_building
        self.average_watering_frequency_building =self.average_watering_frequency_building - random.randrange(ceil(self.average_watering_frequency_building))
        
        if self.watering_method == "Smart_Watering":
            self.decimals = self.average_watering_frequency_building-floor(self.average_watering_frequency_building)
            if self.decimals > self.smart_watering_time[0] or self.decimals < self.smart_watering_time[1] :
                pass
            else:
                self.average_watering_frequency_building = floor(self.average_watering_frequency_building) + random.gauss(self.smart_watering_time[0], 1/24.)
        elif self.watering_method == "Normal_Watering":
            pass
        else:
            print "Non valid Watering Method!"        
        
        self.time = date2num(datetime.strptime(str(start.to_datetime()),"%Y-%m-%d %H:%M:%S")) - floor(date2num(datetime.strptime(str(start.to_datetime()),"%Y-%m-%d %H:%M:%S")))
        self.watering_volume_per_dt = self.tap_flow_rate/1000/60*dt 
        self.switch = 0
        
        return True
        
    def f(self, current, dt):
        
        self.DemandStorage += self.inport[0]
        
        if self.time >= self.average_watering_frequency_building:
            self.average_watering_frequency_building += self.to_add
            if self.watering_method == "Smart_Watering":
                self.decimals = self.average_watering_frequency_building-floor(self.average_watering_frequency_building)
                if self.decimals > self.smart_watering_time[0] or self.decimals < self.smart_watering_time[1] :
                    pass
                else:
                    self.average_watering_frequency_building = floor(self.average_watering_frequency_building) + random.gauss(self.smart_watering_time[0], 1/24.)
            elif self.watering_method == "Normal_Watering":
                pass
            else:
                print "Non valid Watering Method!"
            self.switch = 1
        else:
            pass

        if self.switch == 1:
            if self.DemandStorage > self.watering_volume_per_dt:
                self.DemandStorage -= self.watering_volume_per_dt
                self.watering = self.watering_volume_per_dt
            elif self.DemandStorage == self.watering_volume_per_dt:
                self.DemandStorage -= self.watering_volume_per_dt
                self.watering = self.watering_volume_per_dt
                self.switch = 0
            else:
                self.watering = self.DemandStorage
                self.DemandStorage = 0
                self.switch = 0
        else:
            self.watering = 0.0

        self.outport[0] = self.watering
        self.currentOutdoorDemand[0]= self.DemandStorage
        self.time += dt/3600./24.
        
        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "GardenWateringModel"

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
