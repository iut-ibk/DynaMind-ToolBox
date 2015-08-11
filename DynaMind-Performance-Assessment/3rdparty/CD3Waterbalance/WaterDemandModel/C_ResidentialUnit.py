__author__ = 'JBreman'
__project__ = "WaterDemandModel"

import C_UnitType as Unit
import C_ResidentialActor as RA
import math as math


class ResidentialUnit(Unit.UnitType):
    """
    The residential class has two purposes:
    1) it generates water use events for appliances used on a residential unit bases: washing machine, taps and dishwasher.
        Where the event objects contain data on the time of the event and the volume of water generated and where they took place (i.e. res unit).
    2) It generates a res. unit size (i.e. people (actors) in the unit) and for each actor, gets all of their water use events from the res actor class.
       All of these events (plus those from res unit appliances) are stored within the array self.__ResidentialEvents.

    TAPS: water events generated through tap use are considered at the residential unit level. This is because the volume of water produced by a res. unit through tap use is dependant
    on the unit size, where total tap water volume decreases with increasing unit size. This is due to the use of the kitchen tap which is generally used for communal purposes e.g.
    cooking/hand washing dishes.
    """

    # Residential unit objects manipulated by class
    __res_actors = []                      # used to store the actor objects in the unit
    __residentialEvents = []                  # used to store all water use events from the residential unit and actor

    def __init__(self,numberOfActors):

        """
        :param numberOfActors: number of actors in the res unit. Determined in the MainInterface window
        """

        # household objects
        self.__residentialEvents = []
        self.__res_actors = []

        "Get actors and their end use info"

        for a in range(int(numberOfActors)):           # For each actor in the house:

            self.__res_actors.append(RA.ResidentialActor())               # Add each actor and their events (i.e. water usage events)

        Unit.UnitType.__init__(self,"RESIDENTIAL")      # instantiate the parent class and give it the unit type

    def calcWashMachineFreq(self,wash_freq):
        """
        This method uses the relationship between residential unit size and the number of washes per week to calculate how many times a residential unit would do washing
        per day. The equation is inbuilt into this function.

        :param wash_freq: is the wash freq. returned based on an equation of relevance to a residential unit. The method in the parent class (UnitType), uses this param
        when the method is called.
        :return: This method returns the frequency a residential unit uses a washing machine per day. Attached to the object self.__wash_machine_freq
        NOTE: This frequency stays constant each day. It does not change from day to day - to represent that a household will implement a weekly pattern.
        """

        self.__wash_machine_freq = (2.62*math.log(len(self.__res_actors))+ 1.87) / 7    # frequency of using the washing machine per day (float). Equation is from the Yarra Valley Water residential water use study 2011 (winter)
                                                                                        # divided by 7 to get frequency/day as equation is for weekly occurance

        super(ResidentialUnit,self).calcWashMachineFreq(self.__wash_machine_freq)        # calls the method in the parent class, but with the updated self.wash_machine_freq variable


    def calcDishWasherFreq(self,dish_wash_freq):
        """
        This method uses the relationship between residential unit size and number of washes per week to calculate how many times a residential unit would use the dishwasher
        per day. The equation is inbuilt into this function.

        :param dish_wash_freq: is the variable returned based on an equation of relevance to a residential unit. The method in the parent class (UnitType), uses this param
        when the method is called.
        :return: This method returns the frequency a residential unit uses a dishwasher per day. Attached to the object self.__dish_washer_freq
        NOTE: This frequency stays constant each day. It does not change from day to day - to represent that a residential unit will implement a weekly pattern.
        """

        actor = len(self.__res_actors)                # number of actors in the residential unit

        self.__dish_washer_freq = (2.70*math.log(actor)+ 0.64) / 7    # frequency of using the dish washer per day (float). Equation is from the Yarra Valley Water residential water use study 2011 (winter)
                                                                       # divided by 7 to get frequency/day as equation is for weekly occurance

        super(ResidentialUnit,self).calcDishWasherFreq(self.__dish_washer_freq)         # calls the method in the parent class, but with the updated self.dish_washer_freq variable

    def calcTapUseVolume(self,volume):
        """
        This method returns the volume of water a residential unit would use in a day through tap use. It is used by the method newDay in the C_UnitType class to determine
        how many tap use events a residential unit should have. When the sum of each tap use event is > than the volume calculated here, the newDay method
        stops making tap use events for the residential unit.

        This method was used as there was no data available on the frequency of tap use for a actor. It is based on an equation that shows the relationship
        between household size and per capita tap use.

        The equation used needs to know the number of people (actors) in the residential unit.

        :param volume:  is the volume of water a residential unit uses in a day
        """

        self.__tap_volume = (-8.61*math.log(len(self.__res_actors))+31.83)*len(self.__res_actors)   # equation of relevance to a residential unit

        super(ResidentialUnit,self).calcTapUseVolume(self.__tap_volume)                             # calls the method in the parent class, but with the updated self.__volume variable


    def getTapEvents(self):

        return super(ResidentialUnit,self).getTapEvents()       # get all tap events of the res unit

    def getWashEvents(self):

        return super(ResidentialUnit,self).getWashEvents()      # get all wash events of the res unit

    def getDishWashEvents(self):

        return super(ResidentialUnit,self).getDishWashEvents()  # get all dishwash events of the res unit

    def getAllUnitEvents(self):
        """
        Gets all the events for the residential unit including those produced by the actors and the unit itself
        :return: an array containing all the event objects. This is possible since each event object stores infomration on: what appliance it came from,
        what unit it was generated in, its volume and the time it was generate (hour)
        """

        events = []                     # empty array to store all the events.

        # Residential unit events:
        events.append(self.getTapEvents())
        events.append(self.getWashEvents())
        events.append(self.getDishWashEvents())

        for a in self.__res_actors:             # for each residential actor in the residential unit:
            for e in a.getAllActorEvents():           # for each event (from all appliances) of the residential actor
                events.append(e)                           # Append the event

        "unravel events array:"         # currently will contain lists and numbers e.g. [[1,2,3],4,5,[6]]
                                        # not compatible with other code. Therefore need to be made into: [1,2,3,4,5,6]:
        for i in events:
            if type(i) == (list):                   # if i in event [] is a list e.g. [1,2,3],[6]:
                for j in i:
                    self.__residentialEvents.append(j)        # append each item in list
            else:                                   # if i in event [] is a value e.g. 4,5
                self.__residentialEvents.append(i)            # append each item

        return self.__residentialEvents






