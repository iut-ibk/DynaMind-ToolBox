__author__ = 'JBreman'
__project__ = "WaterDemandModel"

import C_Actor as Actor

class CommercialActor(Actor.Actor):
    """
    This class is  a child class of the C_Actor Parent. It creates events only for those appliances that are of relevance in a
    commercial setting, whereby the commercial unit Class can then call all of these events for each commercial actor within
    the unit.

    Appliances that are of relevance to a commercial actor are: shower and toilet
    """

    def __init__(self):

        Actor.Actor.__init__(self,"COMMERCIAL")     # initialise the parent class


    def getToiletEvents(self):
        """
        :return: all toilet events of the commercial actor
        """

        return super(CommercialActor, self).getToiletEvents()

    def getShowerEvents(self):
        """
        :return: return all shower events of the commercial actor
        """

        return super(CommercialActor,self).getShowerEvents()

    def getAllActorEvents(self):
        """
        :return: all events of the commercial actor --> commercial unit class calls this method to get all events.
        """

        return self.getToiletEvents(), self.getShowerEvents()





