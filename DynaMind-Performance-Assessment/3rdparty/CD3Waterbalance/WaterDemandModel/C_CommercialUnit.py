__author__ = 'Jbreman'
__project__ = 'WaterDemandModel'


import math as math
import C_CommercialActor as CA
import C_UnitType as Unit

class CommercialUnit(Unit.UnitType):
    """
    The commercial unit class has two purposes:
    1) it generates water use events for appliances used on a commercial unit bases:  taps and dishwasher.
        Where the event objects contain data on the time of the event and the volume of water generated and where they took place (i.e. comm unit).
    2) It generates a com. unit size (i.e. people (actors) in the unit) and for each actor, gets all of their water use events from the comm actor class.
       All of these events (plus those from comm unit appliances) are stored within the array self.__CommercialEvents.
    """

    # Commercial unit objects manipulated by class
    __comm_actors = []      # used to store the actor objects within the comm unit
    __commercialEvents = [] # used to store all the water use events from the comm actor and unit

    def __init__(self,numberOfActors):
        """

        :param numberOfActors: number of actors within the comm unit that generate water events
        """

        # building objects:
        self.__comm_actors = []
        self.__commercialEvents = []

        "Get actors and their end use info"
        for a in range(int(numberOfActors)):
            self.__comm_actors.append(CA.CommercialActor())

        Unit.UnitType.__init__(self,"COMMERCIAL")               # instantiate the parent class and give it the unit type


    def calcDishWasherFreq(self,dish_wash_freq):
        """
        This method uses the relationship between commercial unit size and number of washes per week to calculate how many times a commercial unit would use the dishwasher
        per day. The equation is inbuilt into this function.

        :param dish_wash_freq: is the variable returned based on an equation of relevance to a commercial unit. The method in the parent class (UnitType), uses this param
        when the method is called.
        :return: This method returns the frequency a commercial unit uses a dishwasher  per day. Attached to the object self.__dish_washer_freq
        NOTE: This frequency stays constant each day. It does not change from day to day
        """

        actor = len(self.__comm_actors)                # number of actors in the residential unit

        self.__dish_washer_freq = (2.70*math.log(actor)+ 0.64) / 7    # frequency of using the dish washer per day (float). Equation is from the Yarra Valley Water residential water use study 2011 (winter)
                                                                       # divided by 7 to get frequency/day as equation is for weekly occurance

        super(CommercialUnit,self).calcDishWasherFreq(self.__dish_washer_freq)

    def calcTapUseVolume(self,volume):
        """
         This method returns the volume of water a commercial unit would use in a day through tap use. It is used by the method newDay in the C_UnitType class to determine
        how many tap use events a commercial unit should have. When the sum of each tap use event is > than the volume calculated here, the newDay method
        stops making tap use events for the residential unit.

        This method was used as there was no data available on the frequency of tap use for a actor. It is based on an equation that shows the relationship
        between unit size and per capita tap use.

        The equation used needs to know the number of people (actors) in the unit.

        :param volume:  is the volume of water a residential unit uses in a day
        """

        self.__tap_volume = (-8.61*math.log(len(self.__comm_actors))+31.83)*len(self.__comm_actors)     # dummy equation

        super(CommercialUnit,self).calcTapUseVolume(self.__tap_volume)                                  # calls the method in the parent class, but with the updated self.__volume variable

    def getDishWashEvents(self):

        return super(CommercialUnit,self).getDishWashEvents()           # get all the dish wash events

    def getTapEvents(self):

        return super(CommercialUnit,self).getTapEvents()                # get all the tap events

    def getAllUnitEvents(self):
        """
        Gets all the events for the commercial unit including those produced by the actors and the unit itself
        :return: an array containing all the event objects. This is possible since each event objects stores information on : what appliance it came from,
        what unit it was generated in, its volume and the time it was generate (hour)

        """

        events = []                              # empty array to store all the events.

        # commercial unit events:
        events.append(self.getTapEvents())
        events.append(self.getDishWashEvents())

        for a in self.__comm_actors:             # for each commercial actor in the unit:

            for e in a.getAllActorEvents():           # for each event (from all appliances) of the comm actor
                events.append(e)                           # Append the event

        "unravel events array:"         # currently will contain lists and numbers e.g. [[1,2,3],4,5,[6]]
                                        # not compatible with other code. Therefore need to be made into: [1,2,3,4,5,6]:
        for i in events:
            if type(i) == (list):                   # if i in event [] is a list e.g. [1,2,3],[6]:
                for j in i:
                    self.__commercialEvents.append(j)        # append each item in list
            else:                                   # if i in event [] is a value e.g. 4,5
                self.__commercialEvents.append(i)            # append each item

        return self.__commercialEvents



