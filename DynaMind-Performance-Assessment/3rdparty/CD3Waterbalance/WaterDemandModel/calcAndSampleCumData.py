__author__ = 'JBreman'
__project__ = 'WaterDemandModel'


import numpy as np
import os

import extractCsvData as EC



def createCumData(input):
    """
    This method creates a cumulative distribution of an 2D input array - contain x values and y values.
        The 2D array is split into two 1D arrays (one for x vals, one for y vals) to make the process of cumulating the y vals more simple

        Input: a 2D array in the form array = [[x1,y1],[x2,y2],......[xn,yn]]

        Outputs: Xvals: is a list of all the x values of the DF Pattern (i.e. time t values from 1hr to 24 hrs)
                 Cumm : is the cumulative distribution of the y values (probabilities).
    """

    inputArray = input

    Xvals = []
    Yvals = []

    "Split 2D inputArray contain x and y data into 2 1D arrays"

    for i in range(len(inputArray)):
        Xvals.append(inputArray[i][0])           # append all x values (in first column of DF_P) to the array Xvals
        Yvals.append(inputArray[i][1])           # append all y values (in the second column of DF_P) to the array Yvals


    "Convert yVals into a cumulative distribution"          # i.e. c1 = y1, c2 = c1 + y2, c3 = c2 + y3............... where y = Yvals, c = cumulative values

    Cumm = []                                  # Create a empty array to store the cumulative distribution of the y values (probabilities)
    Cumm.append(Yvals[0])                      # append the first value of the probabilities to Cumm (c1 = y1)

    del Yvals[0]                               # delete the first values of the probabilities in Yvals --> this shifts the Yvals list so that c(i-1) and y(i) (i.e c2 = c1 + y2), are at the same index point [i] and can be easily added together

    for i in range(len(Yvals)):                # for the length of Yvals:
        Cumm.append(Cumm[i]+Yvals[i])          # append c(i-1) + y(i) to Cumm.

    return Cumm, Xvals                     # return the cumulative distribution of the demand factors


def sampleCumData(cumData,x_values):
    """
    This method randomly samples from a cumulative distribution.

    It works by randomly sampling an array of cumulative data. The index of the data point in the array is then found. This index is then used to find the cum data points corresponding x value.

    This method is used to e.g. sample out of a distribution of household sizes

    Output: the corresponding x value of the sample cumulative data point

    :param cumData: is a 1D array of cumulative data
           x_values: is a 1D array of the corresponding x values for the cumulative data
    """

    rand = np.random.sample(1)  # sample a number (float) between 0 and 1

    "Find the cumulative value that is closest to the random number generated:"     # As there wont be an exact match between rand and a value in the cumData

    for i in range(len(cumData)):           # For the length of the array cumData:
        if rand < cumData[i]:                   # Once rand < cumData[i] it means that this is the cumData point [i] that most closely matches the random variable and it should be returned:
            val = cumData[i]                        # return the probability value found at row i
            break                                   # leave the if statement --> otherwise you would get all the values from this row[i] until the end of the array

    cummData_index = cumData.index(val)     # Find the index of the cumData probability value returned (i.e. its position in the array)

    "Find the corresponding x value household size) for the randomly selected cumulative probability value:"

    x_output = x_values[cummData_index]     # Since the order of the x_values and cumData arrays are the same, the index of the cumulative probability value that was randomly selected can be used to find its corresponding x value

    return x_output                         # Outputs from method

def getSampledCumValue(db):

    """
    This method uses the two above methods to get a randomly sampled value with only the file path as an input.
    :param db: input data already converted into array form. This is done in the helper method 'sampling_db'.
    """
    InputArray = db                         # get 2D array with x  and y (probability) data. db = array stored in database in the helper file 'sampling_db'.

    "Create cummulative distribution"
    CumDataArray = createCumData(InputArray)[0]  # For the y data - make a cumulative distribution and put it in its own 1D array

    xValsArray = createCumData(InputArray)[1]  # For the x data - no changes to data made. Just put into its own 1D array

    "Sample from distribution and return corresponding x value"

    x_value = sampleCumData(CumDataArray, xValsArray)

    return x_value


if __name__ == "__main__":

    dir = os.path.abspath("csvFiles\ShowerDuration.csv")

    for i in range(1000):

        print getSampledCumValue(dir)



