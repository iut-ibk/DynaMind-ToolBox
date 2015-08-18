# -*- coding: utf-8 -*-
"""
Created on Mon Jan 05 12:16:56 2015

@author: Acer
"""
import sys
sys.path.append('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\Modelcreator')
from Need_to_have_modelinput import Need_to_have_modelinput
from Simulation_basic_setup import Simulation_basic_setup
from Gardenwateringsetup import Gardenwateringsetup
from Catchmentsetup import Catchmentsetup
from Buildingsetup import Buildingsetup
from Stormwaterdrainsetup import Stormwaterdrainsetup
from Sewer2setup import Sewer2setup
from Potablewaterreservoirsetup import Potablewaterreservoirsetup
from Raintanksetup import Raintanksetup
from Greywatertanksetup import Greywatertanksetup
from Fileoutsetup import Fileoutsetup
from Collectorsetup import Collectorsetup
from Distributorsetup import Distributorsetup
from Demandmodelsetup import Demandmodelsetup
from Stormwaterreservoirsetup import Stormwaterreservoirsetup
from Greywaterreservoirsetup import Greywaterreservoirsetup
from Supplylevel import Supplylevel
from Global_counters import Global_counters
Global_counters = Global_counters.Instance()
Supplylevel = Supplylevel.Instance()
  

class XML_Creator():
    
    def __init__(self):
        
        self.Allstrings = []
        self.Nodelist = []
        self.Connectionlist = []
        self.Basicslist = []
        return
        
     
    
    def WriteConnections(self, supplyvec):
    
        ''' Creating Connections, Supplyvector necessary (explanation in the XML-Creator.md on Github in the doc folder) '''
        
        Supplylevel.writeconnections(supplyvec)
        
        return
        
        
        
    def WriteNodes(self, Catchattrvec, Greywaterattrvec, Stormwaterresattrvec , Rainwaterattrvec, Demandmodelattrvec, Greywaterresattrvec, Simulationsetupvec, Needtohaveinputsvec, Gardenwaterattrvec):

        Collectorattrvec = []
        for i in range(len(Global_counters.number_of_collectors_ports_list)):
            Collectorattrvec.append([Global_counters.number_of_collectors_ports_list[i][1]])
            
        Distributorattrvec = []
        for i in range(len(Global_counters.number_of_distributors_ports_list)):
            Distributorattrvec.append([Global_counters.number_of_distributors_ports_list[i][1]])
            
        Fileoutattrvec = []
        for i in range(len(Global_counters.numbers_names_of_fileouts_list)):
            Fileoutattrvec.append(Global_counters.numbers_names_of_fileouts_list[i][1])
            
        
        ''' Creating Nodes and "Header" '''
        
        Setupheader = Simulation_basic_setup(Simulationsetupvec[0], Simulationsetupvec[1], Simulationsetupvec[2], Simulationsetupvec[3])
        Needtohaveinputs = Need_to_have_modelinput(Needtohaveinputsvec[0], Needtohaveinputsvec[1], Needtohaveinputsvec[2], Needtohaveinputsvec[3])
        Catchments = Catchmentsetup(Global_counters.number_of_catchments ,0, Decay_Constant =1.9, Catchment_Area = 100, Fraktion_of_Impervious_Area_to_Reservoir_iAR= 0.4, Fraktion_of_Impervious_Area_to_Stormwater_Drain_iASD = 0.3, Fraktion_of_Pervious_Area_pA = 0.3, Number_of_Subareas = 1, Initial_Infiltration_Capacity = 0.6, Final_Infiltration_Capacity = 0.21, Depression_Loss = 1.5, Wetting_Loss = 0.4, Outdoor_Demand_Weighing_Factor = 0.5, Runoff_Runtime_iAR = 400, Runoff_Runtime_iASD = 500, Runoff_Runtime_pA = 700, Weighting_Coefficient_iAR = 0.04, Weighting_Coefficient_iASD = 0.05, Weighting_Coefficient_pA = 0.06,Catchment_with_or_without_Routing_with_or_without = "without")
        Catchments.Setandwrite_attributes(Global_counters.number_of_catchments,0,Catchattrvec)
        Buildings = Buildingsetup(Global_counters.number_of_buildings,0)
        Stormwaterdrain =  Stormwaterdrainsetup(Global_counters.number_of_stormwaterpipes,0)
        Sewers =  Sewer2setup(Global_counters.number_of_sewers,0)
        Potablewaterreservoir = Potablewaterreservoirsetup(Global_counters.number_of_potablwaterreservoirs,0)
        Greywatertank = Greywatertanksetup(Global_counters.number_of_greywatertanks,0,Yield_of_Treatment = 0.9, Storage_Volume = 15.0)
        Greywatertank.Setandwrite_attributes(Global_counters.number_of_greywatertanks,0,Greywaterattrvec)
        Raintank = Raintanksetup(Global_counters.number_of_raintanks,0, Storage_Volume = 10.0)
        Raintank.Setandwrite_attributes(Global_counters.number_of_raintanks,0,Rainwaterattrvec)
        Stormwaterreservoir = Stormwaterreservoirsetup(Global_counters.number_of_stormwaterreservoirs,0, Yield_of_Treatment = 0.9, Storage_Volume = 30.0)
        Stormwaterreservoir.Setandwrite_attributes(Global_counters.number_of_stormwaterreservoirs,0,Stormwaterresattrvec)
        Collectors=Collectorsetup(Global_counters.number_of_collectors,0,Number_of_Inports = 2)
        Collectors.Setandwrite_attributes(Global_counters.number_of_collectors,0,Collectorattrvec)
        Fileoutsneedtohave = Fileoutsetup(Global_counters.number_of_fileouts,0,out_file_name = 'Test.txt')
        Fileoutsneedtohave.Setandwrite_attributes(Global_counters.number_of_fileouts,0,Fileoutattrvec)
        Distributors = Distributorsetup(Global_counters.number_of_distributors,0,Number_of_Outports = 2)
        Distributors.Setandwrite_attributes(Global_counters.number_of_distributors,0,Distributorattrvec)
        Demandmodels = Demandmodelsetup(Global_counters.number_of_demandmodels,0,Commercial_Units = [4], Residential_Units=[4], Select_Model_Simple_Model_or_Complex_Model = "Simple_Model" )
        Demandmodels.Setandwrite_attributes(Global_counters.number_of_demandmodels,0,Demandmodelattrvec)
        Gardenwat = Gardenwateringsetup(Global_counters.number_of_gardenwateringmodules,0,Average_Watering_Frequency=7, Deviation_of_Frequency=2, Maximal_Watering_Flow_Rate=22, Smart_Watering_Start_Time_End_Time =[18,6], Watering_Method_Normal_Watering_or_Smart_Watering = "Normal_Watering")
        Gardenwat.Setandwrite_attributes(Global_counters.number_of_gardenwateringmodules,0,Gardenwaterattrvec)
        Greywaterreservoir = Greywaterreservoirsetup(Global_counters.number_of_greywaterreservoirs,0,Yield_of_Treatment = 0.9, Storage_Volume = 15.0)
        Greywaterreservoir.Setandwrite_attributes(Global_counters.number_of_greywaterreservoirs,0,Greywaterresattrvec)
        
        #writes header and need to have nodes in a list
        for i in range(len(Setupheader.Simulationsetupstring)):
            self.Basicslist.append(Setupheader.Simulationsetupstring[i])
        for i in range(len(Needtohaveinputs.Modelinputnodesstring)):
            self.Basicslist.append(Needtohaveinputs.Modelinputnodesstring[i])
        
        #wiritng all nodes in one list
        for i in range(len(Catchments.Catchmentnodelist)):
            self.Nodelist.append(Catchments.Catchmentnodelist[i])
        for i in range(len(Buildings.Buildingnodelist)):
            self.Nodelist.append(Buildings.Buildingnodelist[i])
        for i in range(len(Stormwaterdrain.Stormwaterpipenodelist)):
            self.Nodelist.append(Stormwaterdrain.Stormwaterpipenodelist[i])
        for i in range(len(Sewers.Sewer2nodelist)):
            self.Nodelist.append(Sewers.Sewer2nodelist[i])
        for i in range(len(Potablewaterreservoir.Potablewaterreservoirnodelist)):
            self.Nodelist.append(Potablewaterreservoir.Potablewaterreservoirnodelist[i])
        for i in range(len(Raintank.Raintanknodelist)):
            self.Nodelist.append(Raintank.Raintanknodelist[i])
        for i in range(len(Greywatertank.Greywatertanknodelist)):
            self.Nodelist.append(Greywatertank.Greywatertanknodelist[i])
        for i in range(len(Collectors.Collectornodelist)):
            self.Nodelist.append(Collectors.Collectornodelist[i])
        for i in range(len(Stormwaterreservoir.Stormwaterreservoirnodelist)):
            self.Nodelist.append(Stormwaterreservoir.Stormwaterreservoirnodelist[i])
        for i in range(len(Fileoutsneedtohave.Fileoutnodelist)):
            self.Nodelist.append(Fileoutsneedtohave.Fileoutnodelist[i])
        for i in range(len(Distributors.Distributornodelist)):
            self.Nodelist.append(Distributors.Distributornodelist[i])    
        for i in range(len(Demandmodels.Demand_Modelnodelist)):
            self.Nodelist.append(Demandmodels.Demand_Modelnodelist[i])
        for i in range(len(Gardenwat.Gardenmodulenodelist)):
            self.Nodelist.append(Gardenwat.Gardenmodulenodelist[i])
        for i in range(len(Greywaterreservoir.Greywaterresnodelist)):
            self.Nodelist.append(Greywaterreservoir.Greywaterresnodelist[i])    
            
        #writing all connections in one list
        for i in range(len(Supplylevel.Supplylevel_list)):
            self.Connectionlist.append(Supplylevel.Supplylevel_list[i])   
        return
        
        
        
    def PrintConnections(self):
        for i in range(len(self.Connectionlist)):    
            print self.Connectionlist[i] 
        return
    
#    Connection_Name_List = [['Inport', "Raintank_0", "Collected_Water"],['Outport', "Raintank_0", "Current_Volume"]]
    
    def Additional_Fileouts(self, Connection_Name_List):
        ''' Function to add additional Fileouts
            [["Inport" or "Outport", "Raintank_0", "Current_Volume","Name of txt file"],...]'''
        Fileout_numbers_names = []
        for i in range(len(Connection_Name_List)):
            
            if Connection_Name_List[i][0] == 'Inport':
            
                for u in range(len(self.Connectionlist)):
    
                    searchstring = '\t\t\t<connection id="'+str(u)+'">\n\t\t\t\t<source node="'+str(self.Connectionlist[u][self.Connectionlist[u].find('<source node="')+14:self.Connectionlist[u].find('" port=')])+'" port="'+str(self.Connectionlist[u][self.Connectionlist[u].find('" port=')+8:self.Connectionlist[u].find('"/>\n\t\t\t\t<sink')])+'"/>\n\t\t\t\t<sink node="'+str(Connection_Name_List[i][1])+'" port="'+str(Connection_Name_List[i][2])+'"/>\n\t\t\t</connection>\n'

                    if searchstring in self.Connectionlist[u]:
                        
                        #changing old connection
                        start_cut = self.Connectionlist[u].index('<sink')
                        stop_cut = self.Connectionlist[u].index('/>\n\t\t\t</connection>\n')+2
                        sink_new_connection = self.Connectionlist[u][start_cut:stop_cut]
                        self.Connectionlist[u] = self.Connectionlist[u].replace(self.Connectionlist[u][start_cut:stop_cut],'<sink node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="in"/>')
                        
                        #inserting new connection
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="out"/>\n' 
                        string3='\t\t\t\t'+sink_new_connection+'\n' 
                        string4='\t\t\t</connection>\n'
                        Global_counters.number_of_connections += 1
                        
                        Fileout_numbers_names.append([Global_counters.number_of_fileouts, Connection_Name_List[i][1]])
                        Global_counters.numbers_names_of_fileouts_list.append([Global_counters.number_of_fileouts,Connection_Name_List[i][1]])
                        self.Connectionlist.append(string1+string2+string3+string4)
                        
                    else:
                        pass
                    
            elif Connection_Name_List[i][0] == 'Outport':
                for u in range(len(self.Connectionlist)):
                    
                    searchstring = '\t\t\t<connection id="'+str(u)+'">\n\t\t\t\t<source node="'+str(Connection_Name_List[i][1])+'" port="'+str(Connection_Name_List[i][2])+'"/>\n\t\t\t\t<sink node="'+str(self.Connectionlist[u][self.Connectionlist[u].find('<sink node="')+12:self.Connectionlist[u].find('" port=',80)])+'" port="'+str(self.Connectionlist[u][self.Connectionlist[u].find('" port=',80)+8:self.Connectionlist[u].find('"/>', self.Connectionlist[u].find('" port=',80)+8)])+'"/>\n\t\t\t</connection>\n'

                    if searchstring in self.Connectionlist[u]:
                         
                        #changing old connection
                        start_cut = self.Connectionlist[u].index('<sink')
                        stop_cut = self.Connectionlist[u].index('/>\n\t\t\t</connection>\n')+2
                        sink_new_connection = self.Connectionlist[u][start_cut:stop_cut]
                        self.Connectionlist[u] = self.Connectionlist[u].replace(self.Connectionlist[u][start_cut:stop_cut],'<sink node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="in"/>')
                        
                        #inserting new connection
                        string1='\t\t\t<connection id="'+str(Global_counters.number_of_connections)+'">\n' 
                        string2='\t\t\t\t<source node="FileOut_'+str(Global_counters.number_of_fileouts)+'" port="out"/>\n' 
                        string3='\t\t\t\t'+sink_new_connection+'\n' 
                        string4='\t\t\t</connection>\n'
                        Global_counters.number_of_connections += 1
                        
                        Fileout_numbers_names.append([Global_counters.number_of_fileouts, Connection_Name_List[i][1]])
                        Global_counters.numbers_names_of_fileouts_list.append([Global_counters.number_of_fileouts,Connection_Name_List[i][1]])
                        self.Connectionlist.append(string1+string2+string3+string4)
                        
                    else:
                        pass
                        
            else:
                print "Wrong Input!!"
            Global_counters.number_of_fileouts += 1 
        Fileoutattrvec = []
        for i in range(len(Connection_Name_List)):
            Fileoutattrvec.append(Connection_Name_List[i][3])
        AddedFileouts = Fileoutsetup(len(Connection_Name_List), Fileout_numbers_names[0][0], out_file_name = 'Test.txt')        
        AddedFileouts.Setandwrite_attributes(len(Connection_Name_List), Fileout_numbers_names[0][0], Fileoutattrvec)
        
        for i in range(len(AddedFileouts.Fileoutnodelist)):
            self.Nodelist.append(AddedFileouts.Fileoutnodelist[i])
        return 
      
      
      
    def SaveXML(self,directory = 'C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulationwithpatterns\outputfiles\Test.xml'):
        ''' creating XML - List '''
    
        #Basic Setup list
        for i in range(len(self.Basicslist)):
            self.Allstrings.append(self.Basicslist[i])
        
        #Node List
        for i in range(len(self.Nodelist)):
            self.Allstrings.append(self.Nodelist[i])
        
        #Connectionlist
        self.Allstrings.append('\t\t</nodelist>\n')
        self.Allstrings.append('\t\t<connectionlist>\n')
        
        for i in range(len(self.Connectionlist)):
            self.Allstrings.append(self.Connectionlist[i])
        
        #Finish file
        self.Allstrings.append('\t\t</connectionlist>\n')
        self.Allstrings.append('\t</model>\n')
        self.Allstrings.append('</citydrain>\n')
        
        
        ''' Writing List in an XML File '''
        
        outFile = open(directory, 'w')
        for i in range(len(self.Allstrings)):
            outFile.write( self.Allstrings[i])
        outFile.close()
        print str(len(self.Connectionlist))+' Connections have been created!'
        print 'The xml file has been created and saved!'        
        
        return 
        
        
    
    
    
    
    
     