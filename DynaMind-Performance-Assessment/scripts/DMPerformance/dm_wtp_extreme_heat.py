__author__ = 'christianurich'


from pydynamind import *
from scipy import random


# agent_wtp(Region_ID,agent,'wtp_stream_high1') = 10.99824 + 0.3368328*agent_wtp(Region_ID,agent,'Bedroom') - 0.0334691*agent_wtp(Region_ID,agent,'Age') - 0.0765181*agent_wtp(Region_ID,agent,'Education');

class WTP_Extream_Heat_AU(Module):
    display_name = "WTP Extreme Heat (AU)"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.households = ViewContainer("household", DM.COMPONENT, DM.READ)
        self.households.addAttribute("age", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("bedrooms", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("education", DM.Attribute.STRING, DM.READ)
        self.households.addAttribute("wtp_extreme_heat", DM.Attribute.DOUBLE, DM.WRITE)

        self.registerViewContainers([self.households])

        # self.education_levels = {}
        # self.education_levels[""] = 1.
        # self.education_levels["technical"] = 1.
        # self.education_levels["other"] = 2.
        # self.education_levels["secondary"] = 3.
        # self.education_levels["tertiary"] = 4.

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
            wtp = max(0.0, random.normal(7.348656, 9.407043))
            h.SetField("wtp_extreme_heat", wtp)
            if counter % 100000 == 0:
                log(str(counter), DM.Standard)
                self.households.sync()
                self.households.set_next_by_index(counter)
        self.households.finalise()
