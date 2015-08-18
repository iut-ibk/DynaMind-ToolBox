# -*- coding: utf-8 -*-
"""
Created on Tue Dec 02 11:20:04 2014

@author: Acer
"""
from ReservoirlevelTwo import ReservoirlevelTwo
from Global_counters import Global_counters
from Global_meaning_list import Global_meaning_list
from Singleton import Singleton
from Clusterlevel import Clusterlevel
from Buildinglevel import Buildinglevel
Global_counters = Global_counters.Instance()
Cluster = Clusterlevel.Instance()
Buildings = Buildinglevel.Instance()

@Singleton
class Supplylevel:
    def __init__(self):
        self.Supplylevel_list = []
        self.Supplylevel_loop_list = []
        self.blackwater_coll_list = []
        self.potablewater_coll_list = []
        self.infiltr_coll_list = []
        self.outdoordemand = []
        self.indoor_demand_coll_list = []
        self.need_to_have_filout_list = []
        self.fileout_numbers_names_list = []

        self.number_of_buildings = 0
        self.number_of_greywatertanks = 0
        self.number_of_greywaterreservoirs = 0
        self.number_of_stormwaterreservoirs = 0        
        self.len_restwo_list = 0
        self.len_additional_demand_swr = 0
        self.len_additional_demand_gwr = 0
        self.len_additionaldemand_from_pwr_coll_list = 0
        self.len_runoff_overflow_coll_list_to_swd = 0
        self.len_greywater_to_sewer_coll_list = 0
        
        
        
    def writeconnections(self, supplyvec):

        '''
        Explaination in XML-Creator.md
        '''

        Stormwater = ReservoirlevelTwo.Instance()
        for i in range(len(supplyvec)):
            
            Stormwater.writeconnections(supplyvec[i])
            
            self.len_restwo_list_before = self.len_restwo_list
            self.len_restwo_list = len(Stormwater.ReservoirlevelTwo_list)
            
            #stores values for next loop run, gets rid of double connections
            self.number_of_buildings_before = self.number_of_buildings
            self.number_of_buildings = Global_counters.number_of_buildings
            self.number_of_greywatertanks_before = self.number_of_greywatertanks
            self.number_of_greywatertanks = Global_counters.number_of_greywatertanks
            self.number_of_greywaterreservoirs_before = self.number_of_greywaterreservoirs
            self.number_of_greywaterreservoirs = Global_counters.number_of_greywaterreservoirs 
            self.number_of_stormwaterreservoirs_before = self.number_of_stormwaterreservoirs
            self.number_of_stormwaterreservoirs = Global_counters.number_of_stormwaterreservoirs           

            #indexes for only connecting new blocks 
            self.len_additional_demand_swr_before = self.len_additional_demand_swr
            self.len_additional_demand_swr = len(Stormwater.additional_demand_swr)
            self.len_additional_demand_gwr_before = self.len_additional_demand_gwr
            self.len_additional_demand_gwr = len(Stormwater.additional_demand_gwr)
            self.len_additionaldemand_from_pwr_coll_list_before = self.len_additionaldemand_from_pwr_coll_list
            self.len_additionaldemand_from_pwr_coll_list = len(Cluster.additionaldemand_from_pwr_coll_list)
            self.len_runoff_overflow_coll_list_to_swd_before = self.len_runoff_overflow_coll_list_to_swd
            self.len_runoff_overflow_coll_list_to_swd = len(Cluster.runoff_overflow_coll_list_to_swd)
            self.len_greywater_to_sewer_coll_list_before = self.len_greywater_to_sewer_coll_list
            self.len_greywater_to_sewer_coll_list = len(Cluster.greywater_to_sewer_coll_list)
                
            '''
            adding Potable Water Reservoir
            '''            
            
            #adds collector for potable water reservoir of nonpot(Additional Demand)
            self.nonpot_coll_strings=[]
            if Stormwater.additional_demand_swr[self.len_additional_demand_swr_before:] != []:
                counter_1 = 0
                for n in range(len(Stormwater.additional_demand_swr))[self.len_additional_demand_swr_before:]:
                    counter_1 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Stormwaterreservoir_'+str(Stormwater.additional_demand_swr[n])+'" port="Additional_Demand"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    
                    #writes all string in one and puts it in list
                    self.nonpotcollstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.nonpotcollstrings += string'+str(o)
                    self.nonpot_coll_strings.append(self.nonpotcollstrings)
            else:
                counter_1 = 0
               
            if Stormwater.additional_demand_gwr[self.len_additional_demand_gwr_before:] != []:
                counter_2 = 0  
                for p in range(len(Stormwater.additional_demand_gwr))[self.len_additional_demand_gwr_before:]:
                    counter_2 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Greywaterreservoir_'+str(Stormwater.additional_demand_gwr[p])+'" port="Additional_Demand"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_2+counter_1)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    
                    #writes all string in one and puts it in list
                    self.nonpotcollstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.nonpotcollstrings += string'+str(o)
                    self.nonpot_coll_strings.append(self.nonpotcollstrings)
            else:
                counter_2 = 0

            #adds collector for potable water reservoir of nonpot (from buildings, not connected to GWR/SWR)
            if Cluster.additionaldemand_from_pwr_coll_list[self.len_additionaldemand_from_pwr_coll_list_before:] != []:
                counter_3 = 0
                for m in range(len(Cluster.additionaldemand_from_pwr_coll_list))[self.len_additionaldemand_from_pwr_coll_list_before:]:
                    counter_3 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Collector_'+str(Cluster.additionaldemand_from_pwr_coll_list[m])+'" port="Outport"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2+counter_3)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    
                    #writes all string in one and puts it in list
                    self.nonpotcollstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.nonpotcollstrings += string'+str(o)
                    self.nonpot_coll_strings.append(self.nonpotcollstrings)
            else:
                counter_3 = 0

            #adds potable water reservoir connecting nonpot
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Outport"/>\n' 
            string3='\t\t\t\t<sink node="Potable_Water_Reservoir_'+str(Global_counters.number_of_potablwaterreservoirs)+'" port="Non_Potable_Water_Demand"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.nonpotcollstrings = ''
            for o in range(5)[1:]:
                exec 'self.nonpotcollstrings += string'+str(o)
            self.nonpot_coll_strings.append(self.nonpotcollstrings)
            #writes collector number in list that knows number of inports for later reference
            Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_1+counter_2+counter_3])
            #writes collector number in list that knows connection for later reference
            Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Additonal Demands from small Tanks (Buildings) and GW-/SW-Reservoirs for nonpot Demand from PWR: '+str(counter_1+counter_2+counter_3)+' Inports'])
            Global_counters.number_of_collectors += 1
            
            #adds collector for potable water reservoir of pot
            self.pot_coll_strings=[]
            counter_1 = 0
            for m in range(self.number_of_buildings)[self.number_of_buildings_before:]:
                counter_1 += 1
                string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                string2='\t\t\t\t<source node="Building_'+str(m)+'" port="Potable_Demand"/>\n' 
                string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                string4='\t\t\t</connection>\n' 
                Global_counters.number_of_connections += 1
                #writes all string in one and puts it in list
                self.potcollstrings = ''
                for o in range(5)[1:]:
                    exec 'self.potcollstrings += string'+str(o)
                self.pot_coll_strings.append(self.potcollstrings)
            
            #adds potable water reservoir connecting pot
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Outport"/>\n' 
            string3='\t\t\t\t<sink node="Potable_Water_Reservoir_'+str(Global_counters.number_of_potablwaterreservoirs)+'" port="Potable_Water_Demand"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.potcollstrings = ''
            for o in range(5)[1:]:
                exec 'self.potcollstrings += string'+str(o)
            self.pot_coll_strings.append(self.potcollstrings)  
            #writes collector number in list that knows number of inports for later reference
            Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_1])
            #writes collector number in list that knows connection for later reference
            Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects potable Demand from Buildings for pot Demand from PWR: '+str(counter_1)+' Inports'])
            Global_counters.number_of_collectors += 1
            Global_counters.number_of_potablwaterreservoirs += 1
            
            '''
            adding Stormwaterdrain
            '''  

            #adds collector for runoff and overflow collectors and stormwateroverflow
            self.stormdrain_strings=[]
            counter_1 = 0
            if self.number_of_stormwaterreservoirs - self.number_of_stormwaterreservoirs_before != 0:
                for n in range(self.number_of_stormwaterreservoirs)[self.number_of_stormwaterreservoirs_before:]:
                    counter_1 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Stormwaterreservoir_'+str(n)+'" port="Overflow"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.stormdrainstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.stormdrainstrings += string'+str(o)
                    self.stormdrain_strings.append(self.stormdrainstrings)
            else:
                counter_1 = 0
                
            counter_2 = 0
            if Cluster.runoff_overflow_coll_list_to_swd[self.len_runoff_overflow_coll_list_to_swd_before:] != []:
                for m in range(len(Cluster.runoff_overflow_coll_list_to_swd))[self.len_runoff_overflow_coll_list_to_swd_before:]:
                    counter_2 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Collector_'+str(Cluster.runoff_overflow_coll_list_to_swd[m])+'" port="Outport"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1 + counter_2 )+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.stormdrainstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.stormdrainstrings += string'+str(o)
                    self.stormdrain_strings.append(self.stormdrainstrings)
            else:
                counter_2 = 0
               
            #adds stormwaterdrain
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Outport"/>\n' 
            string3='\t\t\t\t<sink node="Stormwaterdrain_'+str(Global_counters.number_of_stormwaterpipes)+'" port="Runoff"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.stormdrainstrings = ''
            for o in range(5)[1:]:
                exec 'self.stormdrainstrings += string'+str(o)
            self.stormdrain_strings.append(self.stormdrainstrings)  
            #writes collector number in list that knows number of inports for later reference
            Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_1 + counter_2 ])
            #writes collector number in list that knows connection for later reference
            Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Additional Demand and Runoff leading to Stormwaterpipe: '+str(counter_1 + counter_2)+' Inports'])
            Global_counters.number_of_collectors += 1
            Global_counters.number_of_stormwaterpipes += 1
            
            '''
            adds sewer
            '''
            #collects waste water from Stormwaterreservoirs if present
            self.sewer_strings=[]
            counter_1 = 0
            if self.number_of_stormwaterreservoirs - self.number_of_stormwaterreservoirs_before != 0:
                for n in range(self.number_of_stormwaterreservoirs)[self.number_of_stormwaterreservoirs_before:]:
                    counter_1 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Stormwaterreservoir_'+str(n)+'" port="Wastewater"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.sewerstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.sewerstrings += string'+str(o)
                    self.sewer_strings.append(self.sewerstrings)
                    
            else:
                counter_1 = 0
            
            counter_2 = 0
            #collects Overflow from large Greywaterreservoirs    
            if self.number_of_greywaterreservoirs - self.number_of_greywaterreservoirs_before != 0:
                for mn in range(self.number_of_greywaterreservoirs)[self.number_of_greywaterreservoirs_before:]:
                    counter_2 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Greywaterreservoir_'+str(mn)+'" port="Greywater_Overflow"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.sewerstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.sewerstrings += string'+str(o)
                    self.sewer_strings.append(self.sewerstrings)
                    
            else:
                counter_2 = 0           
            
            counter_3 = 0
            #collects waste water from Greywatertanks if present    
            if self.number_of_greywatertanks - self.number_of_greywatertanks_before != 0:
                for m in range(self.number_of_greywatertanks)[self.number_of_greywatertanks_before:]:
                    counter_3 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Greywatertank_'+str(m)+'" port="Wastewater"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2+counter_3)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.sewerstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.sewerstrings += string'+str(o)
                    self.sewer_strings.append(self.sewerstrings)
                    
            else:
                counter_3 = 0
            
            counter_4 = 0
            #collects waste water from Greywaterreservoirs if present    
            if self.number_of_greywaterreservoirs - self.number_of_greywaterreservoirs_before != 0:
                for mv in range(self.number_of_greywaterreservoirs)[self.number_of_greywaterreservoirs_before:]:
                    counter_4 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Greywaterreservoir_'+str(mv)+'" port="Wastewater"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2+counter_3+counter_4)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.sewerstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.sewerstrings += string'+str(o)
                    self.sewer_strings.append(self.sewerstrings)
                    
            else:
                counter_4 = 0
            
            counter_5 = 0
            #collects Greywater form buildings/small greywatertanks that are not connected to bis reservoir    
            if Cluster.greywater_to_sewer_coll_list[self.len_greywater_to_sewer_coll_list_before:] != []:
                for p in range(len(Cluster.greywater_to_sewer_coll_list))[self.len_greywater_to_sewer_coll_list_before:]:
                    counter_5 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Collector_'+str(Cluster.greywater_to_sewer_coll_list[p])+'" port="Outport"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2+counter_3+counter_4+counter_5)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.sewerstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.sewerstrings += string'+str(o)
                    self.sewer_strings.append(self.sewerstrings)
                    
            else:
                counter_5 = 0
                
            counter_6 = 0
            #collects all black water from buildings
            for q in range(self.number_of_buildings)[self.number_of_buildings_before:]:
                    counter_6 += 1
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Building_'+str(q)+'" port="Blackwater"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2+counter_3+counter_4+counter_5+counter_6)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.sewerstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.sewerstrings += string'+str(o)
                    self.sewer_strings.append(self.sewerstrings)
                    
                    
            #adds sewer       
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Outport"/>\n' 
            string3='\t\t\t\t<sink node="Sewer2_'+str(Global_counters.number_of_sewers)+'" port="Blackwater"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.sewerstrings = ''
            for o in range(5)[1:]:
                exec 'self.sewerstrings += string'+str(o)
            self.sewer_strings.append(self.sewerstrings)
            
            #writes collector number in list that knows number of inports for later reference
            Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_1+counter_2+counter_3+counter_4+counter_5+counter_6])
            Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Black Water from Buildings, Greywater not connected to Reservoir, Waste Water from Greywaterreservoirs and Stormwaterreservoirs leading to Sewer: '+str(counter_1+counter_2+counter_3+counter_4+counter_5+counter_6)+' Inports'])            
            Global_counters.number_of_collectors += 1
            Global_counters.number_of_sewers += 1
         
            
            for m in range(len(Stormwater.ReservoirlevelTwo_list))[self.len_restwo_list_before:]:    
                self.Supplylevel_loop_list.append(Stormwater.ReservoirlevelTwo_list[m])
            for m in range(len(self.nonpot_coll_strings)):    
                self.Supplylevel_loop_list.append(self.nonpot_coll_strings[m])
            for m in range(len(self.pot_coll_strings)):    
                self.Supplylevel_loop_list.append(self.pot_coll_strings[m])    
            for m in range(len(self.stormdrain_strings)):    
                self.Supplylevel_loop_list.append(self.stormdrain_strings[m])
            for m in range(len(self.sewer_strings)):    
                self.Supplylevel_loop_list.append(self.sewer_strings[m])
            
   
        '''    
        connecting filereader (rain) and pattern implementer (evapo) to all Catchments (including catchment streets)
        '''
        self.modelinput_strings = []
        #filereader - fileout connection
        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
        string2='\t\t\t\t<source node="File_Reader_'+str(Global_counters.number_of_filereaders)+'" port="Outport"/>\n' 
        string3='\t\t\t\t<sink node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="in"/>\n' 
        string4='\t\t\t</connection>\n' 
        Global_counters.number_of_connections += 1
        #writes all string in one and puts it in list
        self.modelinputstrings = ''
        for o in range(5)[1:]:
            exec 'self.modelinputstrings += string'+str(o)
        self.modelinput_strings.append(self.modelinputstrings)
        
        #fileout - distributor connection
        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
        string2='\t\t\t\t<source node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="out"/>\n' 
        string3='\t\t\t\t<sink node="Distributor_'+str(Global_counters.number_of_distributors)+'" port="Inport"/>\n' 
        string4='\t\t\t</connection>\n' 
        Global_counters.number_of_connections += 1
        #writes all string in one and puts it in list
        self.modelinputstrings = ''
        for o in range(5)[1:]:
            exec 'self.modelinputstrings += string'+str(o)
        self.modelinput_strings.append(self.modelinputstrings)
        
        #distributor - catchment connection (rain)
        for q in range(Global_counters.number_of_catchments):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Distributor_'+str(Global_counters.number_of_distributors)+'" port="Outport_'+str(q+1)+'"/>\n' 
            string3='\t\t\t\t<sink node="Catchment_w_Routing_'+str(q)+'" port="Rain"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.modelinputstrings = ''
            for o in range(5)[1:]:
                exec 'self.modelinputstrings += string'+str(o)
            self.modelinput_strings.append(self.modelinputstrings)
        
        #writes collector number in list that knows number of inports for later reference
        Global_counters.numbers_names_of_fileouts_list.append([Global_counters.number_of_fileouts, 'Rain_Model.txt'])
        Global_counters.number_of_distributors_ports_list.append([Global_counters.number_of_distributors, Global_counters.number_of_catchments])
        Global_counters.number_of_filereaders += 1
        Global_counters.number_of_distributors += 1
        Global_counters.number_of_fileouts += 1
        
        #filereader - pattern_implementer connection
        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
        string2='\t\t\t\t<source node="File_Reader_'+str(Global_counters.number_of_filereaders)+'" port="Outport"/>\n' 
        string3='\t\t\t\t<sink node="Pattern_Impl_'+str(Global_counters.number_of_patternimplementers)+'" port="Inport"/>\n' 
        string4='\t\t\t</connection>\n' 
        Global_counters.number_of_connections += 1
        #writes all string in one and puts it in list
        self.modelinputstrings = ''
        for o in range(5)[1:]:
            exec 'self.modelinputstrings += string'+str(o)
        self.modelinput_strings.append(self.modelinputstrings)
        
        
        #pattern_implementer - fileout connection
        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
        string2='\t\t\t\t<source node="Pattern_Impl_'+str(Global_counters.number_of_patternimplementers)+'" port="Outport"/>\n' 
        string3='\t\t\t\t<sink node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="in"/>\n' 
        string4='\t\t\t</connection>\n' 
        Global_counters.number_of_connections += 1
        #writes all string in one and puts it in list
        self.modelinputstrings = ''
        for o in range(5)[1:]:
            exec 'self.modelinputstrings += string'+str(o)
        self.modelinput_strings.append(self.modelinputstrings)
        
        #fileout - distributor connection
        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
        string2='\t\t\t\t<source node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="out"/>\n' 
        string3='\t\t\t\t<sink node="Distributor_'+str(Global_counters.number_of_distributors)+'" port="Inport"/>\n' 
        string4='\t\t\t</connection>\n' 
        Global_counters.number_of_connections += 1
        #writes all string in one and puts it in list
        self.modelinputstrings = ''
        for o in range(5)[1:]:
            exec 'self.modelinputstrings += string'+str(o)
        self.modelinput_strings.append(self.modelinputstrings)
        
        #distributor - catchment connection (evapo)
        for q in range(Global_counters.number_of_catchments):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Distributor_'+str(Global_counters.number_of_distributors)+'" port="Outport_'+str(q+1)+'"/>\n' 
            string3='\t\t\t\t<sink node="Catchment_w_Routing_'+str(q)+'" port="Evapotranspiration"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.modelinputstrings = ''
            for o in range(5)[1:]:
                exec 'self.modelinputstrings += string'+str(o)
            self.modelinput_strings.append(self.modelinputstrings)
             
        #writes collector number in list that knows number of inports for later reference
        Global_counters.numbers_names_of_fileouts_list.append([Global_counters.number_of_fileouts, 'Evapo_Model.txt'])
        Global_counters.number_of_distributors_ports_list.append([Global_counters.number_of_distributors, Global_counters.number_of_catchments])
        Global_counters.number_of_patternimplementers += 1
        Global_counters.number_of_distributors += 1
        Global_counters.number_of_filereaders += 1
        Global_counters.number_of_fileouts += 1
        
        '''
        adds collector for gardenwateringmodel storage
        '''
        self.check_garden_strings = []
        for q in range(Global_counters.number_of_gardenwateringmodules):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="GardenWateringModel_'+str(q)+'" port="Check_Storage"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkgardensstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkgardensstrings += string'+str(o)
            self.check_garden_strings.append(self.checkgardensstrings)
             
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_gardenwateringmodules])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Gardenwateringstorage (for non watered demand at last time step): '+str(Global_counters.number_of_gardenwateringmodules)+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Gardenwateringstorage'])
        Global_counters.number_of_collectors += 1        
        
        '''
        adds collector for storagelevel of greywatertanks
        '''
        if Global_counters.number_of_greywatertanks != 0:
            self.check_greytanks_strings = []
            for q in range(Global_counters.number_of_greywatertanks):
                string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                string2='\t\t\t\t<source node="Greywatertank_'+str(q)+'" port="Current_Volume"/>\n' 
                string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
                string4='\t\t\t</connection>\n' 
                Global_counters.number_of_connections += 1
                #writes all string in one and puts it in list
                self.checkgreytanksstrings = ''
                for o in range(5)[1:]:
                    exec 'self.checkgreytanksstrings += string'+str(o)
                self.check_greytanks_strings.append(self.checkgreytanksstrings)
                 
            #writes collector number in list that knows number of inports for later reference
            Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_greywatertanks])
            Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Current_Volume of Greywatertanks (for checking levels): '+str(Global_counters.number_of_greywatertanks)+' Inports'])
            self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Greywatertanklevels'])
            Global_counters.number_of_collectors += 1
        else:
            pass
        
        '''
        adds collector for storagelevel of greywaterreservoirs
        '''
        if Global_counters.number_of_greywaterreservoirs != 0:
            self.check_greyres_strings = []
            for q in range(Global_counters.number_of_greywaterreservoirs):
                string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                string2='\t\t\t\t<source node="Greywaterreservoir_'+str(q)+'" port="Current_Volume"/>\n' 
                string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
                string4='\t\t\t</connection>\n' 
                Global_counters.number_of_connections += 1
                #writes all string in one and puts it in list
                self.checkgreyresstrings = ''
                for o in range(5)[1:]:
                    exec 'self.checkgreyresstrings += string'+str(o)
                self.check_greyres_strings.append(self.checkgreyresstrings)
                 
            #writes collector number in list that knows number of inports for later reference
            Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_greywaterreservoirs])
            Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Current_Volume of Greywaterreservoirs (for checking levels): '+str(Global_counters.number_of_greywaterreservoirs)+' Inports'])
            self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Greywaterreservoirlevels'])
            Global_counters.number_of_collectors += 1        
        else:
            pass
        
        '''
        adds collector for storagelevel of stormwatertanks
        '''
        if Global_counters.number_of_stormwaterreservoirs != 0:
            self.check_stormtanks_strings = []
            for q in range(Global_counters.number_of_stormwaterreservoirs):
                string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                string2='\t\t\t\t<source node="Stormwaterreservoir_'+str(q)+'" port="Current_Volume"/>\n' 
                string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
                string4='\t\t\t</connection>\n' 
                Global_counters.number_of_connections += 1
                #writes all string in one and puts it in list
                self.checkstormtanksstrings = ''
                for o in range(5)[1:]:
                    exec 'self.checkstormtanksstrings += string'+str(o)
                self.check_stormtanks_strings.append(self.checkstormtanksstrings)
                 
            #writes collector number in list that knows number of inports for later reference
            Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_stormwaterreservoirs])
            Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Current_Volume of Stormwaterreservoirs (for checking levels): '+str(Global_counters.number_of_stormwaterreservoirs)+' Inports'])
            self.need_to_have_filout_list.append( [Global_counters.number_of_collectors, 'Stormwaterreservoirlevels'])
            Global_counters.number_of_collectors += 1
        else:
            pass
        
        '''
        adds collector for storagelevel of rainwatertanks
        '''
        
        self.check_raintanks_strings = []
        for q in range(len(Cluster.raintankstorage_coll_list)):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Collector_'+str(Cluster.raintankstorage_coll_list[q])+'" port="Outport"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkraintanksstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkraintanksstrings += string'+str(o)
            self.check_raintanks_strings.append(self.checkraintanksstrings)
             
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(Cluster.raintankstorage_coll_list)])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Current_Volume Collectors from Clusterlevel of Raintanks (for checking levels): '+str(len(Cluster.raintankstorage_coll_list))+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Rainwatertanklevels'])
        Global_counters.number_of_collectors += 1
        
        '''
        adds collector for infiltration of catchments
        '''
        self.check_infiltration_strings = []
        for q in range(Global_counters.number_of_catchments):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Catchment_w_Routing_'+str(q)+'" port="Actual_Infiltration"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkinfiltrationstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkinfiltrationstrings += string'+str(o)
            self.check_infiltration_strings.append(self.checkinfiltrationstrings)
             
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_catchments])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Actual Infiltration of all Catchments: '+str(Global_counters.number_of_catchments)+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Actual_Infiltration'])
        Global_counters.number_of_collectors += 1

        '''
        adds collector for checking outdoordemand
        '''
        self.check_outdoordemand_strings = []
        for q in range(len(Buildings.numbers_of_catchments_list)):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Catchment_w_Routing_'+str(Buildings.numbers_of_catchments_list[q])+'" port="Outdoor_Demand_Check"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkoutdoordemandstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkoutdoordemandstrings += string'+str(o)
            self.check_outdoordemand_strings.append(self.checkoutdoordemandstrings)
             
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(Buildings.numbers_of_catchments_list)])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Outdoor Demand of all Catchments: '+str(len(Buildings.numbers_of_catchments_list))+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Outdoor_Demand'])
        Global_counters.number_of_collectors += 1
        
        '''
        adds collector for checking demandmodel
        '''
        self.check_demandmodel_bath_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Bathtub"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_bath_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Bathtub Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1
        
        self.check_demandmodel_shower_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Shower"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_shower_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Shower Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1
        
        self.check_demandmodel_kitchen_tap_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Kitchen_Tap"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_kitchen_tap_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Kitchen Tap Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1
        
        self.check_demandmodel_handbasin_tap_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Handbasin_Tap"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_handbasin_tap_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Handbasin Tap Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1
        
        self.check_demandmodel_toilet_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Toilet"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_toilet_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Toilet Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1
        
        self.check_demandmodel_washingmachine_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Washing_Machine"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_washingmachine_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Washing Machine Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1
        
        self.check_demandmodel_dishwasher_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Dishwasher"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_dishwasher_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Dishwasher Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1
        
        self.check_demandmodel_evapcooler_strings = []
        for q in range(Global_counters.number_of_demandmodels):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Demand_Model_'+str(q)+'" port="Outport_Check_Evapcooler"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkdemandmodelbathstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkdemandmodelbathstrings += string'+str(o)
            self.check_demandmodel_evapcooler_strings.append(self.checkdemandmodelbathstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_demandmodels])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Evapcooler Demand from all Demandmodels: '+str(Global_counters.number_of_demandmodels)+' Inports'])
        self.indoor_demand_coll_list.append(Global_counters.number_of_collectors)
        Global_counters.number_of_collectors += 1        
        
        #all indoordemands togehter
        self.check_indoordemand_strings = []
        for q in range(len(self.indoor_demand_coll_list)):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Collector_'+str(self.indoor_demand_coll_list[q])+'" port="Outport"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.checkindoordemandstrings = ''
            for o in range(5)[1:]:
                exec 'self.checkindoordemandstrings += string'+str(o)
            self.check_indoordemand_strings.append(self.checkindoordemandstrings)   
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(self.indoor_demand_coll_list)])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects entire Indoor Demand from all 8 individuell Collectors: '+str(len(self.indoor_demand_coll_list))+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Indoor_Demand'])
        Global_counters.number_of_collectors += 1
        
        '''
        adds collector for sewers
        '''
        self.sewers_strings =[]
        for q in range(Global_counters.number_of_sewers):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Sewer2_'+str(q)+'" port="Discharged_Volume"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.sewersstrings = ''
            for o in range(5)[1:]:
                exec 'self.sewersstrings += string'+str(o)
            self.sewers_strings.append(self.sewersstrings)
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_sewers])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects all Sewer outports: '+str(Global_counters.number_of_sewers)+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Sewer'])
        Global_counters.number_of_collectors += 1
        
        '''
        adds collector for Stormwaterdrains
        '''        
        self.stormdrains_strings =[]
        for q in range(Global_counters.number_of_stormwaterpipes):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Stormwaterdrain_'+str(q)+'" port="Discharged_Volume"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.stormdrainsstrings = ''
            for o in range(5)[1:]:
                exec 'self.stormdrainsstrings += string'+str(o)
            self.stormdrains_strings.append(self.stormdrainsstrings)
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_stormwaterpipes])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects all Stormwaterdrain outports: '+str(Global_counters.number_of_stormwaterpipes)+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Stormwaterdrain'])
        Global_counters.number_of_collectors += 1
        
        '''
        adds collector for Potable_Water_Resorvoirs
        '''    
        self.pot_strings =[]
        for q in range(Global_counters.number_of_potablwaterreservoirs):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Potable_Water_Reservoir_'+str(q)+'" port="Demand"/>\n' 
            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(q+1)+'"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            #writes all string in one and puts it in list
            self.potstrings = ''
            for o in range(5)[1:]:
                exec 'self.potstrings += string'+str(o)
            self.pot_strings.append(self.potstrings)
        #writes collector number in list that knows number of inports for later reference
        Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, Global_counters.number_of_potablwaterreservoirs])
        Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects all Potable_Water_Reservoir outports: '+str(Global_counters.number_of_potablwaterreservoirs)+' Inports'])
        self.need_to_have_filout_list.append([Global_counters.number_of_collectors, 'Potable_Water_Demand'])
        Global_counters.number_of_collectors += 1      
        
        '''
        ads need to have fileouts
        '''
        #adds fileouts to collectors
        self.fileout_strings =[]
        for q in range(len(self.need_to_have_filout_list)):
            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
            string2='\t\t\t\t<source node="Collector_'+str(self.need_to_have_filout_list[q][0])+'" port="Outport"/>\n' 
            string3='\t\t\t\t<sink node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="in"/>\n' 
            string4='\t\t\t</connection>\n' 
            Global_counters.number_of_connections += 1
            
            #writes all string in one and puts it in list
            self.fileoutstrings = ''
            for o in range(5)[1:]:
                exec 'self.fileoutstrings += string'+str(o)
            self.fileout_strings.append(self.fileoutstrings) 
            Global_counters.numbers_names_of_fileouts_list.append([Global_counters.number_of_fileouts, str(self.need_to_have_filout_list[q][1])+'.txt'])
            Global_counters.number_of_fileouts += 1
        
        
        
        
        
        for m in range(len(self.Supplylevel_loop_list)):
            self.Supplylevel_list.append(self.Supplylevel_loop_list[m])
        for m in range(len(self.modelinput_strings)):
            self.Supplylevel_list.append(self.modelinput_strings[m])  
        for m in range(len(self.check_garden_strings)):
            self.Supplylevel_list.append(self.check_garden_strings[m])   
        if Global_counters.number_of_greywatertanks != 0:
            for m in range(len(self.check_greytanks_strings)):
                self.Supplylevel_list.append(self.check_greytanks_strings[m])
        else:
            pass
        if Global_counters.number_of_greywaterreservoirs != 0:
            for m in range(len(self.check_greyres_strings)):
                self.Supplylevel_list.append(self.check_greyres_strings[m])
        else:
            pass 
        if Global_counters.number_of_stormwaterreservoirs != 0:
            for m in range(len(self.check_stormtanks_strings)):
                self.Supplylevel_list.append(self.check_stormtanks_strings[m])
        else:
            pass 
        for m in range(len(self.check_raintanks_strings)):
            self.Supplylevel_list.append(self.check_raintanks_strings[m])
        for m in range(len(self.check_infiltration_strings)):
            self.Supplylevel_list.append(self.check_infiltration_strings[m])
        for m in range(len(self.check_outdoordemand_strings)):
            self.Supplylevel_list.append(self.check_outdoordemand_strings[m])
        for m in range(len(self.check_demandmodel_bath_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_bath_strings[m])
        for m in range(len(self.check_demandmodel_shower_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_shower_strings[m])
        for m in range(len(self.check_demandmodel_kitchen_tap_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_kitchen_tap_strings[m])
        for m in range(len(self.check_demandmodel_handbasin_tap_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_handbasin_tap_strings[m])
        for m in range(len(self.check_demandmodel_toilet_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_toilet_strings[m])
        for m in range(len(self.check_demandmodel_washingmachine_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_washingmachine_strings[m])
        for m in range(len(self.check_demandmodel_dishwasher_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_dishwasher_strings[m])
        for m in range(len(self.check_demandmodel_evapcooler_strings)):
            self.Supplylevel_list.append(self.check_demandmodel_evapcooler_strings[m])
        for m in range(len(self.check_indoordemand_strings)):
            self.Supplylevel_list.append(self.check_indoordemand_strings[m])
        for m in range(len(self.sewers_strings)):
            self.Supplylevel_list.append(self.sewers_strings[m])
        for m in range(len(self.stormdrains_strings)):
            self.Supplylevel_list.append(self.stormdrains_strings[m])
        for m in range(len(self.pot_strings)):
            self.Supplylevel_list.append(self.pot_strings[m])
        for m in range(len(self.fileout_strings)):
            self.Supplylevel_list.append(self.fileout_strings[m])
            

  
    