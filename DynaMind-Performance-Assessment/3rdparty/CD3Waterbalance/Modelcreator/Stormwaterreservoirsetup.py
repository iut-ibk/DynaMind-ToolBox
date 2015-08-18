# -*- coding: utf-8 -*-
"""
Created on Mon Jan 05 12:50:54 2015

@author: Acer
"""


class Stormwaterreservoirsetup:
    
    def __init__(self, numberofStormwaterreservoirs, starting_value_i, **Stormwaterreservoirattributes ):
        self.numberofStormwaterreservoirs = numberofStormwaterreservoirs
        self.Stormwaterreservoirattributelist = []
        self.Stormwaterreservoirnodelist = []
        for i in range(numberofStormwaterreservoirs+starting_value_i)[starting_value_i:]:
            exec 'self.Stormwaterreservoirattributelist.append({"Stormwaterreservoir_'+str(i)+'" : dict.copy(Stormwaterreservoirattributes)})'

        print str(numberofStormwaterreservoirs)+' Stormwaterreservoirs have been created!'
        return
   
    def Setandwrite_attributes(self,numberofStormwaterreservoirs, starting_value_i, attributevector):
        for i in range(numberofStormwaterreservoirs+starting_value_i)[starting_value_i:]:
            self.Stormwaterreservoirattributelist[i-starting_value_i][str('Stormwaterreservoir_'+str(i))]["Yield_of_Treatment"] = attributevector[i-starting_value_i][0]
            self.Stormwaterreservoirattributelist[i-starting_value_i][str('Stormwaterreservoir_'+str(i))]["Storage_Volume"] = attributevector[i-starting_value_i][1]
    
        for i in range(numberofStormwaterreservoirs+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="Stormwaterreservoir_'+str(i)+'" class="Stormwaterreservoir"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="Yield_of_Treatment [-]" type="double" value="'+str(self.Stormwaterreservoirattributelist[i-starting_value_i][str('Stormwaterreservoir_'+str(i))]["Yield_of_Treatment"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t\\t<parameter name="Storage_Volume_[m^3]" type="double" value="'+str(self.Stormwaterreservoirattributelist[i-starting_value_i][str('Stormwaterreservoir_'+str(i))]["Storage_Volume"])+'"/> \\n ' '''
            exec '''self.line4='\\t\\t\\t</node> \\n ' '''        
            
            alllines = ''
            for n in range(4):
                exec 'alllines += self.line'+str(n+1)
            self.Stormwaterreservoirnodelist.append(alllines)
        
        return





