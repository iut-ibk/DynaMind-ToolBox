# -*- coding: utf-8 -*-
"""
Created on Thu Oct 02 08:41:08 2014

@author: Acer
"""

import sys
import pycd3
import csv
from datetime import datetime
from matplotlib.dates import date2num
from scipy.interpolate import interp1d
from numpy.core.fromnumeric import around
from numpy import floor, ceil, arange


#class NodeFactoryFilereader(pycd3.INodeFactory):
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

class File_Reader (pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        self.inflow = pycd3.String("")
        self.decision = pycd3.String("H")
        self.out = pycd3.Flow()
        
#        print "init node"
        self.addParameter("", self.inflow)
        self.addParameter("Type_H_for_height_[mm]_or_F_for_flow_[l/h]", self.decision)
        self.addOutPort("Outport", self.out)
        
        self.growing_t = 0.0
        self.row_to_get = 0
        self.interp_counter = 0
        self.remember_line = 0.0
        self.decimals = 0.0
        self.rest = 0.0
        self.sum_decimals = 0.0
        
        
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
        
        #reading the file
        csv_file = open(str(self.inflow), "r")       
        self.data = csv.reader(csv_file, delimiter='\t')  
        self.mylist = list(self.data) 
        
        #getting the files time step and setting the models starting time
        self.dt_read = abs(date2num(datetime.strptime(self.mylist[1][0]+" "+ self.mylist[1][1],"%d.%m.%Y %H:%M:%S")) - date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S")))
        self.growing_t = date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S"))
        #print [date2num(datetime.strptime(str(start.to_datetime()),"%Y-%m-%d %H:%M:%S")),self.growing_t ]
        #printing the files time step
        #print 'The files time step is ' + str(int(around(self.dt_read*24*3600))) +' seconds.'
        
        return True
        
    def f(self, current, dt):
        
        #The set time step is equal to the one of the file
        if float(repr(self.dt_read)[:11]) == float(repr(dt/24./3600.)[:11]):
            
            self.row_to_get += 1
            self.out[0] = float(self.mylist[int(0+self.row_to_get)][2])
            
        #There's a difference between the interpolation or extrapolating of a absolute value and a value refering to time. 'F' explesses a fLow rate...
        if str(self.decision)=='F':
            
            #The set time step is smaller than the files
            if float(repr(self.dt_read)[:11]) > float(repr(dt/24./3600.)[:11]):
            
                self.growing_t += dt/24./3600.
                
                #initial time step and interpolation
                if self.row_to_get == 0:
                    
                    self.row_to_get += 1
                    
                    #interpolation vectors
                    self.date_vector = [date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S")) , date2num(datetime.strptime(self.mylist[1][0]+" "+ self.mylist[1][1],"%d.%m.%Y %H:%M:%S"))]
                    self.flow = [self.mylist[0][2], self.mylist[1][2]]
                    
                    #interpolation
                    self.flow_int = interp1d(self.date_vector, self.flow)
                    
                    #value output at needed time
                    self.out[0] = float(self.flow_int(self.growing_t))
                          
                #if the overall time is out of the last interpolation range the next to rows will be interpolated    
                elif self.growing_t > date2num(datetime.strptime(self.mylist[1+self.interp_counter][0]+" "+ self.mylist[1+self.interp_counter][1],"%d.%m.%Y %H:%M:%S")):
                    
                    #passing through rows
                    self.interp_counter += 1
                    
                    #interpolation vectors
                    self.date_vector = [ date2num(datetime.strptime(self.mylist[int(0+self.interp_counter)][0]+" "+ self.mylist[int(0+self.interp_counter)][1],"%d.%m.%Y %H:%M:%S")) , date2num(datetime.strptime(self.mylist[int(1+self.interp_counter)][0]+" "+ self.mylist[int(1+self.interp_counter)][1],"%d.%m.%Y %H:%M:%S"))]
                    self.flow = [self.mylist[int(0+self.interp_counter)][2], self.mylist[int(1+self.interp_counter)][2] ]
                    
                    #print [self.date_vector, self.growing_t, len(self.mylist),self.interp_counter,self.mylist[int(0+self.interp_counter)][0],self.mylist[int(1+self.interp_counter)][0]]
                    #interpolation
                    self.flow_int = interp1d(self.date_vector, self.flow)
                
                #solves out of range problem
                #if self.interp_counter <= len(self.mylist)-1:
                    #value output at needed time
                self.out[0] = float(self.flow_int(self.growing_t))
                #else:
                   # self.out[0] =float(self.mylist[-1][2])               
                
                
                
            #The set time step is larger than the files
            else:
                self.growing_t += dt/24./3600.
                #calculating the need line of input-file
                self.myline = (self.growing_t - date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S"))) / self.dt_read
                
                #prevents index out of boundry error
                if self.myline <= len(self.mylist)-1:
                    
                    #interpolation vectors
                    self.date_vector = [ date2num(datetime.strptime(self.mylist[int(floor(self.myline))][0]+" "+ self.mylist[int(floor(self.myline))][1],"%d.%m.%Y %H:%M:%S")) , date2num(datetime.strptime(self.mylist[int(ceil(self.myline))][0]+" "+ self.mylist[int(ceil(self.myline))][1],"%d.%m.%Y %H:%M:%S"))]
                    self.flow = [self.mylist[int(floor(self.myline))][2], self.mylist[int(ceil(self.myline))][2] ]
                    
                    #interpolation
                    self.flow_int = interp1d(self.date_vector, self.flow)
                    
                    #takes care of the rounding issues... otherwise encountering interpolation errors
                    #if the needet time is very close to one thats listed in input - file it will be given out
                    if around((self.growing_t- date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S")))/(self.date_vector[0]- date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S"))), decimals= 6) == 1:
                    
                        self.out[0] = float(self.flow[0])
                        
                    elif around((self.growing_t- date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S")))/(self.date_vector[1]- date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S"))), decimals= 6) == 1:
                    
                        self.out[0] = float(self.flow[1])
                        
                    else:
                        self.out[0] = float(self.flow_int(self.growing_t))
                        
                #last value of file will be given out, becos last time step length can be different from others
                else:
 
                    self.out[0] = float(self.mylist[-1][2])
                    
        #There's a difference between the interpolation or extrapolating of a absolute value and a value refering to time. 'V' explesses a volume...           
        elif str(self.decision)=='H':
            
            #The set time step is smaller than the files
            if float(repr(self.dt_read)[:11]) > float(repr(dt/24./3600.)[:11]):
            
                #initial time step and interpolation
                if self.row_to_get == 0:
                
                    self.row_to_get += 1
                    
                    #gives height thats valid for first time step
                    self.flow = float(self.mylist[1][2])
                    self.flow_int = 0.0
                           
                #if the overall time (volume) is out of the last intervall range the next to rows will be summed up   
                elif self.growing_t > date2num(datetime.strptime(self.mylist[1+self.interp_counter][0]+" "+ self.mylist[1+self.interp_counter][1],"%d.%m.%Y %H:%M:%S")):
                
                    self.interp_counter += 1
                    
                    #gives height (volume) thats valid for first time step
                    self.flow = float(self.mylist[int(1+self.interp_counter)][2])
                    self.flow_int=0.0
                    
                #divided the cumulated volume by new interval length
                self.flow_int= self.flow / (float(self.dt_read)/(dt/24./3600.))
                
                #adds time step and output of volume
                self.growing_t += dt/24./3600.
                self.out[0] = float(self.flow_int) 
                
            #The set time step is larger than the files
            else:
                
                #calculates the percantage of incomplete time steps
                self.sum_decimals += float(repr(((dt/24./3600.)/self.dt_read)-floor(((dt/24./3600.)/self.dt_read)))[1:])
                self.mydecimals = float(repr(self.sum_decimals-floor(self.sum_decimals))[1:])
                if self.mydecimals == 0.0:
                    self.mydecimals = 1.0
                #calculating the need line of input-file
                self.myline = (self.growing_t + dt/24./3600. - date2num(datetime.strptime(self.mylist[0][0]+" "+ self.mylist[0][1],"%d.%m.%Y %H:%M:%S"))) / self.dt_read
            
                if self.myline <= len(self.mylist)-1:
                    
                    #resets flow and gets indexes prepared
                    self.flow_int = 0.0
                    self.indexes = [ int(floor(self.remember_line)), int(ceil(self.myline)) ]
                    #print self.indexes
                    #cumulates volumes for needed time interval
                    for i in arange(self.indexes[0], self.indexes[1]+1):
                        if i == self.indexes[0]:
                            self.flow_int += self.rest*float(self.mylist[self.indexes[0]][2])
                        elif i < self.indexes[1]:
                            self.flow_int += float(self.mylist[i][2])
                        else:
                            self.flow_int += self.mydecimals * float(self.mylist[self.indexes[1]][2])
                    
                    #adds height(volume) of incomplete time step of current step and rest of earlier time step, output value
                    #self.flow_int += self.mydecimals * float(self.mylist[self.indexes[1]][2])+self.rest
                    self.growing_t += dt/24./3600.
                    self.out[0] = self.flow_int
                    
                    #calculates rest of current time step and remembers upper index for next time step
                    self.rest = (1-self.mydecimals)
                    self.remember_line = self.indexes[1]
                    
                #last timestep, takes care of out of index error
                else:
                    
                    #resets flow and gets indexes prepared
                    self.flow_int = 0.0
                    self.indexes = [ int(floor(self.remember_line)), len(self.mylist)]
                    
                    #cumulates volumes for needed time interval up to the last value
                    for i in arange(self.indexes[0]+1, self.indexes[1]):
                        self.flow_int += float(self.mylist[i][2])
                    
                    #adds last rest, output value
                    self.flow_int += self.rest*float(self.mylist[self.indexes[0]][2])
                    self.growing_t += dt/24./3600.
                    self.out[0] = self.flow_int
                    
        #if wrong letter is typed in and timestep is differs from input files message will occur and output will be set zero        
        else:
            
            print 'Error: The input file has to be a time - height - series (H) [DD.MM.YYYY HH:MM:SS mm] or a time - flow - series (F) [DD.MM.YYYY HH:MM:SS l/h]'
            print 'The default setting is height (H)'
            self.out[0] = 0.0       
                    
        return dt
    
    def getClassName(self):
#        print "getClassName"
        return "File_Reader"

#def register(nr):
#    for c in pycd3.Node.__subclasses__():
#        nf = NodeFactoryFilereader(c)
#        nf.__disown__()
#        nr.addNodeFactory(nf)
#        
