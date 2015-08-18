__author__ = 'JBreman'
__project__ = 'WaterDemandModel'


def extractCsvData(filepath):
    """
    Method extracts data from a CSV file and appends it to an array ready for manipulation by other methods.

    This csv file should contain 2 columns:
        - one containing the x axis data (Xvals)
        - one containing the y axis data. (Yvals)

    ASSUMPTION: code assumes that input csv has a header row, which is skipped by the code and hence not included in the output array
    :param filepath:
    """

    f = open(filepath,'r')      # Open the csv file
    f.readline()                # read the 1st line (contains headings)
    datavec = []                # create a empty array to fill

    for lines in f:             # for each row in the file:
        datavec.append(lines)   # Add it to datavec

    f.close()                   # close the csv file

    return datavec


def createInputArray(filepath):
    """
        This method converts the input data into a 2D array of float and/or text data. i.e. array = [[x1,y1],[x2,y2],...[xn,yn]]

        It uses the imported method "extractCsvData" to transfer the raw data from a csv file to an array. Whereby it is then further manipulated by the following method.

        The method can cope with data that is either text or number format

        :param filepath: relevant CSV file path
        """

    datavec = extractCsvData(filepath)  # input data

    for i in range(len(datavec)):  # for the length of the array:
        datavec[i] = datavec[i].split(",")  # for each row split columns based on ","

        datavec[i][len(datavec[i]) - 1] = datavec[i][len(datavec[i]) - 1].rstrip("\n")  # remove the "\n" that exists at the end of each row

        for col in range(len(datavec[i])):  # for each column in the row:

            try:  # Try to:
                datavec[i][col] = float(datavec[i][col])  # convert the data point to a float value

            except ValueError:  # if the data isn't a number and an error is returned:
                pass  # leave data as is (don't convert to float)

    return datavec  # return the outputs

def createInputArrayWithDummy(filepath):
    """
        This method is used to convert input data containing distribution data (histogram bin format) into a usable format.

        Input: raw data from csv that has already been put into an array using the file extractCsvData.

        Output: The method outputs the data as two seperate arrays; one containing the x axis data and one containing the y axis data
        This is then used by the method constructDistribution.

        For bin data where the x labels cover a range of variables e.g. 2 - 4 L/min, an array of x axis dummy data is created. This is required because the
        other methods in this class require integer/float values for the x axis. Using the dummy data does not change the shape of the distribution. Outputs when
        the x axis input data is in this form are:
            - Yvals: y axis bin points
            - Xvals: x axis bin points (original e.g. 2-4)
            - XVals_dummy: dummy xvals as integers. Created for the len(Xvals). For is e.g [1,2,3,n)

        :param db: Is the location of the csv file
        """

    datavec = extractCsvData(filepath)

    "Convert data to a usable form - need two arrays for x and y axis data"
    Xvals = []  # Empty array to store the x-axis data
    Xvals_dummy = []
    Yvals = []  # Empty array to store the y axis data
    for lines in datavec:  # For the length of the array:
        x, y = lines.split(',', 1)  # split the data based on "," and separate into two different lines (x,y)
        Yvals.append(float(y))  # append the values in line y to Yvals

        try:  # if Xvals are numbers:
            Xvals.append(float(x))  # append the values in line x to Xvals

        except ValueError:  # if they arent numbers and a Error is returned:
            Xvals.append(x)  # append Xvals but dont convert to float. Will be in e.g '0-2' format
            Xvals_dummy = range(1, len(Xvals) + 1,1)  # create a dummy Xvals array that contains numbers and can be used the following methods to create a logNormDist. range(start,stop,step)

    return Xvals, Yvals, Xvals_dummy