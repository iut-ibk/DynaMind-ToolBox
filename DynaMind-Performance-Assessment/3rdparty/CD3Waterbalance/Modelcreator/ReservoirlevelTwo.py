# -*- coding: utf-8 -*-
"""
Created on Wed Dec 03 11:37:18 2014

@author: Acer
"""


#this class adds Stormwatertanks to certain clusters
from ReservoirlevelOne import ReservoirlevelOne
from Clusterlevel import Clusterlevel
from Global_counters import Global_counters
from Global_meaning_list import Global_meaning_list
from Singleton import Singleton
Global_counters = Global_counters.Instance()
Cluster = Clusterlevel.Instance()

def Listupdate(Veclarge,Vecsmall):
            ''' set operation - Veclarge\Vecsmall '''
            Veclarge = set(Veclarge)
            Vecsmall = set(Vecsmall)
            Veclarge.difference_update(Vecsmall)
            Veclarge = list(Veclarge)
            return Veclarge
            
@Singleton
class ReservoirlevelTwo:
    def __init__(self):
        self.ReservoirlevelTwo_list = []
        self.runoff_coll_list = []
        self.overflow_coll_list = []
        self.additionaldemand_from_pwr_coll_list = []
        self.numbers_of_large_gwr = []
        self.additional_demand_swr = []
        self.additional_demand_gwr = []
        self.len_resone_list = 0
        
        
        
        self.len_greywater_to_reservoir_coll_list = 0
        self.len_additionaldemand_from_gwr_coll_list = 0
        self.len_runoff_overflow_coll_list_to_swr = 0
        self.len_additionaldemand_from_swr_coll_list = 0
        self.len_greywater_to_reservoir_coll_list_already_connected = 0
        self.len_additionaldemand_from_gwr_coll_list_already_connected = 0
        self.len_runoff_overflow_coll_list_to_swr_already_connected = 0
        self.len_additionaldemand_from_swr_coll_list_already_connected = 0
        self.len_numbers_of_large_swr_level1 = 0
        self.len_numbers_of_large_gwr_level1 = 0
        
    def writeconnections(self, stormvec):
        '''
        Explaination in XML-Creator.md
        '''
        Reslevel1 = ReservoirlevelOne.Instance()
        for i in range(len(stormvec)):
            
            Reslevel1.writeconnections(stormvec[i][:-1])
            
            self.len_resone_list_before = self.len_resone_list
            self.len_resone_list = len(Reslevel1.ReservoirlevelOne_list)            
            
            #indexes for only connecting new blocks      
            self.len_greywater_to_reservoir_coll_list_before = self.len_greywater_to_reservoir_coll_list
            self.len_greywater_to_reservoir_coll_list = len(Cluster.greywater_to_reservoir_coll_list)
            self.len_additionaldemand_from_gwr_coll_list_before = self.len_additionaldemand_from_gwr_coll_list
            self.len_additionaldemand_from_gwr_coll_list = len(Cluster.additionaldemand_from_gwr_coll_list)
            self.len_runoff_overflow_coll_list_to_swr_before = self.len_runoff_overflow_coll_list_to_swr
            self.len_runoff_overflow_coll_list_to_swr = len(Cluster.runoff_overflow_coll_list_to_swr)
            self.len_additionaldemand_from_swr_coll_list_before = self.len_additionaldemand_from_swr_coll_list
            self.len_additionaldemand_from_swr_coll_list = len(Cluster.additionaldemand_from_swr_coll_list)
            self.len_greywater_to_reservoir_coll_list_already_connected_before = self.len_greywater_to_reservoir_coll_list_already_connected
            self.len_greywater_to_reservoir_coll_list_already_connected = len(Reslevel1.greywater_to_reservoir_coll_list_already_connected)
            self.len_additionaldemand_from_gwr_coll_list_already_connected_before = self.len_additionaldemand_from_gwr_coll_list_already_connected
            self.len_additionaldemand_from_gwr_coll_list_already_connected = len(Reslevel1.additionaldemand_from_gwr_coll_list_already_connected)
            self.len_runoff_overflow_coll_list_to_swr_already_connected_before = self.len_runoff_overflow_coll_list_to_swr_already_connected
            self.len_runoff_overflow_coll_list_to_swr_already_connected = len(Reslevel1.runoff_overflow_coll_list_to_swr_already_connected)
            self.len_additionaldemand_from_swr_coll_list_already_connected_before = self.len_additionaldemand_from_swr_coll_list_already_connected
            self.len_additionaldemand_from_swr_coll_list_already_connected = len(Reslevel1.additionaldemand_from_swr_coll_list_already_connected)
            self.len_numbers_of_large_swr_level1_before = self.len_numbers_of_large_swr_level1
            self.len_numbers_of_large_swr_level1 = len(Reslevel1.numbers_of_large_swr_level1)
            self.len_numbers_of_large_gwr_level1_before = self.len_numbers_of_large_gwr_level1
            self.len_numbers_of_large_gwr_level1 = len(Reslevel1.numbers_of_large_gwr_level1)
            
            #preparing inflow and outflowlists for reservoirs to prevent double connections
            self.gwr_in_still_not_connected = Listupdate(Cluster.greywater_to_reservoir_coll_list[self.len_greywater_to_reservoir_coll_list_before:],Reslevel1.greywater_to_reservoir_coll_list_already_connected[self.len_greywater_to_reservoir_coll_list_already_connected_before:])
            self.gwr_out_still_not_connected = Listupdate(Cluster.additionaldemand_from_gwr_coll_list[self.len_additionaldemand_from_gwr_coll_list_before:],Reslevel1.additionaldemand_from_gwr_coll_list_already_connected[self.len_additionaldemand_from_gwr_coll_list_already_connected_before:])
            self.swr_in_still_not_connected = Listupdate(Cluster.runoff_overflow_coll_list_to_swr[self.len_runoff_overflow_coll_list_to_swr_before:],Reslevel1.runoff_overflow_coll_list_to_swr_already_connected[self.len_runoff_overflow_coll_list_to_swr_already_connected_before:])
            self.swr_out_still_not_connected = Listupdate(Cluster.additionaldemand_from_swr_coll_list[self.len_additionaldemand_from_swr_coll_list_before:],Reslevel1.additionaldemand_from_swr_coll_list_already_connected[self.len_additionaldemand_from_swr_coll_list_already_connected_before:])
            
           
            if stormvec[i][-1] == 1:
                               
                
                self.additionaldemand_from_gw_coll_list = []
                self.gw_inflow_coll_list = []
                # adds collector for additional demand from gwr
                self.additionaldemand_coll_strings=[]
                if self.gwr_out_still_not_connected != []:
                    for m in range(len(self.gwr_out_still_not_connected)):
                        #if cluster is connected to greywaterreservoir
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(self.gwr_out_still_not_connected[m])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.additionaldemand_coll_strings.append(self.additionaldemandcollstrings)
                else:
                    m=-1
                    
                if Reslevel1.numbers_of_large_swr_level1[self.len_numbers_of_large_swr_level1_before:] != []:
                    counter_1 = 0
                    for n in range(len(Reslevel1.numbers_of_large_swr_level1))[self.len_numbers_of_large_swr_level1_before:]:
                        #if cluster is connected to greywaterreservoir
                        counter_1 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Stormwaterreservoir_'+str(Reslevel1.numbers_of_large_swr_level1[n])+'" port="Additional_Demand"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+m+1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.additionaldemand_coll_strings.append(self.additionaldemandcollstrings)
                else:
                    counter_1 = 0    
                    
                if Reslevel1.numbers_of_large_gwr_level1[self.len_numbers_of_large_gwr_level1_before:] != []:
                    counter_2 = 0
                    for mn in range(len(Reslevel1.numbers_of_large_gwr_level1))[self.len_numbers_of_large_gwr_level1_before:]:
                        #if cluster is connected to greywaterreservoir
                        counter_2 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Greywaterreservoir_'+str(Reslevel1.numbers_of_large_gwr_level1[mn])+'" port="Additional_Demand"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+counter_2+m+1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.additionaldemand_coll_strings.append(self.additionaldemandcollstrings)
                else:
                    counter_2 = 0   

                if m == -1 and counter_2 == 0 and counter_1 == 0 :
                    pass
                else:
                    #writes collector number in list that knows number of inports for later reference
                    Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors,counter_1+counter_2+m+1 ])
                    #writes collector number in list that knows connection for later reference
                    self.additionaldemand_from_gw_coll_list.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Additional from Clusterlevels, Reservoirs from Level1 and gives it to Greywaterreservoir (Reslevel2): '+str(counter_1+counter_2+m+1)+' Inports'])
                    Global_counters.number_of_collectors += 1
                    
                    
                # adds collector for gwr inflow
                self.gwr_inflow_strings = []
                for m in range(len(self.gwr_in_still_not_connected)):
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Collector_'+str(self.gwr_in_still_not_connected[m])+'" port="Outport"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.gwrinflowstring = ''
                    for o in range(5)[1:]:
                        exec 'self.gwrinflowstring += string'+str(o)
                    self.gwr_inflow_strings.append(self.gwrinflowstring)
                if self.gwr_in_still_not_connected == []:
                    print "You can't insert another Greywater Reservoir, since there's not gonna be any inflow!"
                else:
                    pass
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, m+1])
                #writes collector number in list that knows connection for later reference
                self.gw_inflow_coll_list.append(Global_counters.number_of_collectors)
                Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Greywater Collectors from Clusterlevel for Greywaterreservoirs (Reslevel2): '+str(m+1)+' Inports'])
                Global_counters.number_of_collectors += 1
                    
                    
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
                self.additional_demand_gwr.append(Global_counters.number_of_greywaterreservoirs)
                Global_counters.number_of_greywaterreservoirs += 1
                
            elif stormvec[i][-1] == 2:
                                
                self.swr_inflow_coll_list = []
                self.swr_outflow_coll_list = []
                
                #adds collector for stormwaterreservoir inflow
                self.swr_inflow_coll_strings=[]
                for m in range(len(self.swr_in_still_not_connected)):
                    string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                    string2='\t\t\t\t<source node="Collector_'+str(self.swr_in_still_not_connected[m])+'" port="Outport"/>\n' 
                    string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1)+'"/>\n' 
                    string4='\t\t\t</connection>\n' 
                    Global_counters.number_of_connections += 1
                    #writes all string in one and puts it in list
                    self.swrinflowcollstrings = ''
                    for o in range(5)[1:]:
                        exec 'self.swrinflowcollstrings += string'+str(o)
                    self.swr_inflow_coll_strings.append(self.swrinflowcollstrings)
                if self.swr_in_still_not_connected == []:
                    print "You can't insert another Stormwater Reservoir, since there's not gonna be any inflow!" 
                else:
                    pass                    
                #writes collector number in list that knows number of inports for later reference
                Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, m+1])
                #writes collector number in list that knows connection for later reference
                self.swr_inflow_coll_list.append(Global_counters.number_of_collectors)
                Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Runoff and Overflow Collectors that arent connected yet from Clusterlevel as Stormwaterreservoir Inflow (Reslevel2): '+str(m+1)+' Inports'])
                Global_counters.number_of_collectors += 1
                
                #adds additional demand
                self.swr_outflow_coll_strings=[]
                if self.swr_out_still_not_connected != []:
                    for m in range(len(self.swr_out_still_not_connected)):
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Collector_'+str(self.swr_out_still_not_connected[m])+'" port="Outport"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        #writes all string in one and puts it in list
                        self.swroutflowcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.swroutflowcollstrings += string'+str(o)
                        self.swr_outflow_coll_strings.append(self.swroutflowcollstrings)  
                else:
                    m=-1
                    
                if Reslevel1.numbers_of_large_swr_level1[self.len_numbers_of_large_swr_level1_before:] != []:
                    counter_1 = 0
                    for n in range(len(Reslevel1.numbers_of_large_swr_level1))[self.len_numbers_of_large_swr_level1_before:]:
                        #if cluster is connected to greywaterreservoir
                        counter_1 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Stormwaterreservoir_'+str(Reslevel1.numbers_of_large_swr_level1[n])+'" port="Additional_Demand"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(counter_1+m+1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                        
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.swr_outflow_coll_strings.append(self.additionaldemandcollstrings)
                else:
                    counter_1 = 0    
                    
                if Reslevel1.numbers_of_large_gwr_level1[self.len_numbers_of_large_gwr_level1_before:] != []:
                    counter_2 = 0
                    for mn in range(len(Reslevel1.numbers_of_large_gwr_level1))[self.len_numbers_of_large_gwr_level1_before:]:
                        #if cluster is connected to greywaterreservoir
                        counter_2 += 1
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="Greywaterreservoir_'+str(Reslevel1.numbers_of_large_gwr_level1[mn])+'" port="Additional_Demand"/>\n' 
                        string3='\t\t\t\t<sink node="Collector_'+str(Global_counters.number_of_collectors)+'" port="Inport_'+str(m+counter_1+counter_2+1)+'"/>\n' 
                        string4='\t\t\t</connection>\n' 
                        Global_counters.number_of_connections += 1
                                                
                        #writes all string in one and puts it in list
                        self.additionaldemandcollstrings = ''
                        for o in range(5)[1:]:
                            exec 'self.additionaldemandcollstrings += string'+str(o)
                        self.swr_outflow_coll_strings.append(self.additionaldemandcollstrings)
                else:
                    counter_2 = 0    
                                        
                
                if m == -1 and counter_2 == 0 and counter_1 == 0 :
                    pass
                else:   
                    #writes collector number in list that knows number of inports for later reference
                    Global_counters.number_of_collectors_ports_list.append([Global_counters.number_of_collectors, m+counter_1+counter_2+1 ])
                    #writes collector number in list that knows connection for later reference
                    self.swr_outflow_coll_list.append(Global_counters.number_of_collectors)
                    Global_meaning_list.collectors.append(['Collector_'+str(Global_counters.number_of_collectors), 'collects Additional Demand needed from Stormwaterreservoir (Reslevel2): '+str(m+counter_1+counter_2+1)+' Inports'])
                    Global_counters.number_of_collectors += 1
                
                
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
                self.additional_demand_swr.append(Global_counters.number_of_stormwaterreservoirs)
                Global_counters.number_of_stormwaterreservoirs += 1
            
            
            
            
            else:
                if Reslevel1.numbers_of_large_swr_level1[self.len_numbers_of_large_swr_level1_before:] != []:
                    for m in range(len(Reslevel1.numbers_of_large_swr_level1))[self.len_numbers_of_large_swr_level1_before:]:
                        self.additional_demand_swr.append( Reslevel1.numbers_of_large_swr_level1[m])
                else:
                    pass
                if Reslevel1.numbers_of_large_gwr_level1[self.len_numbers_of_large_gwr_level1_before:] != []:
                    for m in range(len(Reslevel1.numbers_of_large_gwr_level1))[self.len_numbers_of_large_gwr_level1_before:]:
                        self.additional_demand_gwr.append( Reslevel1.numbers_of_large_gwr_level1[m])
                else:
                    pass
            
            
   

                
            for m in range(len(Reslevel1.ReservoirlevelOne_list))[self.len_resone_list_before:]:    
                self.ReservoirlevelTwo_list.append(Reslevel1.ReservoirlevelOne_list[m])
            if stormvec[i][-1] == 1:    
                for m in range(len(self.additionaldemand_coll_strings)):
                    self.ReservoirlevelTwo_list.append(self.additionaldemand_coll_strings[m])
                for m in range(len(self.gwr_inflow_strings)):
                    self.ReservoirlevelTwo_list.append(self.gwr_inflow_strings[m])
                self.ReservoirlevelTwo_list.append(self.gwr_out_strings[0])
                self.ReservoirlevelTwo_list.append(self.gwr_in_strings[0])
            elif stormvec[i][-1] == 2:    
                for m in range(len(self.swr_inflow_coll_strings)):
                    self.ReservoirlevelTwo_list.append(self.swr_inflow_coll_strings[m])
                for m in range(len(self.swr_outflow_coll_strings)):
                    self.ReservoirlevelTwo_list.append(self.swr_outflow_coll_strings[m])
                self.ReservoirlevelTwo_list.append(self.swr_out_strings[0])
                self.ReservoirlevelTwo_list.append(self.swr_in_strings[0]) 
            else:
                pass

    
