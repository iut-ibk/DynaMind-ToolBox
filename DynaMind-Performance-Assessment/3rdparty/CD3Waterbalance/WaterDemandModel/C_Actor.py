__author__ = 'JBreman'
__project__ = 'WaterDemandModel'


import numpy as np
import math as math

import sampling_db as DB
import sampleDiurnalPattern as DP
import calcAndSampleCumData as CD
import sampleLogNormDistribution as LN
import sampleDataBin as SB

import C_ShowerEvent as SE
import C_Toilet as TE
import C_Bath as BT

actor_types = ["RESIDENTIAL", "COMMERCIAL"]

class Actor(object):
    """
    This class is the parent class for both the residenital and commercial actors.

    This class creates objects for all water use events conducted by actors within a unit. It only contains those events that are done on an individual basis e.g. showering, toilet flushing, bathing.
    Water use events such as the dishwasher, taps and washing machine are done at a household level and therefore are not included in this class
    """

    # Shower objects manipulated by class:
    __shower_frequency = 0            # frequency that someone has a shower per day (i.e. probability of a shower during a day"
    __showers = []                    # Numbers of showers a actor has in a day.
    __showerEvents = []               # Actual shower objects at a certain time. The object represent the time a actor has a shower, the duration of the shower and the flowrate of the shower head
    __shower_duration = 0             # Time that one actor showers for

    # Toilet objects manipulated by class:
    __toilet_flush_freq = 0                 # represents the number of times someone uses a toilet
    __toilet_flushes = []                   # used to store number of flushes for a actor in a day
    __toiletEvents = []                      # Actual toilet flush object. Each object in this array represents the volume and time of the flush event

    # Bath objects manipulated by class:
    __bath_frequency = 0                    # number of times a actor has a bath per day
    __bath_uses = []                        # stores number of bath events in a day
    __bathEvents = []                       # stores the bath event objects: volume and time of bath

    # Actor objects:
    __actor_type = ''                       # i.e. residential or commercial

    # def __init__(self,unit_type,actor):
    def __init__(self, actor):

        self.__actor_type = actor                   # is the actor type generating the water events. e.g. a residential actor, commercial actor. The type of actor will influence which appliances
                                                    # are being used. e.g. a commercial actor wont generate bath events. The actor also needs to know this so that it can select the correct input file for
                                                    # e.g. the diurnal pattern, which will change depending on the actors location.

        # initialise objects
        self.__showers = []
        self.__showerEvents = []
        self.__shower_frequency = 0
        self.__shower_duration = 0
        self.__toilet_flush_freq = 0
        self.__toilet_flushes = []
        self.__toiletEvents = []
        self.__bath_frequency = 0
        self.__bath_uses = []
        self.__bathEvents = []

        #Initialise methods:
        self.calcShowerFrequency()
        self.haveShower()
        self.calcShowerDuration()
        self.getShowerDuration()
        self.calcToiletFlushFreq()
        self.calcToiletFlushes()
        self.calcBathUseFrequency()
        self.calcBathUses()
        self.newDay()

    def calcShowerFrequency(self):
        """
        This method generates a number which represents how often a actor has a shower during the day. i.e. if frequency = 1, they have a shower everyday. If frequency = 1.2 (>1), they have a shower every day + there is
        a possibility that they will have a 2nd shower. If frequency = 0.6 (<1) they dont have a shower everyday, maybe every 2nd day

        Inputs: requires input data that represents the probability for different shower frequencies. Where the sum of the probabilities = 1

        NOTE: Shower Frequency remains constant for a actor. It is not re-sampled every day. This is to replicate behaviour patterns that people would have in terms of their shower frequency.
        """

        self.__shower_frequency = CD.getSampledCumValue(DB.SamplingDatabase.Instance().getDBTest(self.__actor_type, "SHOWER_FREQUENCY_DATABASE"))     # Get the shower frequency for one actor

        if type(self.__shower_frequency) == str:                                               # if the frequency is not a single number, but a range i.e 0.2-0.4 then sample from this bin:

            self.__shower_frequency = SB.sampleDataBin(self.__shower_frequency)                               # call helper function to sample from bin

    def haveShower(self):
        """
        This method determines whether a actor has a shower during one day. It is determined using the output from def calcShowerFrequency()

        From this method, a actor can either have no shower, one shower or two shower in a day. Where this output is determined by the value of self.__frequency.

            - If frequency < 1. A actor may or may not have a shower that day.
            - If frequency >= 1. A actor will have 1 or more shower per day.

        For F<1, whether or not a actor has a shower is determined by comparing a random number (b/n 0-1) to the frequency. If the random number is less than the
        frequency, then the actor has a shower. If the random number is greater than the frequency, the actor doesn't have a shower. This means that the smaller the shower frequency number is
        the smaller the chance is that someone will have a shower during the day.

        For F>=, the method will add a shower for the whole integer (e.g. 1.8 = 1 guaranteed shower, 2.1 = 2 guaranteed showers). If will then apply the above proces for the remain decimal.

        NOTE: Whether a actor has a shower (yes/no) IS re-sampled EVERYDAY. But the frequency the sample is based on DOES NOT CHANGE from day to day.

        Output: the method outputs the array self.__showers. For each shower that a actor has, a 1 is appended to the array (representing 'yes')
        """
        yes = 1                             # Used to append to shower[] if a actor does have a shower during a day
        rand = np.random.sample()           # Randomly sample a number between 0 and 1

        # if actor does not always have a shower each day:
        if self.__shower_frequency < 1:

            if rand < self.__shower_frequency:             # if the random number returned is less than the shower frequency:
                self.__showers.append(yes)                 # The actor has a shower. Append a '1' to the array shower

        # if actor has 1 or more showers per day:
        elif self.__shower_frequency >= 1:
            showers = int(math.floor(self.__shower_frequency))      # showers = guaranteed showers. i.e. if freq = 1.6 or 1.1 or 1.02. actor is guaranteed to have 1 shower. Math.floor of x returns the largest integer that is not greater than x.

            for i in range(showers):                                # for each guaranteed shower
                self.__showers.append(yes)                                # add a 1 to the array

            if rand < self.__shower_frequency - showers:            # if rand < remaining shower frequency (e.g. 2.5 - 2 = 0.5), then:
                self.__showers.append(yes)                                # add another 1 to the array.

    def calcShowerDuration(self):
        """
        calculates the length of time a actor has one shower event i.e. 10 minutes.
        Inputs: requires a log normal distribution of shower duration data, giving duration times (t) on the x axis and probabilities on the y axis. Where sum of probabilities = 1
        """

        self.__shower_duration = LN.randSampleLogNormDist(DB.SamplingDatabase.Instance().getDBTest(self.__actor_type, "SHOWER_DURATION_DATABASE"))     # calculate the length of a shower event for a actor.

    def getShowerDuration(self):
        """
        :return: the length of shower event for a actor
        """
        return self.__shower_duration

    def calcToiletFlushFreq(self):
        """
        Method used to calculate how many times a actor flushes the toilet in a day (frequency). It does this by sampling a relative frequency distribution (input),
        which returns a float value e.g. 3.4. This would mean that a actor goes to the toilet 3 times a day, and have a probability of 0.4 of going a 4th time.

        The output (flush frequency), is used by the def calcToiletFlushes which makes an array of 1's for each time a toilet flushes a toilet.
        """

        "Calculate the number of flushes per day:"

        self.__toilet_flush_freq = CD.getSampledCumValue(DB.SamplingDatabase.Instance().getDBTest(self.__actor_type,"TOILET_FLUSH_FREQ_DATABASE"))

        if type(self.__toilet_flush_freq) == str:                               # if the frequency bin represents a range e.g 2-3 flushes, then sample from this:

            self.__toilet_flush_freq = SB.sampleDataBin(self.__toilet_flush_freq)      # call helper function to sample from bin.

    def calcToiletFlushes(self):
        """
        This method makes an array which represents the number of flushes a actor does in a day. It uses the self.toilet_flush_freq as an input.
        It works by:
            For example if flush_freq = 4.7.
            The method creates an array (toilet_flushes), with four 1's ([1,1,1,1]. It then sees if the actor would have another flush
            based on the probability of 0.7 (4.7-4).

        Output: an array of 1's, representing each flush of a actor.
        """
        yes = 1
        rand = np.random.sample()                               # generate a random number between 0 and 1

        flushes = int(math.floor(self.__toilet_flush_freq))     # get all the guaranteed flushes. i.e. if flush frequency = 3.7. A actor is guaranteed to flush 3 times in a day.

        # for all the guaranteed flushes:
        for i in range(flushes):                                # for each flush:
            self.__toilet_flushes.append(yes)                       # add a 1 to the array self.__toilet_flushes

        # chance that someone has an extra flush:
        if rand < self.__toilet_flush_freq - flushes:           # if rand < remaining flush frequency (e.g. 3.7 - 3 flushes = 0.7), then:
            self.__toilet_flushes.append(yes)                       # add another 1 to the array


    def calcBathUseFrequency(self):
        """
        input: requires a input distribution of relative frequencies (sum = 1) for different bath use frequencies (i.e. the number of baths a actor has a day)
        return: a float number representing the number of baths a actor has per day.

        """
        self.__bath_frequency = CD.getSampledCumValue(DB.SamplingDatabase.Instance().getDBTest(self.__actor_type, "BATH_FREQUENCY_DATABASE"))     # Get the bath frequency for one actor

        if type(self.__bath_frequency) == str:                                               # if the frequency is not a single number, but a range i.e 0.2-0.4 then sample from this bin:

            self.__bath_frequency = SB.sampleDataBin(self.__bath_frequency)                               # call helper function to sample from bin


    def calcBathUses(self):
        yes = 1
        rand = np.random.sample()                               # generate a random number between 0 and 1

        # if a actor does not have a bath each day:
        if self.__bath_frequency < 1:

            if rand < self.__bath_frequency:             # if the random number returned is less than the bath frequency:
                self.__bath_uses.append(yes)                 # The actor has a bath. Append a '1' to the array shower

        # if a actor has one or more baths per day:
        if self.__bath_frequency >= 1:
            baths = int(math.floor(self.__bath_frequency))     # get all the guaranteed baths. i.e. if bath frequency = 3.7. A actor is guaranteed to have a bath 3 times in a day.

            # for all the guaranteed flushes:
            for i in range(baths):                                # for each bath:
                self.__bath_uses.append(yes)                       # add a 1 to the array self.__bath_uses

            # chance that someone has an extra flush:
            if rand < self.__bath_frequency - baths:           # if rand < remaining bath frequency (e.g. 3.7 - 3 baths = 0.7), then:
                self.__bath_uses.append(yes)                       # add another 1 to the array


    def newDay(self):
        """
        For each water use event that a actor has in a day, this method creates the  object of that Event for the actor.
        Where the Event represents:
            - The time of the day that the event begins

            - The flowrate/volume of the event

        This data is stored in a different array for each appliance . The events can then be called by other methods and their data extracted.
        """

        "Shower Events:"
        for i in range(len(self.__showers)):                                 # For each shower that a actor has:
            shower_time = DP.sampleCumDF(DB.SamplingDatabase.Instance().getDBTest(self.__actor_type,"DIURNAL_PATTERN"))          # Get the time that a actor has a shower

            shower_event = SE.ShowerEvent(shower_time,self.__shower_duration,self.__actor_type)                    # Makes the physical object of the event. Where the event contains: Time, Duration and Flowrate. The actor gives time and duration to the ShowerEvent.
                                                                               # The class ShowerEvent makes the shower flowrate. All 3 of these objects are then attached to the physical event (made real)
            self.__showerEvents.append(shower_event)                               # Appends each event to showerEvents

        "Toilet Events:"
        for i in range(len(self.__toilet_flushes)):                         # For each flush:

            flush_time = DP.sampleCumDF(DB.SamplingDatabase.Instance().getDBTest(self.__actor_type,"DIURNAL_PATTERN"))         # Get the time that a actor flushes the toilet

            flush_event = TE.Toilet(flush_time,self.__actor_type)                             # Make an event object for each toilet flush. This event contains info on the time of the flush and the flush volume

            self.__toiletEvents.append(flush_event)                               # add each event to an array

        "Bath Events:"

        for i in range(len(self.__bath_uses)):                # For each bath:
            bath_use_time = DP.sampleCumDF(DB.SamplingDatabase.Instance().getDBTest(self.__actor_type,"DIURNAL_PATTERN"))    # Get the time the actor has the bath

            bath_event = BT.Bath(bath_use_time,self.__actor_type)                     # Make an event object for each bath -- > contain the info on the time and volume

            self.__bathEvents.append(bath_event)                    # append each event to the array. Used to store each object.


    def getAllActorEvents(self):

        pass

    def getShowerEvents(self):
        """
        :return: the showerEvents of the actor for a day
        """

        return self.__showerEvents

    def getToiletEvents(self):
        """
        :return: the toilet flush Events of the actor for a day
        """

        return self.__toiletEvents

    def getBathEvents(self):
        """

        :return: the bath Events of the actor for a day
        """

        return self.__bathEvents


"Testing"

if __name__ == "__main__":

    output = Actor(5)

    # print output.calcBathUseFrequency()

    # output.newDay()
    output.getBathEvents()


    print 'h'


