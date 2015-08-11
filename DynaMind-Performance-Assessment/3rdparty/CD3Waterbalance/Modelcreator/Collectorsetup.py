# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 13:21:57 2014

@author: Acer
"""

class Collectorsetup:
    
    def __init__(self, numberofCollectors, starting_value_i, **Collectorattributes ):
        self.numberofCollectors = numberofCollectors
        self.Collectorattributelist = []
        self.Collectornodelist = []
        for i in range(numberofCollectors+starting_value_i)[starting_value_i:]:
            exec 'self.Collectorattributelist.append({"Collector_'+str(i)+'" : dict.copy(Collectorattributes)})'

        print str(numberofCollectors)+' Collectors have been created!'
        return
   
    def Setandwrite_attributes(self,numberofCollectors, starting_value_i, attributevector):
        for i in range(numberofCollectors+starting_value_i)[starting_value_i:]:
            self.Collectorattributelist[i-starting_value_i][str('Collector_'+str(i))]["Number_of_Inports"] = attributevector[i-starting_value_i][0]
    
        for i in range(numberofCollectors+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="Collector_'+str(i)+'" class="Collector"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="Number_of_Inports" type="int" value="'+str(self.Collectorattributelist[i-starting_value_i][str('Collector_'+str(i))]["Number_of_Inports"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t</node> \\n ' '''        
            alllines = ''
            for n in range(3):
                exec 'alllines += self.line'+str(n+1)
                
            self.Collectornodelist.append(alllines)

        return






