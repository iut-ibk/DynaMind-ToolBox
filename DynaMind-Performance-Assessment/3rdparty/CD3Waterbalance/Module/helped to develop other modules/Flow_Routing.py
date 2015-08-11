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

class Muskingum(pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.rain = pycd3.Flow()
        self.runoff = pycd3.Flow()
        self.inflow = pycd3.Flow()
       
        #dir (self.inf)
        print "init node"
        self.addInPort("rain", self.rain)
        self.addInPort("inflow", self.inflow)
        self.addOutPort("runoff", self.runoff)
   
   
        #Catchment area + fraction info of pervious and impervious parts
        self.area_width = pycd3.Double(10)
        self.addParameter("area_width [m]", self.area_width)
        self.area_length = pycd3.Double(100)
        self.addParameter("area_length [m]", self.area_length)
        self.perv_area = pycd3.Double(0.4)
        self.addParameter("perv_area [-]", self.perv_area)
        self.imp_area_stormwater = pycd3.Double(0.4)
        self.addParameter("imp_area_stormwater [-]", self.imp_area_stormwater)
        self.imp_area_raintank = pycd3.Double(1)
        self.addParameter("imp_area_raintank [-]", self.imp_area_raintank)
        
        #number of subareas for flowconcentration
        self.amount_subareas = pycd3.Double(1)
        self.addParameter("amount_subareas [-]", self.amount_subareas)
        
        #Muskingum parameters K flowtime for entire catchment
        self.muskingum_veloc = pycd3.Double(0.4)
        self.addParameter("muskingum_vel [m/s]", self.muskingum_veloc)
        self.muskingum_X = pycd3.Double(0.07)
        self.addParameter("muskingum_X [-]", self.muskingum_X)
            
    def init(self, start, stop, dt):
        print start
        print stop
        print dt
        
        #calculation catchment area
        self.area_property = self.area_length * self.area_width
        
        #calculating the K values for a single subreach
        self.muskingum_K_single_subreach = (self.area_length/self.amount_subareas)/self.muskingum_veloc
        
        #calculating the Muskingum coefficients
        self.C_x=(dt/2-self.muskingum_K_single_subreach*self.muskingum_X)/(dt/2+self.muskingum_K_single_subreach*(1-self.muskingum_X))
        self.C_y=(1/(dt/2+self.muskingum_K_single_subreach*(1-self.muskingum_X)))
        
        #preparing the storage coefficients for the stored Volume in each subreach
        self.Q_i_storage_1 = []
        self.Q_i_storage_2 = []
        for i in range(self.amount_subareas):
            self.Q_i_storage_1.append(0)
            self.Q_i_storage_2.append(0)
        
        return True
        
    def f(self, current, dt):
        
        #dividing are in 'amout_subareas' parts (same size)
        self.subarea_size = self.area_property*self.imp_area_raintank/self.amount_subareas
        
        #preparing the flow array 
        self.Q_i = []
        for i in range(self.amount_subareas):
            self.Q_i.append(0)
            
            #calculating the flow in for each subreach
            if i==0:
               self.Q_i[i]=(self.inflow[0]*1000+self.rain[0]*self.subarea_size)*self.C_x+self.Q_i_storage_2[i]*self.C_y
               self.Q_i_storage_2[i]=self.Q_i[i]*(1-self.C_x)*dt+self.Q_i_storage_1[i]*(1-self.C_y*dt)
               self.Q_i_storage_1[i]=self.Q_i_storage_2[i]
            else:
                self.Q_i[i]=(self.Q_i[i-1]+self.rain[0]*self.subarea_size)*self.C_x+self.Q_i_storage_2[i]*self.C_y
                self.Q_i_storage_2[i]=self.Q_i[i]*(1-self.C_x)*dt+self.Q_i_storage_1[i]*(1-self.C_y*dt)
                self.Q_i_storage_1[i]=self.Q_i_storage_2[i]
        
        #represents the inflow in knot
        self.runoff[0]=self.Q_i[-1] /1000 
       
        

        return dt
    
    def getClassName(self):
        #print "getClassName"
        return "Muskingum"

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
