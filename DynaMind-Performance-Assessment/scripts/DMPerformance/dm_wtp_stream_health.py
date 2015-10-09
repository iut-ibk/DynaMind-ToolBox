__author__ = 'christianurich'


from pydynamind import *

# max(0, normal(7.348656, 9.407043));

class WTP_Stream_Health_AU(Module):
    display_name = "WTP Stream Health (AU)"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        # self.createParameter("swmm_rain_file", DM.FILENAME)
        # self.swmm_rain_file = ""
        #
        # self.createParameter("time_delta", DM.INT)
        # self.time_delta = 360

        self.households = ViewContainer("household", DM.COMPONENT, DM.READ)
        self.households.addAttribute("age", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("bedrooms", DM.Attribute.INT, DM.READ)
        self.households.addAttribute("education", DM.Attribute.STRING, DM.READ)
        self.households.addAttribute("wtp_stream_health", DM.Attribute.DOUBLE, DM.WRITE)

        self.registerViewContainers([self.households])

        self.education_levels = {}
        # self.education_levels[""] = 1.
        # self.education_levels["technical"] = 1.
        # self.education_levels["other"] = 2.
        # self.education_levels["secondary"] = 3.
        # self.education_levels["tertiary"] = 4.

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
            wtp = 10.99824 + 0.3368328 * bedrooms - 0.0334691 * age - 0.0765181 * self.education_levels[education]
            h.SetField("wtp_stream_health", wtp)
        self.households.finalise()
