__author__ = 'JBreman'
__project__ = 'WaterDemandModel'

import pylab as plt

import numpy as np

class DiurnalPattern:
    """
    This class converts an input Diurnal Pattern into a series of probabilities that describe the likelihood that water will be used at
    a certain time of day.

    Inputs: csv file of a Diurnal Pattern. X axis = timestep (hourly), Y axis = average demand (L)/ per household for each timestep

    Output : array of diurnal factors for each timestep. Where the high the factor (above 1) the higher the probability that water will be used at a particular timestep

    """

    def returnCSVFile(self, filepath):
        """
            Converts input CSV file into a usable form i.e. array

            Code assumptions: input csv has a header line (which is skipped by the code)

            :param filepath: input Diurnal Pattern (containing headings)
        """

        f = open(filepath, 'r')  # Open the csv file
        f.readline()  # read the 1st line (contains headings)
        datavec = []  # create a empty array to fill

        for lines in f:  # for each row in the file:
            datavec.append(lines)  # Add it to datavec

        f.close()  # close the csv file

        for i in range(len(datavec)):  # for the length of the array:
            datavec[i] = datavec[i].split(",")  # for each row split columns based on ","
            datavec[i][len(datavec[i]) - 1] = datavec[i][len(datavec[i]) - 1].rstrip(
                "\n")  # remove the "\n" that exists at the end of each row
            for col in range(len(datavec[i])):  # for each column in the row:
                datavec[i][col] = float(datavec[i][col])  # convert the data point to a float value

        return datavec


    def diurnalFactor(self, filepath):
        """
        requirements: input Diurnal Pattern for one day (hourly timestep). Where at time = t, the average L of water used per household is provided (demand at t) - showing the change in consumption of water during the day

        This function coverts the input Diurnal Pattern into a series of probabilities that describe the likelihood of an event occurring at timestep = t, taking into account the diurnal variation of the input timeseries.
        Where the sum of the probabilities = 1

        It works by firstly scaling the input diurnal pattern to have an average value of 1. This is done by dividing each y value (average L/property at time t) by the overall average L/property of the whole timeseries
        Secondly the each y value is divided by the number of hours in a day (24) to convert each factor into a probability of event occurance. Where the sum of the probabilities = 1.

        This function is used to help determine whether a certain appliance e.g. shower is used by the household at a particular time during the day. Whereby the data is used to create a cumulative distribution curve
        which can then be randomly sampled from.

        :param filepath: input Diurnal Pattern (contains headings)
        """

        input_pattern = self.returnCSVFile(filepath)  # input Diurnal pattern - in form average L per household at timestep t

        "Find the average water use of the household for the day:"

        count = len(input_pattern)  # Number of timesteps

        # Find the total demand of water used for the day:

        tot_demand = 0  # create a object for tot demand. Set value to zero.
        for i in range(len(input_pattern)):         # For each row of the input pattern:
            tot_demand += input_pattern[i][1]           # Add the value in the 2nd column to tot_demand. Where this value = the average L used per household at timestep t
                                                        # After the for loop is complete tot_demand contains the sum of all the demands for the diurnal pattern

        # Find the average demand for the day:

        average = tot_demand / count  # average demand = total demand / number of timesteps


        "Convert the diurnal pattern into a series of dimensionless numbers that signify the probability of an event occuring at time = t"

        DF_pattern = input_pattern

        for i in range(len(DF_pattern)):  # For each row of DF_pattern:
            DF_pattern[i][1] = DF_pattern[i][1] / average   # Divide the value in column 2 by the average. i.e. divide the L/household demand at time step t by the overall average demand for the day

            DF_pattern[i][1] = DF_pattern[i][1] / (24)   # Divide these factors by the number of hours in a day -> creates a series of probabilities where the sum(DFs) = 1


        return DF_pattern  # Return the scaled dimensionless DF pattern


    def cumulativeDFs(self, filepath):
        """
        This method creates a cumulative distribution of the Diurnal Factor pattern.
        The 2D DF_pattern array is split into two 1D arrays (one for x vals, one for y vals) to make the process of cumulating the y vals more simple

        Outputs: DF_xVals: is a list of all the x values of the DF Pattern (i.e. time t values from 1hr to 24 hrs)
                 DF_cumm : is the cumulative distribution of the y values (probabilities).

        These outputs are then used so that a number can be randomly selected from DF_Cumm, where its corresponding x value (time) is returned
        : param filepath: input Diurnal Pattern
        """

        DF_pattern = self.diurnalFactor(filepath)

        "Split DF_pattern array into 2 arrays"

        DF_xVals = []       # contains x axis values i.e. time (t)
        DF_yVals = []       # contains y axis values i.e. probabilities

        for i in range(len(DF_pattern)):            # for the length of the list DF_pattern:
            DF_xVals.append(DF_pattern[i][0])           # append all x values (in first column of DF_P) to the array DF_xVals
            DF_yVals.append(DF_pattern[i][1])           # append all y values (in the second column of DF_P) to the array DF_yVals

        "Convert yVals into a cumulative distribution"          # i.e. c1 = y1, c2 = c1 + y2, c3 = c2 + y3............... where y = yVals, c = cumulative values

        DF_cumm = []                                # Create a empty array to store the cumulative distribution of the y values (probabilities)
        DF_cumm.append(DF_yVals[0])                 # append the first value of the probabilities to DF_cumm (c1 = y1)

        del DF_yVals[0]                             # delete the first values of the probabilities in DF_yVals --> this shifts the yVals list so that c(i-1) and y(i) (i.e c2 = c1 + y2), are at the same index point [i] and can be easily added together

        for i in range(len(DF_yVals)):                      # for the length of DF_yVals:
            DF_cumm.append(DF_cumm[i]+DF_yVals[i])              # append c(i-1) + y(i) to cumm.


        return DF_cumm, DF_xVals                              # return the cumulative distribution of the demand factors


    def sampleCummDF(self,filepath):
        """
        requirement: cumulative DF Pattern from def cumulativeDFs

        This method is used to randomly sample from the cumulative distribution. Where a probability value on the y axis is randomly selected, and its corresponding x value (time) is returned.

        Output: time that an event (e.g. shower being used by a actor) occurs.

        :param filepath: input Diurnal Pattern
        """
        DF_cumm = self.cumulativeDFs(filepath)[0]           # Get array if the cummulative DF values

        xVals = self.cumulativeDFs(filepath)[1]             # Get array with the corresponding x values (time t)

        rand = np.random.sample(1)                          # sample a number (float) between 0 and 1

        "Find the cummulative DF value that is closest to the random number generated:"     # As there wont be an exact match between rand and a value in the DF_cumm

        for i in range(len(DF_cumm)):                      # For the length of the array DF_cumm:
             if rand < DF_cumm[i]:                              # Once rand < DF_cumm[i] it means that this is the DF_cumm that most closely matches the random variable and there should be returned:
                 val = (DF_cumm[i])                                 # return the DF value found at row i
                 break                                              # leave the if statement --> otherwise you would get all the values

        DF_index = DF_cumm.index(val)                      # find the index of the DF value returned (i.e. its position in the array)

        "Find the corresponding x value {time t) for the randomly selected cumulative DV value:"

        time = xVals[DF_index]                             # Since the order of the xVals and DF_cumm arrays are the same, the index of the cumulative DF value that was randomly selected can be used to find its corresponding x value (time)

        return  time                                       # outputs from method

    def plotCummDF(self, filepath):
        """
        plots the cummulative distribution of DFs produced by "def cumulative DFs"
        :param filepath:
        """

        x_axis = self.cumulativeDFs(filepath)[1]
        y_axis = self.cumulativeDFs(filepath)[0]

        plt.plot(x_axis,y_axis)

        plt.show()



"Testing:"

if __name__ == "__main__":
    # returnCSVFile("C:\Users\jbreman\Dropbox\PhD_Docs\Junk_DiurnalPattern.csv")

    Output = DiurnalPattern()

    # Output.diurnalFactor("C:\Users\jbreman\Dropbox\PhD_Docs\Junk_DiurnalPattern.csv")

    # Output.cumulativeDFs("C:\Users\jbreman\Dropbox\PhD_Docs\Junk_DiurnalPattern.csv")

    # Output.plotCummDF("C:\Users\jbreman\Dropbox\PhD_Docs\Junk_DiurnalPattern.csv")

    Output.sampleCummDF("C:\Users\jbreman\Dropbox\PhD_Docs\Junk_DiurnalPattern.csv")




