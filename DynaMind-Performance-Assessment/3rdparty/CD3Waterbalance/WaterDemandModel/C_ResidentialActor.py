__author__ = 'jbreman'
__project__ = 'WaterDemandModel'

import C_Actor as Actor

class ResidentialActor(Actor.Actor):
    """
    This class is  a child class of the C_Actor Parent. It creates events only for those appliances that are of relevance in a
    residential setting, whereby the residential unit Class can then call all of these events for each residential actor within
    the unit.

    Appliances that are of relevance to a residential actor are: shower, toilet and bath
    """

    def __init__(self):

        Actor.Actor.__init__(self,"RESIDENTIAL")            # initialise the parent class


    def getToiletEvents(self):
        """
        :return: all toilet events of the residential actor
        """

        return super(ResidentialActor, self).getToiletEvents()

    def getShowerEvents(self):
        """
        :return: all shower events of the res. actor
        """

        return super(ResidentialActor,self).getShowerEvents()

    def getBathEvents(self):
        """
        :return: all bath events of the res actor
        """

        return super(ResidentialActor,self).getBathEvents()

    def getAllActorEvents(self):
        """
        :return: all events of the residential actor --> residential unit class calls this method to get all events.
        """
        return self.getBathEvents(), self.getToiletEvents(), self.getShowerEvents()
