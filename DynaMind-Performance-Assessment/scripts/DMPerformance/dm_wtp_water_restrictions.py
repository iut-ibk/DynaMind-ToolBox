__author__ = 'christianurich'


from pydynamind import *
from scipy import random


# 2.844952 - 11.62263*agent_wtp(Region_ID,agent,'Education') + 6.463264*agent_wtp(Region_ID,agent,'Gender')

class WTP_Water_Restrictions_AU(Module):
    display_name = "WTP Water Restrictions (AU)"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.households = ViewContainer("household", DM.COMPONENT, DM.READ)
        self.households.addAttribute("age", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("bedrooms", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("education", DM.Attribute.STRING, DM.READ)
        self.households.addAttribute("wtp_water_restrictions", DM.Attribute.DOUBLE, DM.WRITE)

        self.registerViewContainers([self.households])

        self.education_levels = {}
        self.education_levels[""] = 0.
        self.education_levels["technical"] = 0.
        self.education_levels["other"] = 0.
        self.education_levels["secondary"] = 1.
        self.education_levels["tertiary"] = 1.

    def run(self):
        self.households.reset_reading()
        for h in self.households:
            age = h.GetFieldAsInteger("age")
            education = h.GetFieldAsString("education")
            bedrooms = h.GetFieldAsInteger("bedrooms")
            wtp = max(0.0, (3.208593 - 0.6362358 * bedrooms + 0.356186 * age - 0.0027977 * age * age - 1.883466 * self.education_levels[education]))
            h.SetField("wtp_water_restrictions", wtp)
        self.households.finalise()
