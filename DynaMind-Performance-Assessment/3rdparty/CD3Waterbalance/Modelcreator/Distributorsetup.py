# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 13:21:57 2014

@author: Acer
"""

class Distributorsetup:
    
    def __init__(self, numberofDistributors, starting_value_i, **Distributorattributes ):
        self.numberofDistributors = numberofDistributors
        self.Distributorattributelist = []
        self.Distributornodelist = []
        for i in range(numberofDistributors+starting_value_i)[starting_value_i:]:
            exec 'self.Distributorattributelist.append({"Distributor_'+str(i)+'" : dict.copy(Distributorattributes)})'

        print str(numberofDistributors)+' Distributors have been created!'
        return
   
    def Setandwrite_attributes(self,numberofDistributors, starting_value_i, attributevector):
        for i in range(numberofDistributors+starting_value_i)[starting_value_i:]:
            self.Distributorattributelist[i-starting_value_i][str('Distributor_'+str(i))]["Number_of_Outports"] = attributevector[i-starting_value_i][0]
    
        for i in range(numberofDistributors+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="Distributor_'+str(i)+'" class="Distributor"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="Number_of_Outports" type="int" value="'+str(self.Distributorattributelist[i-starting_value_i][str('Distributor_'+str(i))]["Number_of_Outports"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t</node> \\n ' '''        
            alllines = ''
            for n in range(3):
                exec 'alllines += self.line'+str(n+1)
                
            self.Distributornodelist.append(alllines)

        return






