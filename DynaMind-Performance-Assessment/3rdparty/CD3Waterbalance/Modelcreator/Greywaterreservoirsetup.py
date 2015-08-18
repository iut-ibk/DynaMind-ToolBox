

# -*- coding: utf-8 -*-
"""
Created on Wed Nov 12 14:59:00 2014

@author: Acer
"""


class Greywaterreservoirsetup:
    
    def __init__(self, numberofGreywatertanks, starting_value_i, **Greywaterresattributes ):
        self.numberofGreywatertanks = numberofGreywatertanks
        self.Greywaterresattributelist = []
        self.Greywaterresnodelist = []
        for i in range(numberofGreywatertanks+starting_value_i)[starting_value_i:]:
            exec 'self.Greywaterresattributelist.append({"Greywaterreservoir_'+str(i)+'" : dict.copy(Greywaterresattributes)})'

        print str(numberofGreywatertanks)+' Greywaterreservoirs have been created!'
        return
    
    def Setandwrite_attributes(self,numberofGreywatertanks, starting_value_i, attributevector):
        for i in range(numberofGreywatertanks+starting_value_i)[starting_value_i:]:
            self.Greywaterresattributelist[i-starting_value_i][str('Greywaterreservoir_'+str(i))]["Yield_of_Treatment"] = attributevector[i-starting_value_i][0]
            self.Greywaterresattributelist[i-starting_value_i][str('Greywaterreservoir_'+str(i))]["Storage_Volume"] = attributevector[i-starting_value_i][1]
    
        for i in range(numberofGreywatertanks+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="Greywaterreservoir_'+str(i)+'" class="Greywaterreservoir"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="Yield_of_Treatment_[-]" type="double" value="'+str(self.Greywaterresattributelist[i-starting_value_i][str('Greywaterreservoir_'+str(i))]["Yield_of_Treatment"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t\\t<parameter name="Storage_Volume_[m^3]" type="double" value="'+str(self.Greywaterresattributelist[i-starting_value_i][str('Greywaterreservoir_'+str(i))]["Storage_Volume"])+'"/> \\n ' '''
            exec '''self.line4='\\t\\t\\t</node> \\n ' '''        
            
            alllines = ''
            for n in range(4):
                exec 'alllines += self.line'+str(n+1)
            self.Greywaterresnodelist.append(alllines)
        
        return





    
    
    
    
    
    

    
    
    
    
    
    