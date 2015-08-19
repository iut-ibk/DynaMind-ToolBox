# -*- coding: utf-8 -*-
"""
Created on Mon Jan 19 13:13:13 2015

@author: Acer
"""

import sys
sys.path.append('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\Modelcreator')
from Global_counters import Global_counters
from XML_Creator import XML_Creator
from To_XML_Creator_modified_Simulator import TheHoleLot
from Global_meaning_list import Global_meaning_list
Global_counters = Global_counters.Instance()

'''
CREATING THE XML
Supplyvec and Attributevecs explanation in the XML-Creator.md on Github in the doc folder 
'''

supplyvec= [[[[[[0,1],[0,1,1],1],[[1,1,1],[1,1,0],1],1],2]]]
Catchattrvec=[[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06,'without'],[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06,'without'],[1,1.8,10000,0,0.5,0.5,0.6,0.21,1.5,0.4,0.5,380,510,710,0.04,0.05,0.06,'without'],[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06,'without'],[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06,'without'],[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06,'without'],[1,1.8,10000,0,0.5,0.5,0.6,0.21,1.5,0.4,0.5,380,510,710,0.04,0.05,0.06,'without']]
Demandmodelattrvec =[[[5,40],[5], "Simple_Model"]]*5

def XML():
    
    #creating Connectionlist
    CreateXML = XML_Creator()
    #supplyvec = [[[[[[1],[1,1,1],1],1],1]]]
    CreateXML.WriteConnections(supplyvec)
    
    #creating Nodelist
    #Catchattrvec=[[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06,'without']] *Global_counters.number_of_catchments     
    Greywaterresattrvec = [[0.9,15]]*(Global_counters.number_of_greywaterreservoirs) 
    Greywaterattrvec = [[1,5]]*(Global_counters.number_of_greywatertanks) 
    Stormwaterresattrvec = [[0.9,15]]*(Global_counters.number_of_stormwaterreservoirs)
    Rainwaterattrvec = [[5]]*(Global_counters.number_of_raintanks)
    #Demandmodelattrvec = [[[10],[0], "Simple_Model"]]*Global_counters.number_of_demandmodels                      
    Gardenwaterattrvec = [[7,2,22,[18,6],"Smart_Watering"]]*Global_counters.number_of_gardenwateringmodules
    Simulationsetupvec = ["2000-Jan-01 00:00:00", "2001-Jan-01 00:00:00", "3600", "C:/Users/Acer/Documents/GitHub/CD3Waterbalance/Module/cd3waterbalancemodules.py"]
    Needtohaveinputsvec = ["C:/Users/Acer/Documents/GitHub/CD3Waterbalance/simulationwithpatterns/inputfiles/rain.ixx", "C:/Users/Acer/Documents/GitHub/CD3Waterbalance/simulationwithpatterns/inputfiles/evapo.ixx", "13", "20.5"]
    CreateXML.WriteNodes(Catchattrvec, Greywaterattrvec, Stormwaterresattrvec, Rainwaterattrvec, Demandmodelattrvec, Greywaterresattrvec, Simulationsetupvec, Needtohaveinputsvec,Gardenwaterattrvec)
    
    #printing the Connectionlist to insert Fileouts
#    CreateXML.PrintConnections()
    
    #insert Fileouts()
#    Fileout_Connection_Name_List = [['Inport', "Stormwaterreservoir_0", "Stormwater_In","StormwaterInflow.txt"]]
#    CreateXML.Additional_Fileouts(Fileout_Connection_Name_List) 
    
    #safe the xml file
    CreateXML.SaveXML('/Users/christianurich/Documents/CD3Waterbalance/simulationwithpatterns/outputfiles/Garden.xml')
    
    #plot List of Collector Id's and there function
#    for i in range(len(Global_meaning_list.collectors)):
#        print Global_meaning_list.collectors[i]
        
    return

'''
RUNNING AND CHECKING THE XML
'''

def Simulator():
    Simulator = TheHoleLot()
    Simulator.Deleter('/Users/christianurich/Documents/CD3Waterbalance/simulationwithpatterns/outputfiles/')
    Simulator.runcd3('C:\Program Files (x86)\CityDrain3\\bin\cd3.exe', '/Users/christianurich/Documents/CD3Waterbalance/simulationwithpatterns/outputfiles/Garden.xml')
    Simulator.Fractioncalculator(Catchattrvec)#,[1,1.8,10000,0,0.5,0.5,0.6,0.21,1.5,0.4,0.5,380,510,710,0.04,0.05,0.06,'without']])
    Simulator.getoutputdata('/Users/christianurich/Documents/CD3Waterbalance/simulationwithpatterns/outputfiles')
    Simulator.getinputdata('/Users/christianurich/Documents/CD3Waterbalance/simulationwithpatterns/inputfiles')
    Simulator.Balance(['Greywatertanklevels', 'Greywaterreservoirlevels',  'Rainwatertanklevels', 'Stormwaterreservoirlevels','Gardenwateringstorage'], ['Evapo_Model', 'Rain_Model'], ['Actual_Infiltration', 'Potable_Water_Demand', 'Sewer', 'Stormwaterdrain'])
    Simulator.Plotter([20,8],[0,155], [0,20], [ 'Rainwatertanklevels','Greywatertanklevels', 'Greywaterreservoirlevels', 'Stormwaterreservoirlevels' ]) #, 'Rainwatertanklevels','Greywatertanklevels', 'Greywaterreservoirlevels', 'Stormwaterreservoirlevels'
    
    return

XML()
Simulator()

##Input description for Simulator!!!!!!

'''
Deleter - method delets all .txt - files is the City Drain output folder
Input: Deleter( - path to City Drain output folder - )
Example: Deleter('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulationwithpatterns\outputfiles')

runcd3 - method runs City Drain 
Input: runcd3( - path to CityDrain.exe (cd3.exe), path to XML - file that contains model - )
Example: runcd3('C:\Program Files (x86)\CityDrain3\\bin\cd3.exe', C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulationwithpatterns\outputfiles\Test.xml)
Attention \b in the program path has to be written \\b

Fractioncalculator - method calculates the total area of all Cachtments and the average perv, imperv to stormwaterdrain and imperv to storage area
Input: Fractioncalculator( - the catchmentattributevec that was used for setting up the Test.xml - )
Example: Fractioncalculator([[1,1.9,800,0.4,0.2,0.4,0.6,0.21,1.5,0.4,0.5,400,500,700,0.04,0.05,0.06],[1,1.8,10000,0,1,0,0.6,0.21,1.5,0.4,0.5,380,510,710,0.04,0.05,0.06]])

getoutputdata - method imports all data from the output .txt - files created by City Drain
Input: getoutputdata( - path to City Drain output folder - )
Example: getoutputdata('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulationwithpatterns\outputfiles')

getinputdata - method imports rain and evapotranspiration .ixx - files used for the City Drain simulation
Input: getoutputdata( - path to City Drain input folder - )
Example: getoutputdata('C:\Users\Acer\Documents\GitHub\CD3Waterbalance\simulationwithpatterns\inputfiles')

Balance - method checks the models mass balance by comparing input and output data
Input: Balance( - list of all storage output file names, list of filereader/pattern implemeter output file names, list of other neccessary output file names - )
Example: Balance(0.4, 1.5, ['Greywatertanklevels',  'Rainwatertanklevels', 'Stormwaterreservoirlevels'], ['Evapo_Model', 'Rain_Model'], ['Actual_Infiltration', 'Potable_Water_Demand', 'Sewer', 'Stormwaterdrain'])

Plotter - method plots any time series (file) wanted
Input: Plotter( -size (height and width), range of x to plot (in days), range ofy to plot (in m^3), list of file names to plot - )
Example: Plotter([12,10],[0,365], [0,1], ['Rain_Model', 'Stormwaterdrain', 'Evapo_Model', 'effective_rain','Indoor_Demand','Outdoor_Demand'])

Attention!!
The Methods getoutputdata, Balance and Plotter contain file names of the rain and evapotranspiration inputfiles, the rain and evapotr. files given out by the file reader/ pattern implementer and other file names.
Those do have to be adapted to the file names given to the corresponding files! See the methods code for closer description!
'''






