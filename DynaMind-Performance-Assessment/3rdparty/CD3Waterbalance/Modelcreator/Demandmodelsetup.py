# -*- coding: utf-8 -*-
"""
Created on Thu Dec 11 15:47:44 2014

@author: Acer
"""

class Demandmodelsetup:
       
    def __init__(self, numberofDemand_Models, starting_value_i, **Demand_Modelattributes ):
        self.numberofDemand_Models = numberofDemand_Models
        self.Demand_Modelattributelist = []
        self.Demand_Modelnodelist = []
        for i in range(numberofDemand_Models+starting_value_i)[starting_value_i:]:
            exec 'self.Demand_Modelattributelist.append({"Demand_Model_'+str(i)+'" : dict.copy(Demand_Modelattributes)})'
        
        print str(numberofDemand_Models)+' Demand_Models have been created!'
        return
    
    def Setandwrite_attributes(self,numberofDemand_Models, starting_value_i, attributevector):
        for i in range(numberofDemand_Models+starting_value_i)[starting_value_i:]:
            self.Demand_Modelattributelist[i-starting_value_i][str('Demand_Model_'+str(i))]["Commercial_Vector"] = attributevector[i-starting_value_i][1]
            self.Demand_Modelattributelist[i-starting_value_i][str('Demand_Model_'+str(i))]["Residential_Vector"] = attributevector[i-starting_value_i][0]
            self.Demand_Modelattributelist[i-starting_value_i][str('Demand_Model_'+str(i))]["Select_Model_Simple_Model_or_Stochastic_Model"] = attributevector[i-starting_value_i][2]
    
        for i in range(numberofDemand_Models+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="Demand_Model_'+str(i)+'" class="Demand_Model"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="Commercial_Vector" type="string" value="'+str(self.Demand_Modelattributelist[i-starting_value_i][str('Demand_Model_'+str(i))]["Commercial_Vector"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t\\t<parameter name="Residential_Vector" type="string" value="'+str(self.Demand_Modelattributelist[i-starting_value_i][str('Demand_Model_'+str(i))]["Residential_Vector"])+'"/> \\n ' '''
            exec '''self.line4='\\t\\t\\t\\t<parameter name="Select_Model_(Simple_Model_or_Stochastic_Model)" type="string" value="'+str(self.Demand_Modelattributelist[i-starting_value_i][str('Demand_Model_'+str(i))]["Select_Model_Simple_Model_or_Stochastic_Model"])+'"/> \\n ' '''
            exec '''self.line5='\\t\\t\\t</node> \\n ' '''        
            
            alllines = ''
            for n in range(5):
                exec 'alllines += self.line'+str(n+1)
            self.Demand_Modelnodelist.append(alllines)
        
        return




    