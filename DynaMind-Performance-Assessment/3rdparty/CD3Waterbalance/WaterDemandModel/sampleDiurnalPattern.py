__author__ = 'JBreman'
__project__ = 'WaterDemandModel'

import pylab as plt

import calcAndSampleCumData as SC

import os


"""
    This helper file is used to randomly select WHEN a event takes place during the day i.e. when someone has a shower. This is down for hourly intervals (total of 24hrs = 1 day)

    A diurnal pattern is used as a input, which is converted into a series of probabilities that describes the likelihood of an event taking place at a certain time of day. The probabilities are known
    as Diurnal Factors (DF).

    The probability distribution is then cumulatively summed. This allows for a y axis value (probability) to be randomly sampled, whereby its corresponding x axis value (time of day) is returned. Whereby the diurnal
    variability of the input data is taken into account while sampling.

    INPUT = 'db'
    This is the 2D input array used to create the cumulative distribution. When using any of these functions the user needs to call the method
    'sampling_db'. This method turns csv/text files into usable input arrays and then keeps the array's in a dictionary so that the csv file only needs to
    be called once. An example of how you would call it would be:

    import sampling_db as DB
    input_array = DB.SamplingDataBase.Instance().getData("SHOWER_DURATION_DATABASE")
    diurnalFactor(input_array)

    DIURNAL PATTERN INPUT FILE FORMAT : X axis = timestep (hourly), Y axis = average demand (L or m^3)/ per household for each timestep.


    Output :  - The time (at a certain hour) that an event takes place.
              - A plot of the cumulative distribution.

    """

def diurnalFactor(db):
    """
        requirements: input Diurnal Pattern for one day (hourly timestep). Where at time = t, the average L/m^3 of water used per household is provided (demand at t) - showing the change in consumption of water during the day


        This function coverts the input Diurnal Pattern into a series of probabilities that describe the likelihood of an event occurring at timestep = t, taking into account the diurnal variation of the input timeseries.
        Where the sum of the probabilities = 1

        It works by firstly scaling the input diurnal pattern to have an average value of 1. This is done by dividing each y value (e.g. average L/property at time t) by the overall average L/property of the whole timeseries
        Secondly each y value is divided by the number of hours in a day (24) to convert each factor into a probability of event occurance. Where the sum of the probabilities = 1.

        This function is used to help determine whether a certain appliance e.g. shower is used by the household at a particular time during the day. Whereby the data is used to create a cumulative distribution curve
        which can then be randomly sampled from.

        :param db: input Diurnal Pattern (contains headings), already converted to array form. This is done in the helper file 'sampling_db'.
        """
    input_pattern = db                                # input Diurnal pattern (average L per household at timestep t) in 2D array form

    "Find the average water use of the household for the day:"

    count = len(input_pattern)  # Number of timesteps

    # Find the total demand of water used for the day:

    tot_demand = 0  # create a object for tot demand. Set value to zero.
    for i in range(len(input_pattern)):  # For each row of the input pattern:
        tot_demand += input_pattern[i][1]  # Add the value in the 2nd column to tot_demand. Where this value = the average L used per household at timestep t
                                           # After the for loop is complete tot_demand contains the sum of all the demands for the diurnal pattern

    # Find the average demand for the day:

    average = tot_demand / count  # average demand = total demand / number of timesteps

    "Convert the diurnal pattern into a series of dimensionless numbers that signify the probability of an event occuring at time = t"

    DF_pattern = input_pattern

    for i in range(len(DF_pattern)):  # For each row of DF_pattern:
        DF_pattern[i][1] = DF_pattern[i][1] / average  # Divide the value in column 2 by the average. i.e. divide the L/household demand at time step t by the overall average demand for the day
        DF_pattern[i][1] = DF_pattern[i][1] / (24)  # Divide these factors by the number of hours in a day -> creates a series of probabilities where the sum(DFs) = 1

    return DF_pattern  # Return the scaled dimensionless DF pattern


def cumulativeDFs(db):
    """
        This method creates a cumulative distribution of the y values (DF probabilities) produced in def diurnalFactor.

        It uses the external method "createCumulativeData" to do this. See this method for details on how the outputs are produced

        Outputs: - Xvals: A 1D array of the xvals. Unchanged, just appended to its own array
                   DF_cum: a cumulative distribution of the DF factors within a 1D array

        These outputs are then used so that a number can be randomly selected from DF_Cumm, where its corresponding x value (time) is returned
        """

    DF_pattern = diurnalFactor(db)  # input 2D array containing x and y data

    DF_cum = SC.createCumData(DF_pattern)[0]  # create the cumulative distribution of the y data (probabilities)

    DF_Xvals = SC.createCumData(DF_pattern)[1]  # create a 1D array of the x data (time t)

    return DF_cum, DF_Xvals  # return the cumulative distribution of the demand factors


def sampleCumDF(db):
    """
        requirement: cumulative DF Pattern from def cumulativeDFs

        This method is used to randomly sample from the cumulative distribution. Where a probability value on the y axis is randomly selected, and its corresponding x value (time) is returned.

        This method uses the imported method "sampleCumData" to generate outputs. See this file for info on how the sampling is done.
        Output: time that an event (e.g. shower being used by a person) occurs.

        :param filepath: input Diurnal Pattern
        """

    DF_cumm = cumulativeDFs(db)[0]  # Get array of the cumulative DF values

    xVals = cumulativeDFs(db)[1]  # Get array with the corresponding x values (time t)

    time = SC.sampleCumData(DF_cumm, xVals)  # Randomly sample one variable from the cum probabilities array and return its corresponding x value (time)

    return time


def plotCummDF(db):
    """
        plots the cummulative distribution of DFs produced by "def cumulative DFs"
        :param filepath:
        """

    x_axis = cumulativeDFs(db)[1]
    y_axis = cumulativeDFs(db)[0]

    plt.plot(x_axis, y_axis)

    plt.show()


"Testing:"

if __name__ == "__main__":
#

    import sampling_db as DB

    input_array = DB.SamplingDatabase.Instance().getDB("DIURNAL_PATTERN_DATABASE")
    plotCummDF(input_array)






