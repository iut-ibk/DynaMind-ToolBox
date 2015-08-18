# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 13:21:57 2014

@author: Acer
"""

class Raintanksetup:
    
    def __init__(self, numberofRaintanks, starting_value_i, **Raintankattributes ):
        self.numberofRaintanks = numberofRaintanks
        self.Raintankattributelist = []
        self.Raintanknodelist = []
        for i in range(numberofRaintanks+starting_value_i)[starting_value_i:]:
            exec 'self.Raintankattributelist.append({"Raintank_'+str(i)+'" : dict.copy(Raintankattributes)})'

        print str(numberofRaintanks)+' Raintanks have been created!'
        return
   
    def Setandwrite_attributes(self,numberofRaintanks, starting_value_i, attributevector):
        for i in range(numberofRaintanks+starting_value_i)[starting_value_i:]:
            self.Raintankattributelist[i-starting_value_i][str('Raintank_'+str(i))]["Storage_Volume"] = attributevector[i-starting_value_i][0]
    
        for i in range(numberofRaintanks+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="Raintank_'+str(i)+'" class="Raintank"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="Storage_Volume_[m^3]" type="double" value="'+str(self.Raintankattributelist[i-starting_value_i][str('Raintank_'+str(i))]["Storage_Volume"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t</node> \\n ' '''        
            alllines = ''
            for n in range(3):
                exec 'alllines += self.line'+str(n+1)
                
            self.Raintanknodelist.append(alllines)

        return






