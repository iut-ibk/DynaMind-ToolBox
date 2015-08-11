__author__ = 'JBreman'
__project__ = 'WaterDemandModel'

import C_ApplianceType as Appliance

class Bath(Appliance.ApplianceType):

    __start_time = 0      # the time a actor has an event. Passed from the class actor
    __bath_volume = 0       # the volume of water used for one bath event (L)
    __unit_type = ""

    def __init__(self,start_time,unit_type):
        Appliance.ApplianceType.__init__(self,"bath")

        self.__bath_volume = 0

        # Attach the start time to a object
        self.__start_time = start_time
        self.__unit_type = unit_type

        # Call class methods:
        self.getVolume()
        self.getStartTime()
        self.getUnitType()

    def getVolume(self):

        self.__bath_volume = 120.00             # limited data available. This value taken from M. Thyer (2009)

        return self.__bath_volume

    def getStartTime(self):

        return self.__start_time

    def getUnitType(self):

        return self.__unit_type

