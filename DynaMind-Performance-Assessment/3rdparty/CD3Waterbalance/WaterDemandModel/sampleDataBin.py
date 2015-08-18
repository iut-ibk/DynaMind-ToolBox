__author__ = 'JBreman'
__project__ = 'Wastewater'

import random as random

def sampleDataBin(bin):
    """
    this helper function is used when the probability bin selected for e.g. shower frequency, represents more than one value. For example if a frequency bin is selected that represents a shower
    frequency of 1 to 1.2 showers per day ,this helper function can be used to randomly select a variable between these two values.

    bin: an input in the form of e.g. 1-1.2 or 1,1.2
    """

    freq_bin = bin.split("-" or ",")                                       # Split the two numbers that make up the frequency range and assign to 'freq_bin'

    min = float(freq_bin[0])                                                            # The first number in the bin represents the min frequency
    max = float(freq_bin[1])                                                            # The second number in the bin represents the max frequency

    variable = random.uniform(min,max)                                          # Randomly sample between min and max and assign the value to self.__frequency

    return variable
