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
from math import floor, ceil
from numpy import add, around, asarray
#sys.path.append('/Users/christianurich/Documents/CD3Waterbalance/WaterDemandModel')
from C_WaterDemandModel import WaterDemandModel
#import config



#class NodeFactory(pycd3.INodeFactory):
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
#        return "Practice.py"

class Demand_Model (pycd3.Node):
    def __init__(self):
        pycd3.Node.__init__(self)
        
        #Inports
        self.number_residential_units = pycd3.String("[5]")
        self.addParameter("Residential_Vector", self.number_residential_units)
        self.number_commercial_units = pycd3.String("[0]")
        self.addParameter("Commercial_Vector", self.number_commercial_units)
        
        self.model_selected = pycd3.String("Simple_Model")
        self.addParameter("Select_Model_(Simple_Model_or_Stochastic_Model)", self.model_selected)        
        
        #Outports to connect to Household
        self.Bathtub=pycd3.Flow()
        self.addOutPort("Outport_Bathtub", self.Bathtub)
        self.Shower=pycd3.Flow()
        self.addOutPort("Outport_Shower", self.Shower)
        self.Toilet=pycd3.Flow()
        self.addOutPort("Outport_Toilet", self.Toilet)
        self.Kitchen_Tap=pycd3.Flow()
        self.addOutPort("Outport_Kitchen_Tap", self.Kitchen_Tap)
        self.Handbasin_Tap=pycd3.Flow()
        self.addOutPort("Outport_Handbasin_Tap", self.Handbasin_Tap)
        self.Washing_Machine=pycd3.Flow()
        self.addOutPort("Outport_Washing_Machine", self.Washing_Machine)
        self.Dishwasher=pycd3.Flow()
        self.addOutPort("Outport_Dishwasher", self.Dishwasher)
        self.Evap_Cooler=pycd3.Flow()
        self.addOutPort("Outport_Evapcooler",self.Evap_Cooler)
        
        #Outports to check Demand
        self.CheckBathtub=pycd3.Flow()
        self.addOutPort("Outport_Check_Bathtub", self.CheckBathtub)
        self.CheckShower=pycd3.Flow()
        self.addOutPort("Outport_Check_Shower", self.CheckShower)
        self.CheckToilet=pycd3.Flow()
        self.addOutPort("Outport_Check_Toilet", self.CheckToilet)
        self.CheckKitchen_Tap=pycd3.Flow()
        self.addOutPort("Outport_Check_Kitchen_Tap", self.CheckKitchen_Tap)
        self.CheckHandbasin_Tap=pycd3.Flow()
        self.addOutPort("Outport_Check_Handbasin_Tap", self.CheckHandbasin_Tap)
        self.CheckWashing_Machine=pycd3.Flow()
        self.addOutPort("Outport_Check_Washing_Machine", self.CheckWashing_Machine)
        self.CheckDishwasher=pycd3.Flow()
        self.addOutPort("Outport_Check_Dishwasher", self.CheckDishwasher)
        self.CheckEvap_Cooler=pycd3.Flow()
        self.addOutPort("Outport_Check_Evapcooler",self.CheckEvap_Cooler)
        
#        print "init node"
        
    def init(self, start, stop, dt):
#        print start
#        print stop
#        print dt
        
        #converting inputstrings to lists
        self.number_residential_units = str(self.number_residential_units)[1:-1].split(',')
        for i in range(len(self.number_residential_units)):
            self.number_residential_units[i] = int(self.number_residential_units[i])
        self.number_commercial_units = str(self.number_commercial_units)[1:-1].split(',')
        for i in range(len(self.number_commercial_units)):
            self.number_commercial_units[i] = int(self.number_commercial_units[i])
        
        if self.model_selected == "Stochastic_Model":
            
            #simulation start time and stop time for Demand Model
            self.simulation_start_time = date2num(datetime.strptime(str(start.to_datetime()),"%Y-%m-%d %H:%M:%S"))
            self.simulation_stop_time = date2num(datetime.strptime(str(stop.to_datetime()),"%Y-%m-%d %H:%M:%S"))
        
            #start value counting - time - variable for interpolation 
            self.overall_time = self.simulation_start_time
            
            #creating self.run - object
            self.run = WaterDemandModel(self.number_residential_units,self.number_commercial_units) 
            
        elif self.model_selected == "Simple_Model":
            
            self.number_people_res = 0
            self.number_people_com = 0
            
            for i in range(len(self.number_residential_units)):
                self.number_people_res += self.number_residential_units[i]
            
            for i in range(len(self.number_commercial_units)):
                self.number_people_com += self.number_commercial_units[i]
            
        else:
            
            print "Model selected not valid!"
            
        
        return True
        
    def f(self, current, dt):
        
        if self.model_selected == "Stochastic_Model":
               
            def adding_unit_uses(Demanddictionary,usestring):
                '''
                function is adding all the vectors from different actors for commerical and residential uses together
                ''' 
                sumvector = asarray([0.0]*24)
                if self.number_residential_units == [0]:
                    if usestring == 'bath' or usestring == 'washing_machine':
                        pass
                    else:
                        for i in range(len(self.number_commercial_units)):
                            sumvector += asarray(Demanddictionary['C'+str(i+1)][usestring])
                elif self.number_commercial_units == [0]:
                    if usestring == 'evap_cooler':
                        pass
                    else:
                        for i in range(len(self.number_residential_units)):
                            sumvector += asarray(Demanddictionary['R'+str(i+1)][usestring])
                else:
                    if usestring == 'bath' or usestring == 'washing_machine':
                        for i in range(len(self.number_residential_units)):
                            sumvector += asarray(Demanddictionary['R'+str(i+1)][usestring])
                    elif usestring == 'evap_cooler':
                        for i in range(len(self.number_commercial_units)):
                            sumvector += asarray(Demanddictionary['C'+str(i+1)][usestring])                
                    else:
                        for i in range(len(self.number_residential_units)):
                            sumvector += asarray(Demanddictionary['R'+str(i+1)][usestring])
                        for i in range(len(self.number_commercial_units)):
                            sumvector += asarray(Demanddictionary['C'+str(i+1)][usestring])
                            
                return sumvector
            
            
            if dt < 3600 :
                '''
                calculation for timesteps smaller one hour
                '''
                #creates vector with zeors and a single 1 at a random spot within the vector
                def randomuse(demandtimestep = 3600):
                    vector =[0 for m in range(int(ceil(demandtimestep/float(dt))+1))]
                    index = random.randint(0, int(ceil(demandtimestep/float(dt)-1)))
                    vector[index]=1
                    return vector
    
                #first run of demand model, when simulation starts
                if self.overall_time == self.simulation_start_time:
                
                    #runs Demandmodel and adds residential and commercial use
                    self.run.newDay()
                    Demanddictionary = self.run.getDemands()
                    self.shower_vector = adding_unit_uses(Demanddictionary,'shower')
                    self.toilet_vector = adding_unit_uses(Demanddictionary,'toilet')
                    self.kitchen_tap_vector = adding_unit_uses(Demanddictionary,'kitchen_tap')
                    self.handbasin_tap_vector = adding_unit_uses(Demanddictionary,'handbasin_tap')
                    self.bath_vector = adding_unit_uses(Demanddictionary,'bath')
                    self.washing_machine_vector = adding_unit_uses(Demanddictionary,'washing_machine')
                    self.dishwasher_vector = adding_unit_uses(Demanddictionary,'dish_washer')
                    self.evapcooler_vector = adding_unit_uses(Demanddictionary,'evap_cooler')
                 
                    #start values for List indexes
                    self.hourly_rate = int(ceil((around(self.simulation_start_time, decimals=4)-floor(around(self.simulation_start_time, decimals=4)))*24))
                    self.time_facor_index = 0

                    #creating random demand vector eg.:[0,0,..,1,0]
                    self.Bathtub_demand_factor = randomuse(3600-dt)
                    self.Shower_demand_factor = randomuse(3600-dt)
                    self.Toilet_demand_factor = randomuse(3600-dt)
                    self.Kitchen_Tap_demand_factor = randomuse(3600-dt)
                    self.Handbasin_Tap_demand_factor = randomuse(3600-dt)
                    self.Washing_Machine_demand_factor = randomuse(3600-dt)
                    self.Dishwasher_demand_factor = randomuse(3600-dt)
                    self.Evapcooler_demand_factor = randomuse(3600-dt)
            
                #run demand model for the next 24 hours
                elif self.overall_time >= ceil(self.overall_time) -1/24. and self.overall_time <= ceil(self.overall_time) -1/24. + (dt)/3600./24. :
                
                    #runs Demandmodel and adds residential and commercial use
                    self.run.newDay()
                    Demanddictionary = self.run.getDemands()
                    self.shower_vector = adding_unit_uses(Demanddictionary,'shower')
                    self.toilet_vector = adding_unit_uses(Demanddictionary,'toilet')
                    self.kitchen_tap_vector = adding_unit_uses(Demanddictionary,'kitchen_tap')
                    self.handbasin_tap_vector = adding_unit_uses(Demanddictionary,'handbasin_tap')
                    self.bath_vector = adding_unit_uses(Demanddictionary,'bath')
                    self.washing_machine_vector = adding_unit_uses(Demanddictionary,'washing_machine')
                    self.dishwasher_vector = adding_unit_uses(Demanddictionary,'dish_washer')
                    self.evapcooler_vector = adding_unit_uses(Demanddictionary,'evap_cooler')
                
                    #resetting counter that index Demandvector and Demandfactorvector
                    self.hourly_rate = 0
                    self.time_facor_index = 0
                
                else:
                    pass
                
                #getting a random index when new hour begins to simulate random water use at some point over the hour...  
                if self.overall_time - floor(self.overall_time) > self.hourly_rate/24. and self.overall_time - floor(self.overall_time) < self.hourly_rate/24. + (dt)/3600./24.:
                
                    #counter that index Demandvector and Demandfactorvector
                    self.hourly_rate += 1
                    self.time_facor_index = 0
                
                    #creating random demand vector eg.:[0,0,..,1,0]
                    self.Bathtub_demand_factor = randomuse()
                    self.Shower_demand_factor = randomuse()
                    self.Toilet_demand_factor = randomuse()
                    self.Kitchen_Tap_demand_factor = randomuse()
                    self.Handbasin_Tap_demand_factor = randomuse()
                    self.Washing_Machine_demand_factor = randomuse()
                    self.Dishwasher_demand_factor = randomuse()
                    self.Evapcooler_demand_factor = randomuse()
                
                else:
                    pass
            
                '''
                for arkward timesteps like 557 seconds the randomvector is to long and so the single value of one (the factor that creates demand during the hour) could be at the last 
                position of the vector and so not create any demand during that hour. The following if statement solves that problem by checkin the ones postion and if necessary correcting it.
                '''
                if self.overall_time - floor(self.overall_time) +dt/3600./24. > self.hourly_rate/24. and self.overall_time - floor(self.overall_time) < self.hourly_rate/24.:
                    self.Bathtub_demand_factor_check = sum(self.Bathtub_demand_factor[:self.time_facor_index+1])
                    self.Shower_demand_factor_check = sum(self.Shower_demand_factor[:self.time_facor_index+1])
                    self.Toilet_demand_factor_check = sum(self.Toilet_demand_factor[:self.time_facor_index+1])
                    self.Kitchen_Tap_demand_factor_check = sum(self.Kitchen_Tap_demand_factor[:self.time_facor_index+1])
                    self.Handbasin_Tap_demand_factor_check = sum(self.Handbasin_Tap_demand_factor[:self.time_facor_index+1])
                    self.Washing_Machine_demand_factor_check = sum(self.Washing_Machine_demand_factor[:self.time_facor_index+1])
                    self.Dishwasher_demand_factor_check = sum(self.Dishwasher_demand_factor[:self.time_facor_index+1])
                    self.Evapcooler_demand_factor_check = sum(self.Evapcooler_demand_factor[:self.time_facor_index+1])
                 
                    if self.Evapcooler_demand_factor_check != 1 or self.Bathtub_demand_factor_check != 1 or self.Shower_demand_factor_check != 1 or self.Toilet_demand_factor_check != 1 or self.Kitchen_Tap_demand_factor_check != 1 or self.Handbasin_Tap_demand_factor_check != 1 or self.Washing_Machine_demand_factor_check != 1 or self.Dishwasher_demand_factor_check != 1:
                        
                        if self.Evapcooler_demand_factor_check != 1:
                            self.Evapcooler_demand_factor[self.time_facor_index] = 1
                            self.Evapcooler_demand_factor[self.time_facor_index + 1] = 0
                         
                        else:
                             pass                    
                        
                        if self.Bathtub_demand_factor_check != 1:
                            self.Bathtub_demand_factor[self.time_facor_index] = 1
                            self.Bathtub_demand_factor[self.time_facor_index + 1] = 0
                         
                        else:
                             pass      
                     
                        if self.Shower_demand_factor_check != 1:
                            self.Shower_demand_factor[self.time_facor_index] = 1
                            self.Shower_demand_factor[self.time_facor_index + 1] = 0
                             
                        else:
                            pass
                     
                        if self.Toilet_demand_factor_check != 1:
                            self.Toilet_demand_factor[self.time_facor_index] = 1
                            self.Toilet_demand_factor[self.time_facor_index + 1] = 0
                         
                        else:
                            pass
                        
                        if self.Kitchen_Tap_demand_factor_check != 1:
                            self.Kitchen_Tap_demand_factor[self.time_facor_index] = 1
                            self.Kitchen_Tap_demand_factor[self.time_facor_index + 1] = 0
                         
                        else:
                            pass
                     
                        if self.Handbasin_Tap_demand_factor_check != 1:
                            self.Handbasin_Tap_demand_factor[self.time_facor_index] = 1
                            self.Handbasin_Tap_demand_factor[self.time_facor_index + 1] = 0
                         
                        else:
                            pass
                     
                        if self.Washing_Machine_demand_factor_check != 1:
                            self.Washing_Machine_demand_factor[self.time_facor_index] = 1
                            self.Washing_Machine_demand_factor[self.time_facor_index + 1] = 0
                         
                        else:
                            pass
                     
                        if self.Dishwasher_demand_factor_check != 1:
                            self.Dishwasher_demand_factor[self.time_facor_index] = 1
                            self.Dishwasher_demand_factor[self.time_facor_index + 1] = 0
                         
                        else:
                            pass   
                     
                    else:
                        pass
                 
                else: 
                    pass

                #Outports for connecting to Builing
                self.Bathtub[0] = self.bath_vector[self.hourly_rate]*self.Bathtub_demand_factor[self.time_facor_index]/1000
                self.Shower[0] = self.shower_vector[self.hourly_rate]*self.Shower_demand_factor[self.time_facor_index]/1000
                self.Toilet[0] = self.toilet_vector[self.hourly_rate]*self.Toilet_demand_factor[self.time_facor_index]/1000
                self.Kitchen_Tap[0] = self.kitchen_tap_vector[self.hourly_rate]*self.Kitchen_Tap_demand_factor[self.time_facor_index]/1000
                self.Handbasin_Tap[0] = self.handbasin_tap_vector[self.hourly_rate]*self.Handbasin_Tap_demand_factor[self.time_facor_index]/1000
                self.Washing_Machine[0] = self.washing_machine_vector[self.hourly_rate]*self.Washing_Machine_demand_factor[self.time_facor_index]/1000
                self.Dishwasher[0] = self.dishwasher_vector[self.hourly_rate]*self.Dishwasher_demand_factor[self.time_facor_index]/1000
                self.Evap_Cooler[0] = self.evapcooler_vector[self.hourly_rate]*self.Evapcooler_demand_factor[self.time_facor_index]/1000
                
                #Outports for Checking Demand
                self.CheckBathtub[0] = self.Bathtub[0]
                self.CheckShower[0] = self.Shower[0]
                self.CheckToilet[0] = self.Toilet[0]
                self.CheckKitchen_Tap[0] = self.Kitchen_Tap[0]
                self.CheckHandbasin_Tap[0] = self.Handbasin_Tap[0]
                self.CheckWashing_Machine[0] = self.Washing_Machine[0]
                self.CheckDishwasher[0] = self.Dishwasher[0]
                self.CheckEvap_Cooler[0] = self.Evap_Cooler[0]
            
                #counter that index Demandvector and Demandfactorvector
                self.overall_time += dt/3600./24.
                self.time_facor_index += 1
            
                       
            elif dt == 3600:
                '''
                calculation for timesteps equal one hour
                ''' 
                #start values for List indexes         
                self.hourly_rate = int(ceil((around(self.simulation_start_time, decimals=4)-floor(around(self.simulation_start_time, decimals=4)))*24))
                
                #first run of demand model, when simulation starts
                if self.overall_time == self.simulation_start_time:
                    
                    #runs Demandmodel and adds residential and commercial use
                    self.run.newDay()
                    Demanddictionary = self.run.getDemands()
                    self.shower_vector = adding_unit_uses(Demanddictionary,'shower')
                    self.toilet_vector = adding_unit_uses(Demanddictionary,'toilet')
                    self.kitchen_tap_vector = adding_unit_uses(Demanddictionary,'kitchen_tap')
                    self.handbasin_tap_vector = adding_unit_uses(Demanddictionary,'handbasin_tap')
                    self.bath_vector = adding_unit_uses(Demanddictionary,'bath')
                    self.washing_machine_vector = adding_unit_uses(Demanddictionary,'washing_machine')
                    self.dishwasher_vector = adding_unit_uses(Demanddictionary,'dish_washer')
                    self.evapcooler_vector = adding_unit_uses(Demanddictionary,'evap_cooler')
                    
                else:
                    pass
                
                if self.hourly_rate > 23:
                    
                    #runs Demandmodel and adds residential and commercial use
                    self.run.newDay()
                    Demanddictionary = self.run.getDemands()
                    self.shower_vector = adding_unit_uses(Demanddictionary,'shower')
                    self.toilet_vector = adding_unit_uses(Demanddictionary,'toilet')
                    self.kitchen_tap_vector = adding_unit_uses(Demanddictionary,'kitchen_tap')
                    self.handbasin_tap_vector = adding_unit_uses(Demanddictionary,'handbasin_tap')
                    self.bath_vector = adding_unit_uses(Demanddictionary,'bath')
                    self.washing_machine_vector = adding_unit_uses(Demanddictionary,'washing_machine')
                    self.dishwasher_vector = adding_unit_uses(Demanddictionary,'dish_washer')
                    self.evapcooler_vector = adding_unit_uses(Demanddictionary,'evap_cooler')
                    
                    self.hourly_rate = 0
    
                else:
                    pass
                
                #Outports for connecting to Builing
                self.Bathtub[0] = self.bath_vector[self.hourly_rate]/1000
                self.Shower[0] = self.shower_vector[self.hourly_rate]/1000
                self.Toilet[0] = self.toilet_vector[self.hourly_rate]/1000
                self.Kitchen_Tap[0] = self.kitchen_tap_vector[self.hourly_rate]/1000
                self.Handbasin_Tap[0] = self.handbasin_tap_vector[self.hourly_rate]/1000
                self.Washing_Machine[0] = self.washing_machine_vector[self.hourly_rate]/1000
                self.Dishwasher[0] = self.dishwasher_vector[self.hourly_rate]/1000
                self.Evap_Cooler[0] = self.evapcooler_vector[self.hourly_rate]/1000
                
                #Outports for Checking Demand
                self.CheckBathtub[0] = self.Bathtub[0]
                self.CheckShower[0] = self.Shower[0]
                self.CheckToilet[0] = self.Toilet[0]
                self.CheckKitchen_Tap[0] = self.Kitchen_Tap[0]
                self.CheckHandbasin_Tap[0] = self.Handbasin_Tap[0]
                self.CheckWashing_Machine[0] = self.Washing_Machine[0]
                self.CheckDishwasher[0] = self.Dishwasher[0]
                self.CheckEvap_Cooler[0] = self.Evap_Cooler[0]        
    
                self.hourly_rate += 1
                self.overall_time += dt/3600./24.
            
            
            elif dt == 3600*24:
                '''
                calculation for timesteps equal one day
                '''
                #start values for List indexes
                self.startingsum = int(ceil((around(self.simulation_start_time, decimals=4)-floor(around(self.simulation_start_time, decimals=4)))*24))
                
                #first run of demand model, when simulation starts
                if self.overall_time == self.simulation_start_time:
                    
                    #runs Demandmodel and adds residential and commercial use
                    self.run.newDay()
                    Demanddictionary = self.run.getDemands()
                    self.shower_vector_1 = adding_unit_uses(Demanddictionary,'shower').tolist()
                    self.toilet_vector_1 = adding_unit_uses(Demanddictionary,'toilet').tolist()
                    self.kitchen_tap_vector_1 = adding_unit_uses(Demanddictionary,'kitchen_tap').tolist()
                    self.handbasin_tap_vector_1 = adding_unit_uses(Demanddictionary,'handbasin_tap').tolist()
                    self.bath_vector_1 = adding_unit_uses(Demanddictionary,'bath').tolist()
                    self.washing_machine_vector_1 = adding_unit_uses(Demanddictionary,'washing_machine').tolist()
                    self.dishwasher_vector_1 = adding_unit_uses(Demanddictionary,'dish_washer').tolist()
                    self.evapcooler_vector_1 = adding_unit_uses(Demanddictionary,'evap_cooler').tolist()
                    
                    #runs Demandmodel and adds residential and commercial use
                    self.run.newDay()
                    Demanddictionary = self.run.getDemands()
                    self.shower_vector_2 = adding_unit_uses(Demanddictionary,'shower').tolist()
                    self.toilet_vector_2 = adding_unit_uses(Demanddictionary,'toilet').tolist()
                    self.kitchen_tap_vector_2 = adding_unit_uses(Demanddictionary,'kitchen_tap').tolist()
                    self.handbasin_tap_vector_2 = adding_unit_uses(Demanddictionary,'handbasin_tap').tolist()
                    self.bath_vector_2 = adding_unit_uses(Demanddictionary,'bath').tolist()
                    self.washing_machine_vector_2 = adding_unit_uses(Demanddictionary,'washing_machine').tolist()
                    self.dishwasher_vector_2 = adding_unit_uses(Demanddictionary,'dish_washer').tolist()
                    self.evapcooler_vector_2 = adding_unit_uses(Demanddictionary,'evap_cooler').tolist()
                    
                    
                    #correcting list index
                    if self.startingsum == 0:
                        self.startingsum = 1
                    else:
                        pass
                else:
                    #resetting counter that index Demandvector and Demandfactorvector
                    self.startingsum = 1
                    
                    self.shower_vector_1 = self.shower_vector_2
                    self.toilet_vector_1 = self.toilet_vector_2
                    self.kitchen_tap_vector_1 = self.kitchen_tap_vector_2
                    self.handbasin_tap_vector_1 = self.handbasin_tap_vector_2
                    self.bath_vector_1 = self.bath_vector_2
                    self.washing_machine_vector_1 = self.washing_machine_vector_2
                    self.dishwasher_vector_1 = self.dishwasher_vector_2   
                    self.evapcooler_vector_1 = self.evapcooler_vector_2
                    
                    #runs Demandmodel and adds residential and commercial use
                    self.run.newDay()
                    Demanddictionary = self.run.getDemands()
                    self.shower_vector_2 = adding_unit_uses(Demanddictionary,'shower')
                    self.toilet_vector_2 = adding_unit_uses(Demanddictionary,'toilet')
                    self.kitchen_tap_vector_2 = adding_unit_uses(Demanddictionary,'kitchen_tap')
                    self.handbasin_tap_vector_2 = adding_unit_uses(Demanddictionary,'handbasin_tap')
                    self.bath_vector_2 = adding_unit_uses(Demanddictionary,'bath')
                    self.washing_machine_vector_2 = adding_unit_uses(Demanddictionary,'washing_machine')
                    self.dishwasher_vector_2 = adding_unit_uses(Demanddictionary,'dish_washer')
                    self.evapcooler_vector_2 = adding_unit_uses(Demanddictionary,'evap_cooler')
                 
                self.overall_time += dt/3600./24.
                
                #Outports for connecting to Builing     
                self.Bathtub[0] = (sum(self.bath_vector_1[self.startingsum:])+self.bath_vector_2[0])/1000
                self.Shower[0] = (sum(self.shower_vector_1[self.startingsum:])+self.shower_vector_2[0])/1000
                self.Toilet[0] = (sum(self.toilet_vector_1[self.startingsum:])+self.toilet_vector_2[0])/1000
                self.Kitchen_Tap[0] = (sum(self.kitchen_tap_vector_1[self.startingsum:])+self.kitchen_tap_vector_2[0])/1000
                self.Handbasin_Tap[0] = (sum(self.handbasin_tap_vector_1[self.startingsum:])+self.handbasin_tap_vector_2[0])/1000
                self.Washing_Machine[0] = (sum(self.washing_machine_vector_1[self.startingsum:])+self.washing_machine_vector_2[0])/1000
                self.Dishwasher[0] = (sum(self.dishwasher_vector_1[self.startingsum:])+self.dishwasher_vector_2[0])/1000
                self.Evap_Cooler[0] = (sum(self.evapcooler_vector_1[self.startingsum:])+self.evapcooler_vector_2[0])/1000
                
                #Outports for Checking Demand
                self.CheckBathtub[0] = self.Bathtub[0]
                self.CheckShower[0] = self.Shower[0]
                self.CheckToilet[0] = self.Toilet[0]
                self.CheckKitchen_Tap[0] = self.Kitchen_Tap[0]
                self.CheckHandbasin_Tap[0] = self.Handbasin_Tap[0]
                self.CheckWashing_Machine[0] = self.Washing_Machine[0]
                self.CheckDishwasher[0] = self.Dishwasher[0]
                self.CheckEvap_Cooler[0] = self.Evap_Cooler[0]
            
            
            elif dt % 3600 == 0 and dt < 3600*24:
                '''
                calculation for timesteps equal to full hour steps
                '''
                
                #first run of demand model, when simulation starts
                if self.overall_time == self.simulation_start_time:
                    
                    #start values for List indexes
                    self.startingsum = int(ceil((around(self.simulation_start_time, decimals=4)-floor(around(self.simulation_start_time, decimals=4)))*24))
                
                    if self.startingsum + dt/3600 <= 23:
                        
                        #values for List indexes
                        self.vector_end_index = dt/3600 + self.startingsum
                        
                        #runs Demandmodel and adds residential and commercial use
                        self.run.newDay()
                        Demanddictionary = self.run.getDemands()
                        self.shower_vector_1 = adding_unit_uses(Demanddictionary,'shower').tolist()
                        self.toilet_vector_1 = adding_unit_uses(Demanddictionary,'toilet').tolist()
                        self.kitchen_tap_vector_1 = adding_unit_uses(Demanddictionary,'kitchen_tap').tolist()
                        self.handbasin_tap_vector_1 = adding_unit_uses(Demanddictionary,'handbasin_tap').tolist()
                        self.bath_vector_1 = adding_unit_uses(Demanddictionary,'bath').tolist()
                        self.washing_machine_vector_1 = adding_unit_uses(Demanddictionary,'washing_machine').tolist()
                        self.dishwasher_vector_1 = adding_unit_uses(Demanddictionary,'dish_washer').tolist()
                        self.evapcooler_vector_1 = adding_unit_uses(Demanddictionary,'evap_cooler').tolist()
                        
                        #correcting list index
                        if self.startingsum == 0:
                            self.startingsum = 1
                        
                        else:
                            pass        
                        
                        #Outports for connecting to Builing
                        self.Bathtub[0] = sum(self.bath_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Shower[0] = sum(self.shower_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Toilet[0] = sum(self.toilet_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Kitchen_Tap[0] = sum(self.kitchen_tap_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Handbasin_Tap[0] = sum(self.handbasin_tap_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Washing_Machine[0] = sum(self.washing_machine_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Dishwasher[0] = sum(self.dishwasher_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Evap_Cooler[0] = sum(self.evapcooler_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
     
                        #increases index
                        self.startingsum += dt/3600
    
                    else:
                        
                        #runs Demandmodel and adds residential and commercial use
                        self.run.newDay()
                        Demanddictionary = self.run.getDemands()
                        self.shower_vector_1 = adding_unit_uses(Demanddictionary,'shower').tolist()
                        self.toilet_vector_1 = adding_unit_uses(Demanddictionary,'toilet').tolist()
                        self.kitchen_tap_vector_1 = adding_unit_uses(Demanddictionary,'kitchen_tap').tolist()
                        self.handbasin_tap_vector_1 = adding_unit_uses(Demanddictionary,'handbasin_tap').tolist()
                        self.bath_vector_1 = adding_unit_uses(Demanddictionary,'bath').tolist()
                        self.washing_machine_vector_1 = adding_unit_uses(Demanddictionary,'washing_machine').tolist()
                        self.dishwasher_vector_1 = adding_unit_uses(Demanddictionary,'dish_washer').tolist()
                        self.evapcooler_vector_1 = adding_unit_uses(Demanddictionary,'evap_cooler').tolist()
                        
                        #runs Demandmodel and adds residential and commercial use
                        self.run.newDay()
                        Demanddictionary = self.run.getDemands()
                        self.shower_vector_2 = adding_unit_uses(Demanddictionary,'shower').tolist()
                        self.toilet_vector_2 = adding_unit_uses(Demanddictionary,'toilet').tolist()
                        self.kitchen_tap_vector_2 = adding_unit_uses(Demanddictionary,'kitchen_tap').tolist()
                        self.handbasin_tap_vector_2 = adding_unit_uses(Demanddictionary,'handbasin_tap').tolist()
                        self.bath_vector_2 = adding_unit_uses(Demanddictionary,'bath').tolist()
                        self.washing_machine_vector_2 = adding_unit_uses(Demanddictionary,'washing_machine').tolist()
                        self.dishwasher_vector_2 = adding_unit_uses(Demanddictionary,'dish_washer').tolist()
                        self.evapcooler_vector_2 = adding_unit_uses(Demanddictionary,'evap_cooler').tolist()
                        
                        self.vector_end_index = (dt/3600 + self.startingsum) - 23
                        
                        #Outports for connecting to Builing
                        self.Bathtub[0] = (sum(self.bath_vector_1[self.startingsum+1:]) + sum(self.bath_vector_2[:self.vector_end_index+1]))/1000
                        self.Shower[0] = (sum(self.shower_vector_1[self.startingsum+1:]) + sum(self.shower_vector_2[:self.vector_end_index+1]))/1000
                        self.Toilet[0] = (sum(self.toilet_vector_1[self.startingsum+1:]) + sum(self.toilet_vector_2[:self.vector_end_index+1]))/1000
                        self.Kitchen_Tap[0] = (sum(self.kitchen_tap_vector_1[self.startingsum+1:]) + sum(self.kitchen_tap_vector_2[:self.vector_end_index+1]))/1000
                        self.Handbasin_Tap[0] = (sum(self.handbasin_tap_vector_1[self.startingsum+1:]) + sum(self.handbasin_tap_vector_2[:self.vector_end_index+1]))/1000
                        self.Washing_Machine[0] = (sum(self.washing_machine_vector_1[self.startingsum+1:]) + sum(self.washing_machine_vector_2[:self.vector_end_index+1]))/1000
                        self.Dishwasher[0] = (sum(self.dishwasher_vector_1[self.startingsum+1:]) + sum(self.dishwasher_vector_2[:self.vector_end_index+1]))/1000
                        self.Evap_Cooler[0] = (sum(self.evapcooler_vector_1[self.startingsum+1:]) + sum(self.evapcooler_vector_2[:self.vector_end_index+1]))/1000
                        
                        self.shower_vector_1 = self.shower_vector_2
                        self.toilet_vector_1 = self.toilet_vector_2
                        self.kitchen_tap_vector_1 = self.kitchen_tap_vector_2
                        self.handbasin_tap_vector_1 = self.handbasin_tap_vector_2
                        self.bath_vector_1 = self.bath_vector_2
                        self.washing_machine_vector_1 = self.washing_machine_vector_2
                        self.dishwasher_vector_1 = self.dishwasher_vector_2
                        
                        
                        #print 'First Timestep spreading to next day '+str(len(self.dishwasher_vector_1[self.startingsum+1:])+len(self.dishwasher_vector_2[:self.vector_end_index+1]))
                        
                        #restting list index
                        self.startingsum = self.vector_end_index
    
                else:
                    if self.startingsum + dt/3600 <= 23:
                        
                        #values for List indexes
                        self.vector_end_index = dt/3600 + self.startingsum  
                        
                        #Outports for connecting to Builing
                        self.Bathtub[0] = sum(self.bath_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Shower[0] = sum(self.shower_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Toilet[0] = sum(self.toilet_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Kitchen_Tap[0] = sum(self.kitchen_tap_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Handbasin_Tap[0] = sum(self.handbasin_tap_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Washing_Machine[0] = sum(self.washing_machine_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Dishwasher[0] = sum(self.dishwasher_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        self.Evap_Cooler[0] = sum(self.evapcooler_vector_1[self.startingsum+1:self.vector_end_index+1])/1000
                        
                        #increases index
                        self.startingsum += dt/3600
                        
                    elif self.startingsum + dt/3600 > 23 and self.startingsum < 23:
    
                        #runs Demandmodel and adds residential and commercial use
                        self.run.newDay()
                        Demanddictionary = self.run.getDemands()
                        self.shower_vector_2 = adding_unit_uses(Demanddictionary,'shower')
                        self.toilet_vector_2 = adding_unit_uses(Demanddictionary,'toilet')
                        self.kitchen_tap_vector_2 = adding_unit_uses(Demanddictionary,'kitchen_tap')
                        self.handbasin_tap_vector_2 = adding_unit_uses(Demanddictionary,'handbasin_tap')
                        self.bath_vector_2 = adding_unit_uses(Demanddictionary,'bath')
                        self.washing_machine_vector_2 = adding_unit_uses(Demanddictionary,'washing_machine')
                        self.dishwasher_vector_2 = adding_unit_uses(Demanddictionary,'dish_washer')
                        self.evapcooler_vector_2 = adding_unit_uses(Demanddictionary,'evap_cooler')
                        
                        #values for List indexes
                        self.vector_end_index = (dt/3600 + self.startingsum) - 24    
                        
                        #Outports for connecting to Builing
                        self.Bathtub[0] = (sum(self.bath_vector_1[self.startingsum+1:]) + sum(self.bath_vector_2[:self.vector_end_index+1]))/1000
                        self.Shower[0] = (sum(self.shower_vector_1[self.startingsum+1:]) + sum(self.shower_vector_2[:self.vector_end_index+1]))/1000
                        self.Toilet[0] = (sum(self.toilet_vector_1[self.startingsum+1:]) + sum(self.toilet_vector_2[:self.vector_end_index+1]))/1000
                        self.Kitchen_Tap[0] = (sum(self.kitchen_tap_vector_1[self.startingsum+1:]) + sum(self.kitchen_tap_vector_2[:self.vector_end_index+1]))/1000
                        self.Handbasin_Tap[0] = (sum(self.handbasin_tap_vector_1[self.startingsum+1:]) + sum(self.handbasin_tap_vector_2[:self.vector_end_index+1]))/1000
                        self.Washing_Machine[0] = (sum(self.washing_machine_vector_1[self.startingsum+1:]) + sum(self.washing_machine_vector_2[:self.vector_end_index+1]))/1000
                        self.Dishwasher[0] = (sum(self.dishwasher_vector_1[self.startingsum+1:]) + sum(self.dishwasher_vector_2[:self.vector_end_index+1]))/1000
                        self.Evap_Cooler[0] = (sum(self.evapcooler_vector_1[self.startingsum+1:]) + sum(self.evapcooler_vector_2[:self.vector_end_index+1]))/1000
                        
                        self.shower_vector_1 = self.shower_vector_2
                        self.toilet_vector_1 = self.toilet_vector_2
                        self.kitchen_tap_vector_1 = self.kitchen_tap_vector_2
                        self.handbasin_tap_vector_1 = self.handbasin_tap_vector_2
                        self.bath_vector_1 = self.bath_vector_2
                        self.washing_machine_vector_1 = self.washing_machine_vector_2
                        self.dishwasher_vector_1 = self.dishwasher_vector_2         
                        self.evapcooler_vector_1 = self.evapcooler_vector_2 
                        
                        #restting list index
                        self.startingsum = self.vector_end_index
                        
                        
                        
                    elif self.startingsum + dt/3600 == 23:
                        
                        #Outports for connecting to Builing
                        self.Bathtub[0] = sum(self.bath_vector_1[self.startingsum+1:])/1000
                        self.Shower[0] = sum(self.shower_vector_1[self.startingsum+1:])/1000
                        self.Toilet[0] = sum(self.toilet_vector_1[self.startingsum+1:])/1000
                        self.Kitchen_Tap[0] = sum(self.kitchen_tap_vector_1[self.startingsum+1:])/1000
                        self.Handbasin_Tap[0] = sum(self.handbasin_tap_vector_1[self.startingsum+1:])/1000
                        self.Washing_Machine[0] = sum(self.washing_machine_vector_1[self.startingsum+1:])/1000
                        self.Dishwasher[0] = sum(self.dishwasher_vector_1[self.startingsum+1:])/1000
                        self.Evap_Cooler[0] = sum(self.evapcooler_vector_1[self.startingsum+1:])/1000
                        
                        #runs Demandmodel and adds residential and commercial use
                        self.run.newDay()
                        Demanddictionary = self.run.getDemands()
                        self.shower_vector_1 = adding_unit_uses(Demanddictionary,'shower')
                        self.toilet_vector_1 = adding_unit_uses(Demanddictionary,'toilet')
                        self.kitchen_tap_vector_1 = adding_unit_uses(Demanddictionary,'kitchen_tap')
                        self.handbasin_tap_vector_1 = adding_unit_uses(Demanddictionary,'handbasin_tap')
                        self.bath_vector_1 = adding_unit_uses(Demanddictionary,'bath')
                        self.washing_machine_vector_1 = adding_unit_uses(Demanddictionary,'washing_machine')
                        self.dishwasher_vector_1 = adding_unit_uses(Demanddictionary,'dish_washer')
                        self.evapcooler_vector_1 = adding_unit_uses(Demanddictionary,'evap_cooler')
                        
                        #restting list index
                        self.startingsum = 0
                    else:
                        
                        self.vector_end_index = dt/3600
                        
                        #runs Demandmodel and adds residential and commercial use
                        self.run.newDay()
                        Demanddictionary = self.run.getDemands()
                        self.shower_vector_1 = adding_unit_uses(Demanddictionary,'shower')
                        self.toilet_vector_1 = adding_unit_uses(Demanddictionary,'toilet')
                        self.kitchen_tap_vector_1 = adding_unit_uses(Demanddictionary,'kitchen_tap')
                        self.handbasin_tap_vector_1 = adding_unit_uses(Demanddictionary,'handbasin_tap')                    
                        self.bath_vector_1 = adding_unit_uses(Demanddictionary,'bath')
                        self.washing_machine_vector_1 = adding_unit_uses(Demanddictionary,'washing_machine')
                        self.dishwasher_vector_1 = adding_unit_uses(Demanddictionary,'dish_washer')
                        self.evapcooler_vector_1 = adding_unit_uses(Demanddictionary,'evap_cooler')
                        
                        #Outports for connecting to Builing
                        self.Bathtub[0] = sum(self.bath_vector_1[:self.vector_end_index+1])/1000
                        self.Shower[0] = sum(self.shower_vector_1[:self.vector_end_index+1])/1000
                        self.Toilet[0] = sum(self.toilet_vector_1[:self.vector_end_index+1])/1000
                        self.Kitchen_Tap[0] = sum(self.kitchen_tap_vector_1[:self.vector_end_index+1])/1000
                        self.Handbasin_Tap[0] = sum(self.handbasin_tap_vector_1[:self.vector_end_index+1])/1000
                        self.Washing_Machine[0] = sum(self.washing_machine_vector_1[:self.vector_end_index+1])/1000
                        self.Dishwasher[0] = sum(self.dishwasher_vector_1[:self.vector_end_index+1])/1000
                        self.Evap_Cooler[0] = sum(self.evapcooler_vector_1[:self.vector_end_index+1])/1000
                        
                        #print 'Not first Timestep ending exactely '+str(len(self.dishwasher_vector_1[:self.vector_end_index+1]))
                        
                        #restting list index
                        self.startingsum = self.vector_end_index
                
                #counting overall simulationtime
                self.overall_time += dt/3600./24.
                
                #Outports for Checking Demand
                self.CheckBathtub[0] = self.Bathtub[0]
                self.CheckShower[0] = self.Shower[0]
                self.CheckToilet[0] = self.Toilet[0]
                self.CheckKitchen_Tap[0] = self.Kitchen_Tap[0]
                self.CheckHandbasin_Tap[0] = self.Handbasin_Tap[0]
                self.CheckWashing_Machine[0] = self.Washing_Machine[0]
                self.CheckDishwasher[0] = self.Dishwasher[0]
                self.CheckEvap_Cooler[0] = self.Evap_Cooler[0]
                
                #print dt
                #print [dt/3600, dt/3600.]
                #print [self.startingsum, self.vector_end_index]
                
            elif dt > 3600*24:
                
                print "Error: The Time step of simulation can't be greater than one day!!"
                
            else:
                
                print "Error: The Time step of simulation must be in full hours when greater one hour and smaller one day!!"
                
                
        elif self.model_selected == "Simple_Model":
            
            #Outports for Checking Demand
            ratio_handbasin_divided_by_kitchen_and_handbasin_tap = 2.5 / (3 + 2.5)
            demand_bathtub_ld_and_capita = 2.
            demand_shower_ld_and_capita = 34.
            demand_toilet_ld_and_capita = 19. 
            demand_kitchentap_ld_and_capita = 21. * (1-ratio_handbasin_divided_by_kitchen_and_handbasin_tap)
            demand_handbasintap_ld_and_capita = 21. * ratio_handbasin_divided_by_kitchen_and_handbasin_tap
            demand_washingmachine_ld_and_capita = 22.
            demand_dishwasher_ld_and_capita = 1.
            demand_evapcooler_ld_and_capita = 0.
            
            self.Bathtub[0] = demand_bathtub_ld_and_capita * self.number_people_res /3600/1000*dt/24
            self.Shower[0] = demand_shower_ld_and_capita * ( self.number_people_res + self.number_people_com ) /3600/1000*dt/24
            self.Toilet[0] = demand_toilet_ld_and_capita * ( self.number_people_res + self.number_people_com ) /3600/1000*dt/24
            self.Kitchen_Tap[0] = demand_kitchentap_ld_and_capita * ( self.number_people_res + self.number_people_com ) /3600/1000*dt/24
            self.Handbasin_Tap[0] = demand_handbasintap_ld_and_capita * ( self.number_people_res + self.number_people_com ) /3600/1000*dt/24
            self.Washing_Machine[0] = demand_washingmachine_ld_and_capita* self.number_people_res /3600/1000*dt/24
            self.Dishwasher[0] = demand_dishwasher_ld_and_capita * ( self.number_people_res + self.number_people_com )/3600/1000*dt/24
            self.Evap_Cooler[0] = demand_evapcooler_ld_and_capita * self.number_people_com /3600/1000*dt/24
#            print [self.number_residential_units, self.number_commercial_units]            
#

#            print "compare "+str(self.Bathtub[0]+self.Shower[0]+self.Toilet[0] +self.Kitchen_Tap[0] +self.Handbasin_Tap[0] +self.Washing_Machine[0] +self.Dishwasher[0] +self.Evap_Cooler[0])
            
            #Outports for Checking Demand
            self.CheckBathtub[0] = self.Bathtub[0]
            self.CheckShower[0] = self.Shower[0]
            self.CheckToilet[0] = self.Toilet[0]
            self.CheckKitchen_Tap[0] = self.Kitchen_Tap[0]
            self.CheckHandbasin_Tap[0] = self.Handbasin_Tap[0]
            self.CheckWashing_Machine[0] = self.Washing_Machine[0]
            self.CheckDishwasher[0] = self.Dishwasher[0]
            self.CheckEvap_Cooler[0] = self.Evap_Cooler[0]
            
            #print [self.CheckBathtub[0], self.CheckShower[0] ,  self.CheckToilet[0],self.CheckKitchen_Tap[0], self.CheckHandbasin_Tap[0], self.CheckWashing_Machine[0], self.CheckDishwasher[0] ,  self.CheckEvap_Cooler[0]]
           # print [self.CheckBathtub[0]+ self.CheckShower[0] +  self.CheckToilet[0]+self.CheckKitchen_Tap[0]+ self.CheckHandbasin_Tap[0]+ self.CheckWashing_Machine[0]+ self.CheckDishwasher[0] +  self.CheckEvap_Cooler[0]]
        else:
            
            print "Model selected not valid!"
        return dt
    
    def getClassName(self):
#        print "getClassName"
        return "Demand_Model"

#def register(nr):
#    for c in pycd3.Node.__subclasses__():
#        nf = NodeFactory(c)
#        nf.__disown__()
#        nr.addNodeFactory(nf)





