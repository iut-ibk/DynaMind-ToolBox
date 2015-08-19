# -*- coding: utf-8 -*-
"""
Created on Wed Dec 03 11:37:18 2014

@author: Acer
"""

from Global_counters import Global_counters
from Clusterlevel import Clusterlevel
from Global_meaning_list import Global_meaning_list
from Singleton import Singleton
Global_counters = Global_counters.Instance()


@Singleton
class ReservoirlevelOne:
    def __init__(self):
        
        self.ReservoirlevelOne_list = []
        self.additionaldemand_from_swr_coll_list = []
        self.numbers_of_large_gwr_level1 = []
        self.runoff_overflow_coll_list_to_swd = []
        self.numbers_of_large_swr_level1 = []
        self.additionaldemand_from_gwr_coll_list_already_connected = []
        self.greywater_to_reservoir_coll_list_already_connected = []
        self.additionaldemand_from_swr_coll_list_already_connected = []
        self.runoff_overflow_coll_list_to_swr_already_connected = []
        self.len_cluster_list = 0
        self.len_additionaldemand_from_swr_coll_list = 0
        self.len_runoff_overflow_coll_list_to_swr = 0
        self.len_greywater_to_reservoir_coll_list = 0
        self.len_additionaldemand_from_gwr_and_swr_coll_list = 0
        self.len_additionaldemand_from_gwr_coll_list = 0
        
    def writeconnections(self, greyvec):
        ''' 
        Explaination in XML-Creator.md
        '''
        Cluster = Clusterlevel.Instance()
        for i in range(len(greyvec)):

            Cluster.writeconnections(greyvec[i][:-1])
            
            self.len_cluster_list_before = self.len_cluster_list
            self.len_cluster_list = len(Cluster.allclusters)
            
            #indexes for only connecting new blocks
            self.len_additionaldemand_from_gwr_and_swr_coll_list_before = self.len_additionaldemand_from_gwr_and_swr_coll_list
            self.len_additionaldemand_from_gwr_and_swr_coll_list = len(Cluster.additionaldemand_from_gwr_and_swr_coll_list)
            #indexes for only connecting new blocks
            self.len_additionaldemand_from_gwr_coll_list_before = self.len_additionaldemand_from_gwr_coll_list
            self.len_additionaldemand_from_gwr_coll_list = len(Cluster.additionaldemand_from_gwr_coll_list)
            self.len_greywater_to_reservoir_coll_list_before = self.len_greywater_to_reservoir_coll_list
            self.len_greywater_to_reservoir_coll_list = len(Cluster.greywater_to_reservoir_coll_list)
            #indexes for only connecting new blocks
            self.len_runoff_overflow_coll_list_to_swr_before = self.len_runoff_overflow_coll_list_to_swr
            self.len_runoff_overflow_coll_list_to_swr =len(Cluster.runoff_overflow_coll_list_to_swr)
            self.len_additionaldemand_from_swr_coll_list_before = self.len_additionaldemand_from_swr_coll_list
            self.len_additionaldemand_from_swr_coll_list = len(Cluster.additionaldemand_from_swr_coll_list)

         
            if greyvec[i][-1] == 1:
                
                            
                self.additionaldemand_from_gw_coll_list = []
                self.gw_inflow_coll_list = []
                
                # adds collector for additional demand from gwr
                self.additionaldemand_coll_strings=[]
                if Cluster.additionaldemand_from_gwr_coll_list[self.len_additionaldemand_from_gwr_coll_list_before:] == []:
                    counter_1 = 0
                else: 
                    counter_1 = 0
                    for m in range(len(Cluster.additionaldemand_from_gwr_coll_list))[self.len_additionaldemand_from_gwr_coll_list_before:]:
                        #if cluster is connected to greywaterreservoir
                        counter_1 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(Cluster.additionaldemand_from_gwr_coll_list[m])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.additionaldemand_coll_strings.append(self.additionaldemandcollstrings)
                        self.additionaldemand_from_gwr_coll_list_already_connected.append(Cluster.additionaldemand_from_gwr_coll_list[m])
                
                if Cluster.additionaldemand_from_gwr_and_swr_coll_list[self.len_additionaldemand_from_gwr_and_swr_coll_list_before:] == []:
                    counter_2 = 0
                else: 
                    counter_2 = 0
                    for n in range(len(Cluster.additionaldemand_from_gwr_and_swr_coll_list))[self.len_additionaldemand_from_gwr_and_swr_coll_list_before:]:
                        #if cluster is connected to greywaterreservoir
                        counter_2 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(Cluster.additionaldemand_from_gwr_and_swr_coll_list[n])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.additionaldemand_coll_strings.append(self.additionaldemandcollstrings)
                if counter_1==0 and counter_2==0:
                    pass
                else:
                    #writes collector number in list that knows number of inports for later reference
                    Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_1+counter_2])
                    #writes collector number in list that knows connection for later reference
                    self.additionaldemand_from_gw_coll_list.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Additional from Clusterlevels and gives it to Greywaterreservoir (Reslevel1): '+str(counter_1+counter_2)+' Inports'])
                    Global_counters.number_of_collectors += 1
                    
                if Cluster.greywater_to_reservoir_coll_list[self.len_greywater_to_reservoir_coll_list_before:] == []:
                    counter_1 = 0
                else:
                    counter_1 = 0
                    # adds collector for gwr inflow
                    self.gwr_inflow_strings = []
                    for m in range(len(Cluster.greywater_to_reservoir_coll_list))[self.len_greywater_to_reservoir_coll_list_before:]:
                        counter_1 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(Cluster.greywater_to_reservoir_coll_list[m])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        #writes all string in one and puts it in list
                        self.gwrinflowstring = ''
                        for o in range(5)[1:]:
                            exec 'self.gwrinflowstring += string'+str(o)
                        self.gwr_inflow_strings.append(self.gwrinflowstring)
                        self.greywater_to_reservoir_coll_list_already_connected.append(Cluster.greywater_to_reservoir_coll_list[m])
                    
                    #writes collector number in list that knows number of inports for later reference
                    Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_1])
                    #writes collector number in list that knows connection for later reference
                    self.gw_inflow_coll_list.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Greywater Collectors from Clusterlevel for Greywaterreservoirs (Reslevel1): '+str(counter_1)+' Inports'])
                    Global_counters.number_of_collectors += 1
                    
                if self.additionaldemand_from_gw_coll_list==[] or self.gw_inflow_coll_list==[]:
                    print "There's no Greywater flowing into the Reservoir OR the treated Water is not being used"
                elif self.additionaldemand_from_gw_coll_list==[] and self.gw_inflow_coll_list==[]:
                    print "The Greywater Reservoir is not being used"
                else:
                    #adds Greywaterreservoir 
                    self.gwr_in_strings = []
                    self.gwr_out_strings = []
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Collector_'+str(self.additionaldemand_from_gw_coll_list[0])+'" port="Outport"/>\n' 
                    string3='\t\t\t\t<sink node="Greywaterreservoir_'+str(Global_counters.number_of_greywaterreservoirs)+'" port="Greywater_Out"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    string5='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string6='\t\t\t\t<source node="Collector_'+str(self.gw_inflow_coll_list[0])+'" port="Outport"/>\n' 
                    string7='\t\t\t\t<sink node="Greywaterreservoir_'+str(Global_counters.number_of_greywaterreservoirs)+'" port="Greywater_In"/>\n' 
                    string8='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.gwrinstring = ''
                    self.gwroutstring = ''
                    for o in range(5)[1:]:
                        exec 'self.gwrinstring += string'+str(o+4)
                        exec 'self.gwroutstring += string'+str(o)
                    self.gwr_in_strings.append(self.gwrinstring)
                    self.gwr_out_strings.append(self.gwroutstring)
                    self.numbers_of_large_gwr_level1.append(Global_counters.number_of_greywaterreservoirs)
                    Global_counters.number_of_greywaterreservoirs += 1
                
            elif greyvec[i][-1] == 2:
                
#                print Cluster.runoff_overflow_coll_list_to_swr
#                print Cluster.runoff_overflow_coll_list_to_swr[self.len_runoff_overflow_coll_list_to_swr_before:]
                
                self.swr_inflow_coll_list = []
                self.swr_outflow_coll_list = []

                #adds collector for stormwaterreservoir inflow
                self.swr_inflow_coll_strings=[]
                if Cluster.runoff_overflow_coll_list_to_swr[self.len_runoff_overflow_coll_list_to_swr_before:] !=[]:
                    counter_2 = 0
                    for n in range(len(Cluster.runoff_overflow_coll_list_to_swr))[self.len_runoff_overflow_coll_list_to_swr_before:]:
                        counter_2 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(Cluster.runoff_overflow_coll_list_to_swr[n])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_2)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.swrinflowcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.swrinflowcollstrings += string'+str(o)
                        self.swr_inflow_coll_strings.append(self.swrinflowcollstrings)
                        self.runoff_overflow_coll_list_to_swr_already_connected.append(Cluster.runoff_overflow_coll_list_to_swr[n])
                else:
                    counter_2 = 0
                
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_2])
                #writes collector number in list that knows connection for later reference
                self.swr_inflow_coll_list.append(Global_counters.number_of_collectors)
                Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Runoff and Overflow Collectors from Clusterlevel as Stormwaterreservoir Inflow (Reslevel1): '+str(counter_2)+' Inports'])
                Global_counters.number_of_collectors += 1
                
                #adds additional demand from clusterlevel
                self.swr_outflow_coll_strings=[]
                if Cluster.additionaldemand_from_swr_coll_list[self.len_additionaldemand_from_swr_coll_list_before:] != []:
                    counter_1 = 0
                    for m in range(len(Cluster.additionaldemand_from_swr_coll_list))[self.len_additionaldemand_from_swr_coll_list_before:]:
                        counter_1 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(Cluster.additionaldemand_from_swr_coll_list[m])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.swroutflowcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.swroutflowcollstrings += string'+str(o)
                        self.swr_outflow_coll_strings.append(self.swroutflowcollstrings)
                        self.additionaldemand_from_swr_coll_list_already_connected.append(Cluster.additionaldemand_from_swr_coll_list[m])
                else:
                    counter_1 = 0
                 
                if Cluster.additionaldemand_from_gwr_and_swr_coll_list[self.len_additionaldemand_from_gwr_and_swr_coll_list_before:] == []:
                    counter_2 = 0
                else:
                    counter_2 = 0
                    for n in range(len(Cluster.additionaldemand_from_gwr_and_swr_coll_list))[self.len_additionaldemand_from_gwr_and_swr_coll_list_before:]:
                        #if cluster is connected to greywaterreservoir
                        counter_2 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(Cluster.additionaldemand_from_gwr_and_swr_coll_list[n])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.swr_outflow_coll_strings.append(self.additionaldemandcollstrings)        
                
                if counter_1 == 0 and counter_2 == 0:
                    pass
                
                else:   
                    #writes collector number in list that knows number of inports for later reference
                    Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, counter_1+counter_2])
                    #writes collector number in list that knows connection for later reference
                    self.swr_outflow_coll_list.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Additional Demand needed from Stormwaterreservoir (Reslevel1): '+str(counter_1+counter_2)+' Inports'])
                    Global_counters.number_of_collectors += 1
                
                if self.swr_outflow_coll_list == []:
                    print "There's no outflow of the Stormwater Reservoir! Probably the CLusterattribute Vector isn't set properly! ([...,...,0]) - The Cluster is not connected to the Stormw. Res.!"
                else:
                    #adds Stormwaterreservoir
                    self.swr_in_strings = []
                    self.swr_out_strings = []
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Collector_'+str(self.swr_inflow_coll_list[0])+'" port="Outport"/>\n' 
                    string3='\t\t\t\t<sink node="Stormwaterreservoir_'+str(Global_counters.number_of_stormwaterreservoirs)+'" port="Stormwater_In"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    string5='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string6='\t\t\t\t<source node="Collector_'+str(self.swr_outflow_coll_list[0])+'" port="Outport"/>\n' 
                    string7='\t\t\t\t<sink node="Stormwaterreservoir_'+str(Global_counters.number_of_stormwaterreservoirs)+'" port="Stormwater_Out"/>\n' 
                    string8='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all strings in one and puts it in list
                    self.swrinstring = ''
                    self.swroutstring = ''
                    for o in range(5)[1:]:
                        exec 'self.swrinstring += string'+str(o+4)
                        exec 'self.swroutstring += string'+str(o)
                    self.swr_in_strings.append(self.swrinstring)
                    self.swr_out_strings.append(self.swroutstring)
                    self.numbers_of_large_swr_level1.append(Global_counters.number_of_stormwaterreservoirs)
                    Global_counters.number_of_stormwaterreservoirs += 1

            else: 
                pass




                

            for m in range(len(Cluster.allclusters))[self.len_cluster_list_before:]:    
                self.ReservoirlevelOne_list.append(Cluster.allclusters[m])
            if greyvec[i][-1] == 1:    
                for m in range(len(self.additionaldemand_coll_strings)):
                    self.ReservoirlevelOne_list.append(self.additionaldemand_coll_strings[m])
                for m in range(len(self.gwr_inflow_strings)):
                    self.ReservoirlevelOne_list.append(self.gwr_inflow_strings[m])
                self.ReservoirlevelOne_list.append(self.gwr_out_strings[0])
                self.ReservoirlevelOne_list.append(self.gwr_in_strings[0])
            elif greyvec[i][-1] == 2:    
                for m in range(len(self.swr_inflow_coll_strings)):
                    self.ReservoirlevelOne_list.append(self.swr_inflow_coll_strings[m])
                for m in range(len(self.swr_outflow_coll_strings)):
                    self.ReservoirlevelOne_list.append(self.swr_outflow_coll_strings[m])
                self.ReservoirlevelOne_list.append(self.swr_out_strings[0])
                self.ReservoirlevelOne_list.append(self.swr_in_strings[0]) 
            else:
                pass
            