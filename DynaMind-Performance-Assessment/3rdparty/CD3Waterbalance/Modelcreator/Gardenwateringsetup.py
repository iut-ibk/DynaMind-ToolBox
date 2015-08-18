# -*- coding: utf-8 -*-
"""
Created on Mon Dec 01 13:21:57 2014

@author: Acer
"""

class Gardenwateringsetup:
    
    def __init__(self, numberofGardenmodules, starting_value_i, **Gardenmoduleattributes ):
        self.numberofGardenmodules = numberofGardenmodules
        self.Gardenmoduleattributelist = []
        self.Gardenmodulenodelist = []
        for i in range(numberofGardenmodules+starting_value_i)[starting_value_i:]:
            exec 'self.Gardenmoduleattributelist.append({"GardenWateringModel_'+str(i)+'" : dict.copy(Gardenmoduleattributes)})'

        print str(numberofGardenmodules)+' Gardenmodules have been created!'
        return
   
    def Setandwrite_attributes(self,numberofGardenmodules, starting_value_i, attributevector):
        for i in range(numberofGardenmodules+starting_value_i)[starting_value_i:]:
            self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Average_Watering_Frequency"] = attributevector[i-starting_value_i][0]
            self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Deviation_of_Frequency"] = attributevector[i-starting_value_i][1]
            self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Maximal_Watering_Flow_Rate"] = attributevector[i-starting_value_i][2]
            self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Smart_Watering_Start_Time_End_Time"] = attributevector[i-starting_value_i][3]
            self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Watering_Method_Normal_Watering_or_Smart_Watering"] = attributevector[i-starting_value_i][4]
    
        for i in range(numberofGardenmodules+starting_value_i)[starting_value_i:]:
            exec '''self.line1='\\t\\t\\t<node id="GardenWateringModel_'+str(i)+'" class="GardenWateringModel"> \\n' '''
            exec '''self.line2='\\t\\t\\t\\t<parameter name="Average_Watering_Frequency_[d]" type="double" value="'+str(self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Average_Watering_Frequency"])+'"/> \\n ' '''
            exec '''self.line3='\\t\\t\\t\\t<parameter name="Deviation_of_Frequency_[d]" type="double" value="'+str(self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Deviation_of_Frequency"])+'"/> \\n ' '''
            exec '''self.line4='\\t\\t\\t\\t<parameter name="Maximal_Watering_Flow_Rate_[l/min]" type="double" value="'+str(self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Maximal_Watering_Flow_Rate"])+'"/> \\n ' '''
            exec '''self.line5='\\t\\t\\t\\t<parameter name="Smart_Watering_Start_Time_End_Time_[hh,hh]" type="string" value="'+str(self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Smart_Watering_Start_Time_End_Time"])+'"/> \\n ' '''
            exec '''self.line6='\\t\\t\\t\\t<parameter name="Watering_Method_(Normal_Watering_or_Smart_Watering)" type="string" value="'+str(self.Gardenmoduleattributelist[i-starting_value_i][str('GardenWateringModel_'+str(i))]["Watering_Method_Normal_Watering_or_Smart_Watering"])+'"/> \\n ' '''
            exec '''self.line7='\\t\\t\\t</node> \\n ' '''        
            alllines = ''
            for n in range(7):
                exec 'alllines += self.line'+str(n+1)
                
            self.Gardenmodulenodelist.append(alllines)

        return






