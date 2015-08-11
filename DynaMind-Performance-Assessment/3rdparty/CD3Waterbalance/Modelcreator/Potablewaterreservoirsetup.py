
# -*- coding: utf-8 -*-
"""
Created on Wed Nov 12 14:59:00 2014

@author: Acer
"""


class Potablewaterreservoirsetup:
    
    def __init__(self, numberofPotablewaterreservoirs, starting_value_i, **Potablewaterreservoirattributes ):
        self.numberofPotablewaterreservoirs = numberofPotablewaterreservoirs
        self.Potablewaterreservoirattributelist = []
        self.Potablewaterreservoirnodelist = []
        for i in range(numberofPotablewaterreservoirs+starting_value_i)[starting_value_i:]:
            exec 'self.Potablewaterreservoirattributelist.append({"Potablewaterreservoir_'+str(i)+'" : Potablewaterreservoirattributes})'
            exec '''self.line1='\\t\\t\\t<node id="Potable_Water_Reservoir_'+str(i)+'" class="Potable_Water_Reservoir"/> \\n' '''
         
            alllines = ''
            for n in range(1):
                exec 'alllines += self.line'+str(n+1)
                
            self.Potablewaterreservoirnodelist.append(alllines)

        print str(numberofPotablewaterreservoirs)+' Potablewaterreservoirs have been created!'
        return
    
    



    
    
    
    
    
    