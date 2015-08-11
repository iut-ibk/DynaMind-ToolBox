__author__ = 'JBreman'
__project__ = 'WaterDemandModel'


import numpy as np
import pylab as plt
import sampling_db as DB
import os
from scipy import stats

"""
This file contains method related to creating,sampling and plotting lognormal distributions

    def createInputArray: used to extract data from a csv file

    def constructDistribution: generates a distribution from the input file

    def findShapeAndScale: finds the shape and scale of the distribution

    def findMuAndSigma: converts the shape and scale into sigma and mu

    def randSampleLogNormDist: samples from the generated distribution using sigma and my as inputs

    def createLogNormDist: generates a graph of the distributions

    def generatePlot: plots the generated distribution

INPUT 'db':
This is the input array used to create the log norm distribution from. When using any of these functions the user needs to call the method
'sampling_db'. This method turns csv files into usable input arrays and then keeps the array's in a dictionary so that the csv file only needs to
be called once. An example of how you would call it would be:

import sampling_db as DB
input_array = DB.SamplingDataBase.Instance().getData("SHOWER_DURATION_DATABASE")
constructDistribution(input_array)
"""

def constructDistribution(db):
    """
        Inputs: requires data from a distribution/histogram that states the frequency of occurance (y axis) for certain e.g. shower. The sum of the frequency values must = 1
           This comes from def createInputArray

        It uses this distribution to create a set of numbers (e.g. 1000 numbers) that matches the frequency distribution. The numbers generated are in the same range
        as the Xvals inputted from def createInputArray.
        E.g. if shower duration ranges between 1 - 10 min (according to Xvals). And the frequency of occurance of a 5 min shower is 0.2. If this method produces 100 numbers,
        the number 5 will come up 20 times. It isn't this accurate - is slightly off.
        """

    "Generate a list of random numbers that fits the input distribution"

    input_array = db

    Xvals = input_array[0]
    Xvals_dummy = input_array[2]
    Yvals = input_array[1]

    # values of x that occur with the probability pk:
    if type(Xvals[0]) == str:  # integers are needed for xk to run this method. Therefore if Xvals contains str data:
        xk = Xvals_dummy  # Use dummy data (int)
    else:
        xk = Xvals  # Use actual data (float)

    pk = Yvals  # probabilities for x values

    custm = stats.rv_discrete(values=(xk, pk))  # custom made discrete distribution based on xk and pk.

    rand_num = []  # Empty array

    "Make an array of numbers within the range defined by xVals that matches the custom distribution"

    for i in range(1000):  # For a 1000 repetitions:
        R = custm.rvs()  # generate a number R, based on the distribution
        rand_num.append(R)  # append it to the rand_numb array

    return rand_num  # Return the array of 1000 numbers that fits the custom distribution


def findShapeAndScale(db):
    """
        Requirements: uses the output from def constructDistribution

        This method fits the set of generated data from def constructDistribution to a lognormal distribution.
        From this the values shape,loc,scale are returned as outputs.

        Shape = Sigma = standard deviation of log(x). i.e. you would need to take the exponential of sigma (e^sigma) to get the generic Std
        Scale = e^mu where mu = mean of log(x). It compresses or lengthens the distribution. You would need to take the exponential of mu (e^mu) to get the generic mean
        loc = location of the distrubution - it defines the start point. This is set to zero

        stats.lognorm.fit(data,location): returns three outputs: shape, loc,scale
        """
    shape = stats.lognorm.fit(constructDistribution(db), floc=0)[0]  # calc shape (position [0] from the function lognorm.fit

    loc = stats.lognorm.fit(constructDistribution(db), floc=0)[1]  # calc loc (position [1] from the function lognorm.fit

    scale = stats.lognorm.fit(constructDistribution(db), floc=0)[2]  # calc scale (position [2] from the function lognorm.fit

    return shape, loc, scale


def calcMuAndSigma(db):
    """
        Requirements: shape and scale from def findShapeAndScale

        Converts shape and scale to mu and sigma
        """

    mu = np.log(findShapeAndScale(db)[2])  # scale = e^mu

    sigma = findShapeAndScale(db)[0]  # shape = sigma

    return mu, sigma


def randSampleLogNormDist(db):
    """
        Requirements: Uses the outputs from def calcMuAndSigma.

        This method is used to randomly select a value from a log normal distribution defined by mu and sigma.
        Where mu and sigma are derived from input data using the methods:
           def constructDistribution
           def findShapeAndScale
           def calcMuAndSigma

        The sampling method takes the log normal distribution in account. i.e. values of x with a higher frequency (y) are more likely
        to be generated.


        """
    #mu = calcMuAndSigma(db)[0]
    #sigma = calcMuAndSigma(db)[1]
    #print mu,sigma

    rand = np.random.lognormal(DB.SamplingDatabase.Instance().getDB("DISTRIBUTIONS")["shower_duration"][0],DB.SamplingDatabase.Instance().getDB("DISTRIBUTIONS")["shower_duration"][1])

    # "check its sampling ok:"  # need to add the variable 'size' to rand above. i.e. size = 1000.
    # count,bins,ignored = plt.hist(rand,normed=True)
    # plt.ylim(ymax=0.4)
    # plt.plot(bins,np.ones_like(bins))
    # plt.show()

    return rand

def createLogNormDist(db):
    """
        Requirements: need shape, loc, scale from def findShapeAndScale

        Generates the log normal distribution so that it can be graphed. The distribution is generated as both a
        probability density function (pdf) and a cumulative distribution function (cdf).

        Method was created so that both the cdf and pdf could be plotted.
        """

    "create x axis values:"
    input_array = db
    xmax = len(input_array[0]) + 10  # creates x axis range.
    xmin = 0.1  # x axis data start point
    num = 1000  # number of points to genereate

    x = np.linspace(xmin, xmax, num=num)  # Return "num" numbers over evenly spaced intervals between xmin and xmax

    "Grab shape,location,scale from def findShapeAndScale:"
    shape = findShapeAndScale(input_array)[0]
    location = findShapeAndScale(input_array)[1]
    scale = findShapeAndScale(input_array)[2]

    "create the fitted lognorm PDF and CDF based on shape and scale:"
    pdf = stats.lognorm.pdf(x, shape, location, scale)
    cdf = stats.lognorm.cdf(x, shape, location, scale)

    return x, pdf, cdf


def generatePlot(db):
    """
        Makes 2 plots:

        1) PDF distribution plotted against the input data shown as bar graphs
        2) CDF distribution

        Graphs are shown in 2 different windows.
        """
    "-----PDF Distribution Plot---------"
    input_array = db
    Xvals = input_array[0]
    Xvals_dummy = input_array[2]
    Yvals = input_array[1]

    "Plot bar graph of input data"
    if type(Xvals[0]) == str:  # Need numbers for x axis. Therefore if x values are str e.g. "0-0.2", then use the dummy data
        x_axis = Xvals_dummy  # Xvals_dummy

    else:  # else return the actual data (float)
        x_axis = Xvals  # Xvals

    y_axis = Yvals  # yVals

    plt.bar(x_axis, y_axis, align='center')  # plot the bar graph

    "extract x and pdf from def createLogNormDist"

    x = createLogNormDist(input_array)[0]
    pdf = createLogNormDist(input_array)[1]

    "Generate and show plot"
    PDF = plt.plot(x, pdf)  # plot pdf distribution

    plt.show()  # Show the bar graph and distribution

    "---------------CDF Distribution Plot-------------"

    "extract cdf from def createLogNormDist"
    cdf = createLogNormDist(input_array)[2]

    "Generate plot and show"
    CDF = plt.plot(x, cdf)  # plot cdf distribution

    plt.show()  # show the cdf distribution

    return PDF, CDF


"Testing:"

if __name__ == "__main__":

    import sampling_db as DB

    dir = os.path.abspath("csvFiles\ShowerDuration.csv")

    # createInputArray("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # constructDistribution("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # findShapeAndScale("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # calcMuAndSigma("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # createLogNormDist("C:\Users\jbreman\Dropbox\PhD_Docs\junk.csv")

    # generatePlot("csvFiles\ShowerDuration.csv")
    array = DB.SamplingDatabase.Instance().getDB("SHOWER_DURATION_DATABASE")
    dir = os.path.abspath("csvFiles\ShowerDuration.csv")
    # createInputArray(dir)
    # print randSampleLogNormDist(dir)
    # generatePlot(dir)
    # constructDistribution(array)
    # findShapeAndScale(array)
    # calcMuAndSigma(array)
    # print randSampleLogNormDist(array)
    # print createLogNormDist(array)
    # generatePlot(array)
    randSampleLogNormDist(dir)













