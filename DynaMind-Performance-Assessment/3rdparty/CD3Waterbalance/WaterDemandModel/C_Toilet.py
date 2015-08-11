__author__ = 'JBreman'
__project__ = 'WaterDemandModel'

import sampling_db as DB
import C_ApplianceType as Appliance
import calcAndSampleCumData as CD
import sampleDataBin as SB

class Toilet(Appliance.ApplianceType):         # inherits parent class Appliance()
    """

    """

    # Objects used/manipulated by class
    __flush_volume = 0
    __flush_time = 0
    __unit_type = ""


    def __init__(self,flush_time,unit_type):
        Appliance.ApplianceType.__init__(self, "toilet")

        self.__flush_volume = 0

         # Attach the start time to a object:
        self.__flush_time = flush_time
        self.__unit_type = unit_type

        #Call class methods:
        self.calcFlushVolume()
        self.getVolume()
        self.getStartTime()
        self.getUnitType()

    def calcFlushVolume(self):

        self.__flush_volume = CD.getSampledCumValue(DB.SamplingDatabase.Instance().getDB("TOILET_FLUSH_VOLUME_DATABASE"))

        if type(self.__flush_volume) == str:

            self.__flush_volume = SB.sampleDataBin(self.__flush_volume)

        return self.__flush_volume

    def getVolume(self):
        return self.__flush_volume

    def getStartTime(self):

        return self.__flush_time

    def getUnitType(self):

        return self.__unit_type

if __name__ == "__main__":

    output = Toilet(7)

    print output.getVolume()

