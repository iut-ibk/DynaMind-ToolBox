# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 16:42:52 2014

@author: Acer
"""

from Buildinglevel import Buildinglevel
from Global_counters import Global_counters
from Global_meaning_list import Global_meaning_list
from Singleton import Singleton
Global_counters = Global_counters.Instance()

@Singleton
class Clusterlevel:
    def __init__(self):
        self.allclusters = []
        self.greywater_to_sewer_coll_list = []
        self.greywater_to_reservoir_coll_list = []
        self.additionaldemand_from_gwr_coll_list = []
        self.additionaldemand_from_swr_coll_list = []
        self.additionaldemand_from_pwr_coll_list = []
        self.additionaldemand_from_gwr_and_swr_coll_list = []
        self.raintankstorage_coll_list = []
        self.decision_gwr_swr_pwr = []
        self.decision_sewer_gwr = []
        self.numbers_builidng_catchments = []
        self.len_building_list = 0
        self.runoff_overflow_coll_list_to_swr=[]
        self.runoff_overflow_coll_list_to_swd=[]
        self.index_building_list = 0
        self.index_greywatertank_list = 0
        self.index_numbers_of_catchments_list = 0
        
    def writeconnections(self, clusterbuildingvec):
        '''
        Explaination in XML-Creator.md
        '''
        Buildings = Buildinglevel.Instance()
        #circels through all elements of the clusterbuildingvector, calling the Buildingclass, writing the Buildinglevel connections
        for i in range(len(clusterbuildingvec)):
            Global_counters.number_of_clusters += 1
            #adding collectors to runoff from catchment, overflowraintank, greywater, additional demand (from raintanks), raintank storage check
            #and one street for each cluster (as a catchment)
            for run in range(clusterbuildingvec[i][2]):            
                Buildings.writeconnections(clusterbuildingvec[i][:2])
                
                #takes care of only appending new strings in this levels list
                self.len_building_list_before = self.len_building_list
                self.len_building_list=len(Buildings.Buildinglevel_connection_list)
                
                #indexes for only connecting new blocks
                self.index_building_list_before = self.index_building_list
                self.index_building_list = len(Buildings.numbers_of_buildings_list)
                self.index_greywatertank_list_before = self.index_greywatertank_list
                self.index_greywatertank_list = len(Buildings.numbers_of_greywatertanks_list)
                self.index_numbers_of_catchments_list_before = self.index_numbers_of_catchments_list
                self.index_numbers_of_catchments_list = len(Buildings.numbers_of_catchments_list)
                
                
                # adds catchment as a street
                self.street_strings=[]
                string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                string2='\t\t\t\t<source node="Catchment_w_Routing_'+str(Global_counters.number_of_catchments)+'" port="Runoff"/>\n' 
                string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_1"/>\n' 
                string4='\t\t\t</connection>\n' 
                Global_counters.number_of_connections += 1
                Global_counters.number_of_catchments += 1
                #writes all string in one and puts it in list
                self.streetstrings = ''
                for o in range(5)[1:]:
                    exec 'self.streetstrings += string'+str(o)
                self.street_strings.append(self.streetstrings)
    
                # adds collector for runoff from catchment
                self.runoff_coll_strings=[]
                for m in range(len(Buildings.numbers_of_catchments_list))[self.index_numbers_of_catchments_list_before:]:
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Catchment_w_Routing_'+str(Buildings.numbers_of_catchments_list[m])+'" port="Runoff"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+2-self.index_building_list_before)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.runoffstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.runoffstrings += string'+str(o)
                    self.runoff_coll_strings.append(self.runoffstrings)
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(clusterbuildingvec[i][0])+1])
                #writes collector number in list that knows connection for later reference
                if Buildings.numbers_of_buildings_connected_to_stormw[-1] != "not connected to SWR <type 'int'>" and Buildings.numbers_of_buildings_connected_to_stormw[-1] != "not connected to SWR <type 'list'>" :
                    self.runoff_overflow_coll_list_to_swr.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Runoff to Stormwaterreservoir from Catchments including Street on Clusterlevel: '+str(len(clusterbuildingvec[i][0])+1)+' Inports', 'Cluster '+str(Global_counters.number_of_clusters)]) 
                
                else:
                    self.runoff_overflow_coll_list_to_swd.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Runoff to Stormwaterdrain from Catchments including Street on Clusterlevel: '+str(len(clusterbuildingvec[i][0])+1)+' Inports', 'Cluster '+str(Global_counters.number_of_clusters)])   
                Global_counters.number_of_collectors +=1

                # adds collector for greywater
                self.greywater_coll_strings=[]
                for m in range(len(Buildings.numbers_of_buildings_list))[self.index_building_list_before:]:
                    if type(Buildings.numbers_of_buildings_contributing_gw_list[m]) == str:
                        if Buildings.numbers_of_buildings_contributing_gw_list[m] == "not_contributing to gw <type 'int'>":
                            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                            string2='\t\t\t\t<source node="Building_'+str(Buildings.numbers_of_buildings_list[m])+'" port="Greywater"/>\n' 
                            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1-self.index_building_list_before)+'"/>\n' 
                            string4='\t\t\t</connection>\n' 
                            Global_counters.number_of_connections += 1
                        else:
                            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                            string2='\t\t\t\t<source node="Greywatertank_'+str(Buildings.numbers_of_greywatertanks_list[m][1])+'" port="Greywater_Overflow"/>\n' 
                            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1-self.index_building_list_before)+'"/>\n' 
                            string4='\t\t\t</connection>\n' 
                            Global_counters.number_of_connections += 1
                    else:
                        if type(Buildings.numbers_of_buildings_contributing_gw_list[m]) == int:
                            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                            string2='\t\t\t\t<source node="Building_'+str(Buildings.numbers_of_buildings_list[m])+'" port="Greywater"/>\n' 
                            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1-self.index_building_list_before)+'"/>\n' 
                            string4='\t\t\t</connection>\n' 
                            Global_counters.number_of_connections += 1
                        else:
                            string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                            string2='\t\t\t\t<source node="Greywatertank_'+str(Buildings.numbers_of_greywatertanks_list[m][1])+'" port="Greywater_Overflow"/>\n' 
                            string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1-self.index_building_list_before)+'"/>\n' 
                            string4='\t\t\t</connection>\n' 
                            Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.greywaterstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.greywaterstrings += string'+str(o)
                    self.greywater_coll_strings.append(self.greywaterstrings)
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(clusterbuildingvec[i][0])])
                #writes collector number in list that knows connection for later reference
                    
                if Buildings.numbers_of_buildings_contributing_gw_list[-1] == "not_contributing to gw <type 'int'>" or Buildings.numbers_of_buildings_contributing_gw_list[-1] == "not_contributing to gw <type 'list'>": 
                    self.greywater_to_sewer_coll_list.append(Global_counters.number_of_collectors)
                    self.decision_sewer_gwr.append('sewer')
                else:
                    self.greywater_to_reservoir_coll_list.append(Global_counters.number_of_collectors)
                    self.decision_sewer_gwr.append('gwr')
                Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Greywater from Households or Greywatertanksoverflow on Cluster Level: '+str(len(clusterbuildingvec[i][0]))+' Inports'])
                Global_counters.number_of_collectors +=1
    
                # adds collector for overflow of Raintanks
                self.overflow_coll_strings=[]
                for m in range(len(Buildings.numbers_of_buildings_list))[self.index_building_list_before:]:
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Raintank_'+str(Buildings.numbers_of_buildings_list[m])+'" port="Overflow"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1-self.index_building_list_before)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.overflowstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.overflowstrings += string'+str(o)
                    self.overflow_coll_strings.append(self.overflowstrings)
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(clusterbuildingvec[i][0])])
                #writes collector number in list that knows connection for later reference
                
                if Buildings.numbers_of_buildings_connected_to_stormw[-1] != "not connected to SWR <type 'int'>" and Buildings.numbers_of_buildings_connected_to_stormw[-1] != "not connected to SWR <type 'list'>" :
                    self.runoff_overflow_coll_list_to_swr.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Overflow from Raintanks to Stormwaterreservoir on Cluster Level: '+str(len(clusterbuildingvec[i][0]))+' Inports']) 
                    
                else:
                    self.runoff_overflow_coll_list_to_swd.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Overflow from Raintanks to Stormwaterdrain on Cluster Level: '+str(len(clusterbuildingvec[i][0]))+' Inports'])   
                    
                Global_counters.number_of_collectors +=1
                
                # adds collector for checking stored volume Raintanks on a cluster level
                self.raintankstorage_coll_strings=[]
                for m in range(len(Buildings.numbers_of_buildings_list))[self.index_building_list_before:]:
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Raintank_'+str(Buildings.numbers_of_buildings_list[m])+'" port="Current_Volume"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1-self.index_building_list_before)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.raintankstoragestrings = ''
                    for o in range(5)[1:]:
                        exec 'self.raintankstoragestrings += string'+str(o)
                    self.raintankstorage_coll_strings.append(self.raintankstoragestrings)
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(clusterbuildingvec[i][0])])
                #writes collector number in list that knows connection for later reference
                self.raintankstorage_coll_list.append(Global_counters.number_of_collectors)
                Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Current_Volume from Raintanks on Cluster Level: '+str(len(clusterbuildingvec[i][0]))+' Inports'])
                Global_counters.number_of_collectors +=1
                
               
    
                # adds collector for additionaldemand of Raintanks or Greywatertanks
                self.additionaldemand_coll_strings=[]
                counter_1 = 0
                for m in range(len(Buildings.numbers_of_greywatertanks_list))[self.index_greywatertank_list_before:]:
                    if type(Buildings.numbers_of_greywatertanks_list[m]) == int:
                        counter_1 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Raintank_'+str(Buildings.numbers_of_greywatertanks_list[m])+'" port="Additional_Demand"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        
                    else:
                        counter_1 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Greywatertank_'+str(Buildings.numbers_of_greywatertanks_list[m][1])+'" port="Additional_Demand"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                        string4='\t\t\t</connection>\n'
                        
                    Global_counters.number_of_connections += 1    
                    #writes all string in one and puts it in list
                    self.additionaldemandstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.additionaldemandstrings += string'+str(o)
                    self.additionaldemand_coll_strings.append(self.additionaldemandstrings)
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, len(clusterbuildingvec[i][0])])
                #writes collector number in list that knows connection for later reference
                if Buildings.numbers_of_buildings_using_gw_list[-1] == "not using gw <type 'list'>" or Buildings.numbers_of_buildings_using_gw_list[-1] == "not using gw <type 'int'>":
                    if Buildings.numbers_of_buildings_connected_to_stormw[-1] == "not connected to SWR <type 'int'>" or Buildings.numbers_of_buildings_connected_to_stormw[-1] == "not connected to SWR <type 'list'>":
                        self.additionaldemand_from_pwr_coll_list.append(Global_counters.number_of_collectors)
                        self.decision_gwr_swr_pwr.append('pwr')
    
                    else:
                        self.additionaldemand_from_swr_coll_list.append(Global_counters.number_of_collectors)
                        self.decision_gwr_swr_pwr.append('swr')
      
                else:
                    if Buildings.numbers_of_buildings_connected_to_stormw[-1] == "not connected to SWR <type 'int'>" or Buildings.numbers_of_buildings_connected_to_stormw[-1] == "not connected to SWR <type 'list'>":
                        self.additionaldemand_from_gwr_coll_list.append(Global_counters.number_of_collectors)
                        self.decision_gwr_swr_pwr.append('gwr')
                    else:
                        self.additionaldemand_from_gwr_and_swr_coll_list.append(Global_counters.number_of_collectors)
                        self.decision_gwr_swr_pwr.append('gwr+swr')
                #print self.decision_gwr_swr_pwr
                #print self.additionaldemand_from_swr_coll_list
                Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Additional Demand from Greywatertanks or Raintanks on Cluster Level: '+str(len(clusterbuildingvec[i][0]))+' Inports'])
                Global_counters.number_of_collectors +=1

                #writing holdlevelstrings and added Cluster strings in one vector called allclusters
                for m in range(len(Buildings.Buildinglevel_connection_list))[self.len_building_list_before:]:    
                    self.allclusters.append(Buildings.Buildinglevel_connection_list[m])
                for m in range(len(self.street_strings)):    
                    self.allclusters.append(self.street_strings[m])    
                for m in range(len(self.runoff_coll_strings)):    
                    self.allclusters.append(self.runoff_coll_strings[m])
                for m in range(len(self.greywater_coll_strings)):    
                    self.allclusters.append(self.greywater_coll_strings[m])
                for m in range(len(self.overflow_coll_strings)):    
                    self.allclusters.append(self.overflow_coll_strings[m])
                for m in range(len(self.raintankstorage_coll_strings)):    
                    self.allclusters.append(self.raintankstorage_coll_strings[m])
                for m in range(len(self.additionaldemand_coll_strings)):    
                    self.allclusters.append(self.additionaldemand_coll_strings[m])

