__author__ = 'JBreman'
__project__ = 'WaterDemandModel'

from scipy import stats

import numpy as np

import pylab as plt


class logNormDist:
    """
    This class contains method related to creating,sampling and plotting lognormal distributions

        def returnCSVData: used to extract data from a csv file

        def constructDistribution: generates a distribution from the input file
        def findShapeAndScale: finds the shape and scale of the distribution
        def findMuAndSigma: converts the shape and scale into sigma and mu

        def randomlySample: samples from the generated distribution using sigma and my as inputs

        def createLogNormDist: generates a graph of the distributions

        def generatePlot: plots the generated distribution
    """


    def returnCSVData(self, filepath):
        """
        This method is used to convert a csv file containing distribution data (histogram bin format) into a usable format.
        This csv file should contain 2 columns:
            - one containing the x axis data (Xvals)
            - one containing the y axis data. (Yvals)

        Output: The method outputs the data as two seperate arrays; one containing the x axis data and one containing the y axis data
        This is then used by the method constructDistribution.

        For bin data where the x labels cover a range of variables e.g. 2 - 4 L/min, an array of x axis dummy data is created. This is required because the
        other methods in this class require integer/float values for the x axis. Using the dummy data does not change the shape of the distribution. Outputs when
        the x axis input data is in this form are:
            - Yvals: y axis bin points
            - Xvals: x axis bin points (original e.g. 2-4)
            - XVals_dummy: dummy xvals as integers. Created for the len(Xvals). For is e.g [1,2,3,n)

        :param filepath: Is the location of the csv file
        """
        "Append the data to datavec:"
        f = open(filepath,'r')      # Open the csv file
        f.readline()                # read the 1st line (contains headings)
        datavec = []                # create a empty array to fill

        for lines in f:             # for each row in the file:
            datavec.append(lines)   # Add it to datavec

        f.close()                   # close the csv file

        "Convert data to a usable form - need two arrays for x and y axis data"
        Xvals = []                          # Empty array to store the x-axis data
        Xvals_dummy = []
        Yvals = []                          # Empty array to store the y axis data
        for lines in datavec:               # For the length of the array:
            x,y = lines.split(',',1)        # split the data based on "," and separate into two different lines (x,y)
            Yvals.append(float(y))          # append the values in line y to Yvals

            try :                           # if Xvals are numbers:
                Xvals.append(float(x))              # append the values in line x to Xvals

            except ValueError:              # if they arent numbers and a Error is returned:
                Xvals.append(x)                         # append Xvals but dont convert to float. Will be in e.g '0-2' format
                Xvals_dummy = range(1,len(Xvals)+1,1)   # create a dummy Xvals array that contains numbers and can be used the following methods to create a logNormDist. range(start,stop,step)

        return Xvals, Yvals, Xvals_dummy

    def constructDistribution(self,filepath):
        """
        Inputs: requires data from a distribution/histogram that states the frequency of occurance (y axis) for certain e.g. shower. The sum of the frequency values must = 1
        durations. This comes from def returnCSVData

        It uses this distribution to create a set of numbers (e.g. 1000 numbers) that matches the frequency distribution. The numbers generated are in the same range
        as the Xvals inputted from def returnCSVData.
        E.g. if shower duration ranges between 1 - 10 min (according to Xvals). And the frequency of occurance of a 5 min shower is 0.2. If this method produces 100 numbers,
        the number 5 will come up 20 times. It isn't this accurate - is slightly off.

        :param filepath: Is the location of the csv file needed for def returnCSVData
        """

        "Generate a list of random numbers that fits the input distribution"

        Xvals = self.returnCSVData(filepath)[0]
        Xvals_dummy = self.returnCSVData(filepath)[2]
        Yvals = self.returnCSVData(filepath)[1]

        # values of x that occur with the probability pk:
        if type(Xvals[0]) == str:                   # integers are needed for xk to run this method. Therefore if Xvals contains str data:
            xk = Xvals_dummy                                # Use dummy data (int)
        else:
            xk = Xvals                                      # Use actual data (float)

        pk = Yvals           # probabilities for x values

        custm = stats.rv_discrete(values=(xk,pk))       # custom made discrete distribution based on xk and pk.

        rand_num = []                                   # Empty array

        "Make an array of numbers within the range defined by xVals that matches the custom distribution"

        for i in range(1000):                  # For a 1000 repetitions:
            R = custm.rvs()                         # generate a number R, based on the distribution
            rand_num.append(R)                      # append it to the rand_numb array

        return rand_num                        # Return the array of 1000 numbers that fits the custom distribution

    def findShapeAndScale(self,filepath):
        """
        Requirements: uses the output from def constructDistribution

        This method fits the set of generated data from def constructDistribution to a lognormal distribution.
        From this the values shape,loc,scale are returned as outputs.

        Shape = Sigma = standard deviation of log(x). i.e. you would need to take the exponential of sigma (e^sigma) to get the generic Std
        Scale = e^mu where mu = mean of log(x). It compresses or lengthens the distribution. You would need to take the exponential of mu (e^mu) to get the generic mean
        loc = location of the distrubution - it defines the start point. This is set to zero

        stats.lognorm.fit(data,location): returns three outputs: shape, loc,scale

        :param filepath: Is the location of the csv file needed for def returnCSVData
        """
        shape = stats.lognorm.fit(self.constructDistribution(filepath),floc=0)[0]       # calc shape (position [0] from the function lognorm.fit

        loc = stats.lognorm.fit(self.constructDistribution(filepath),floc=0)[1]         # calc loc (position [1] from the function lognorm.fit

        scale = stats.lognorm.fit(self.constructDistribution(filepath),floc=0)[2]       # calc scale (position [2] from the function lognorm.fit

        return shape, loc, scale

    def calcMuAndSigma(self, filepath):
        """
        Requirements: shape and scale from def findShapeAndScale

        Conferst shape and scale to my and sigma
        :param filepath: Is the location of the csv file needed for def returnCSVData
        """

        mu = np.log(self.findShapeAndScale(filepath)[2])        # scale = e^mu

        sigma = self.findShapeAndScale(filepath)[0]             #shape = sigma

        return mu,sigma

    def randomlySample(self, filepath):
        """
        Requirements: Uses the outputs from def calcMuAndSigma.

        This method is used to randomly select a value from a log normal distribution defined by mu and sigma.
        Where mu and sigma are derived from input data using the methods:
           def constructDistribution
           def findShapeAndScale
           def calcMuAndSigma

        The sampling method takes the log normal distribution in account. i.e. values of x with a higher frequency (y) are more likely
        to be generated.

        :param filepath: Is the location of the csv file needed for def returnCSVData
        """
        mu = self.calcMuAndSigma(filepath)[0]
        sigma = self.calcMuAndSigma(filepath)[1]

        rand = np.random.lognormal(mu,sigma,1000)

        "check its sampling ok:"                            # need to add the variable 'size' to rand above. i.e. size = 1000.
        count,bins,ignored = plt.hist(rand,normed=True)
        plt.ylim(ymax=0.4)
        plt.plot(bins,np.ones_like(bins))
        plt.show()

        return rand

    def createLogNormDist(self, filepath):
        """
        Requirements: need shape, loc, scale from def findShapeAndScale

        Generates the log normal distribution so that it can be graphed. The distribution is generated as both a
        probability density function (pdf) and a cumulative distribution function (cdf).

        Method was created so that both the cdf and pdf could be plotted.

        :param filepath: Is the location of the csv file needed for def returnCSVData
        """

        "create x axis values:"

        xmax = len((self.returnCSVData(filepath))[0])+10        # creates x axis range.
        xmin = 0.1                                              # x axis data start point
        num= 1000                                               # number of points to genereate

        x = np.linspace(xmin,xmax,num=num)                      #Return "num" numbers over evenly spaced intervals between xmin and xmax

        "Grab shape,location,scale from def findShapeAndScale:"
        shape = self.findShapeAndScale(filepath)[0]
        location = self.findShapeAndScale(filepath)[1]
        scale =self.findShapeAndScale(filepath)[2]

        "create the fitted lognorm PDF and CDF based on shape and scale:"
        pdf = stats.lognorm.pdf(x,shape,location,scale)
        cdf = stats.lognorm.cdf(x,shape,location,scale)

        return x, pdf, cdf


    def generatePlot(self,filepath):
        """
        Makes 2 plots:

        1) PDF distribution plotted against the input data shown as bar graphs
        2) CDF distribution

        Graphs are shown in 2 different windows.

        :param filepath: Is the location of the csv file needed for def returnCSVData
        """
        "-----PDF Distribution Plot---------"

        Xvals = self.returnCSVData(filepath)[0]
        Xvals_dummy = self.returnCSVData(filepath)[2]
        Yvals = self.returnCSVData(filepath)[1]

        "Plot bar graph of input data"
        if type(Xvals[0]) == str:         # Need numbers for x axis. Therefore if x values are str e.g. "0-0.2", then use the dummy data
            x_axis = Xvals_dummy                #Xvals_dummy

        else:                             # else return the actual data (float)
            x_axis = Xvals                      #Xvals

        y_axis = Yvals        #yVals

        plt.bar(x_axis,y_axis,align = 'center')         #plot the bar graph

        "extract x and pdf from def createLogNormDist"

        x = self.createLogNormDist(filepath)[0]
        pdf = self.createLogNormDist(filepath)[1]

        "Generate and show plot"
        PDF = plt.plot(x,pdf)                          # plot pdf distribution

        plt.show()                                     # Show the bar graph and distribution

        "---------------CDF Distribution Plot-------------"

        "extract cdf from def createLogNormDist"
        cdf =  self.createLogNormDist(filepath)[2]

        "Generate plot and show"
        CDF = plt.plot(x,cdf)                          # plot cdf distribution

        plt.show()                                     # show the cdf distribution

        return PDF,CDF



"Testing:"

if __name__ == "__main__":

    Dist=logNormDist()

    # Dist.returnCSVData("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # Dist.constructDistribution("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # Dist.findShapeAndScale("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # Dist.calcMuAndSigma("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # Dist.createLogNormDist("C:\Users\jbreman\Dropbox\PhD_Docs\junk.csv")

    Dist.generatePlot("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

    # Dist.randomlySample("C:\Users\jbreman\Dropbox\PhD_Docs\ShowerFlowRate.csv")

















