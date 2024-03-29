__author__ = 'christianurich'


from pydynamind import *
from scipy import random


# 2.844952 - 11.62263*agent_wtp(Region_ID,agent,'Education') + 6.463264*agent_wtp(Region_ID,agent,'Gender')

class WTP_Recreational_Value_AU(Module):
    display_name = "WTP Recreational Value (AU)"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.households = ViewContainer("household", DM.COMPONENT, DM.READ)
        self.households.addAttribute("age", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("bedrooms", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("education", DM.Attribute.STRING, DM.READ)
        self.households.addAttribute("wtp_recreational_value", DM.Attribute.DOUBLE, DM.WRITE)

        self.registerViewContainers([self.households])

        self.education_levels = {}
        self.education_levels[""] = 0.
        self.education_levels["technical"] = 0.
        self.education_levels["other"] = 0.
        self.education_levels["secondary"] = 1.
        self.education_levels["tertiary"] = 1.

    def run(self):
        self.households.reset_reading()
        counter = 0
        for h in self.households:
            counter+=1
            education = h.GetFieldAsString("education")
            wtp = max(0.0, (2.844952 - 11.62263*self.education_levels[education] + 6.))
            h.SetField("wtp_recreational_value", wtp)
            if counter % 100000 == 0:
                self.households.sync()
                self.__container.set_next_by_index(counter)
        self.households.finalise()
