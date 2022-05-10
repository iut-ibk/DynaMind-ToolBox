__author__ = 'JBreman'
__project__ = 'WaterDemandModel'

residential_appliance_types = ["bath","toilet","shower","tap", "washing_machine","dish_washer"]

commercial_appliance_types = ["toilet","shower","tap", "dish_washer"]

class ApplianceType():
    """
    Parent class of all the appilances that create water volume events.

    The class is passed to each event class, who's own getStartTime and getVolume methods are over ride these ones
    """
    __appliance_type = ''

    def __init__(self, appliance):
        """
        :param appliance: appliance that generates a water event e.g. toilet, shower, taps etc.
        """
        self.__appliance_type = appliance

    def getStartTime(self):
        pass

    def getUnitType(self):
        pass

    def getActorType(self):
        pass

    def getAppliance(self):

        return self.__appliance_type

    def getVolume(self):

        pass


