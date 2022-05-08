__author__ = 'JBreman'
__project__ = "WaterDemandModel"

import C_ApplianceType as Event
import sampling_db as DB
import calcAndSampleCumData as CD
import sampleDataBin as SB


class DishWasher(Event.ApplianceType):

     # Objects used/manipulated by class
    __start_time = 0
    __wash_volume = 0

    def __init__(self,start_time,unit_type):
        Event.ApplianceType.__init__(self,"dish_washer")

        self.__wash_volume = 0

        # Attach the start time to a object:
        self.__start_time = start_time
        self.__unit_type = unit_type

        #Call class methods:
        self.calcWashVolume()
        self.getStartTime()
        self.getVolume()

    def calcWashVolume(self):

        self.__wash_volume = CD.getSampledCumValue(DB.SamplingDatabase.Instance().getDB("DISH_WASHER_VOLUME_DATABASE"))     # sample wash volume from a distribution.

        if type(self.__wash_volume) == str:                                         # if the bin selected represents a range (e.g 30-40L):

            self.__wash_volume = SB.sampleDataBin(self.__wash_volume)                     # then randomly sample between this range.

    def getStartTime(self):

         return self.__start_time

    def getVolume(self):

         return self.__wash_volume

    def getUnitType(self):

        return self.__unit_type


# if __name__ == "__main__":

    # output = DishWasher(3)

