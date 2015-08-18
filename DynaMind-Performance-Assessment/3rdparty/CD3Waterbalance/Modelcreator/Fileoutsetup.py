# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 13:21:57 2014

@author: Acer
"""

class Fileoutsetup:
    
    def __init__(self, numberofFileouts, starting_value_i, **Fileoutattributes ):
        self.numberofFileouts = numberofFileouts
        self.Fileoutattributelist = []
        self.Fileoutnodelist = []
        for i in range(numberofFileouts+starting_value_i)[starting_value_i:]:
            exec 'self.Fileoutattributelist.append({"Fileout_'+str(i)+'" : dict.copy(Fileoutattributes)})'
            
        print str(numberofFileouts)+' Fileouts have been created!'
        return
   
    def Setandwrite_attributes(self,numberofFileouts, starting_value_i, attributevector):
        for i in range(numberofFileouts+starting_value_i)[starting_value_i:]:
            self.Fileoutattributelist[i-starting_value_i][str('Fileout_'+str(i))]["out_file_name"] = attributevector[i-starting_value_i]
    
        for i in range(numberofFileouts+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="FileOut_'+str(i)+'" class="FileOut"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="out_file_name" type="string" value="'+str(self.Fileoutattributelist[i-starting_value_i][str('Fileout_'+str(i))]["out_file_name"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t</node> \\n ' '''        
            alllines = ''
            for n in range(3):
                exec 'alllines += self.line'+str(n+1)
                
            self.Fileoutnodelist.append(alllines)

        return 






