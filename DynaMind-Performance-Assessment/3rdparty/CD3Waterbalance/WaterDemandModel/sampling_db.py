import extractCsvData as CD
import config

class Singleton:
    """
    A non-thread-safe helper class to ease implementing singletons.
    This should be used as a decorator -- not a metaclass -- to the
    class that should be a singleton.

    The decorated class can define one `__init__` function that
    takes only the `self` argument. Other than that, there are
    no restrictions that apply to the decorated class.

    To get the singleton instance, use the `Instance` method. Trying
    to use `__call__` will result in a `TypeError` being raised.

    Limitations: The decorated class cannot be inherited from.
    """

    def __init__(self, decorated):
        self._decorated = decorated

    def Instance(self):
        """
        Returns the singleton instance. Upon its first call, it creates a
        new instance of the decorated class and calls its `__init__` method.
        On all subsequent calls, the already created instance is returned.

        """
        try:
            return self._instance
        except AttributeError:
            self._instance = self._decorated()
            return self._instance

    def __call__(self):
        raise TypeError('Singletons must be accessed through `Instance()`.')

    def __instancecheck__(self, inst):
        return isinstance(inst, self._decorated)


@Singleton
class SamplingDatabase:
    """
    This class is used to read each csv file used for the model, convert it into a array using the relevent method, and then stores each array in
    a dictionary [key:array]. This means that each csv file is only read once, from then on the model just uses the data in the array as this
    never changes.

    The two methods called to create the arrays are:
        def createInputArray: creates a 2D array in the form [[x1,y1],[x2,y2]...] --> this is used for most csv files

        def creatInputArrayWithDummy: is used when each x axis label represents a range e.g. 1-2, but an x axis array of numbers (float or int)
        is needed. Method creates a seperate array for y values, x values and x dummy values.
    """
    __instance = None
    __database = {}
    __database["RESIDENTIAL"]={}
    __database["COMMERCIAL"]={}

    def __init__(self):

        self.__database["RESIDENTIAL"]["DIURNAL_PATTERN"]= CD.createInputArray(config.RESIDENTIAL_DIURNAL_PATTERN_DATABASE)
        self.__database["RESIDENTIAL"]["RESIDENTIAL_UNIT_SIZE_DATABASE"] = CD.createInputArray(config.RESIDENTIAL_UNIT_SIZE_DATABASE)
        self.__database["RESIDENTIAL"]["TOILET_FLUSH_FREQ_DATABASE"] = CD.createInputArray(config.RESIDENTIAL_TOILET_FLUSH_FREQ_DATABASE)
        self.__database["RESIDENTIAL"]["SHOWER_FREQUENCY_DATABASE"] = CD.createInputArray(config.RESIDENTIAL_SHOWER_FREQUENCY_DATABASE)
        self.__database["RESIDENTIAL"]["SHOWER_DURATION_DATABASE"] = CD.createInputArrayWithDummy(config.RESIDENTIAL_SHOWER_DURATION_DATABASE)
        self.__database["RESIDENTIAL"]["BATH_FREQUENCY_DATABASE"] = CD.createInputArray(config.RESIDENTIAL_BATH_FREQUENCY_DATABASE)
        self.__database["RESIDENTIAL"]["WASH_MACHINE_FREQ_DATABASE"] = CD.createInputArray(config.RESIDENTIAL_WASH_MACHINE_FREQ_DATABASE)

        self.__database["COMMERCIAL"]["DIURNAL_PATTERN"] = CD.createInputArray(config.COMMERCIAL_DIURNAL_PATTERN_DATABASE)
        self.__database["COMMERCIAL"]["BUILDING_SIZE"] = CD.createInputArray(config.COMMERCIAL_BUILDING_NLA)
        self.__database["COMMERCIAL"]["M^2_PER_OCCUPIED_WORKPOINT"] = CD.extractCsvData(config.COMMERCIAL_BUILDING_OWP)
        self.__database["COMMERCIAL"]["TOILET_FLUSH_FREQ_DATABASE"] = CD.createInputArray(config.COMMERCIAL_TOILET_FLUSH_FREQ_DATABASE)
        self.__database["COMMERCIAL"]["SHOWER_FREQUENCY_DATABASE"] = CD.createInputArray(config.COMMERCIAL_SHOWER_FREQUENCY_DATABASE)
        self.__database["COMMERCIAL"]["SHOWER_DURATION_DATABASE"] = CD.createInputArrayWithDummy(config.COMMERCIAL_SHOWER_DURATION_DATABASE)
        self.__database["COMMERCIAL"]["BATH_FREQUENCY_DATABASE"] = CD.createInputArray(config.COMMERCIAL_BATH_FREQUENCY_DATABASE)






        self.__database["TAP_USE_DURATION_DATABASE"] = CD.createInputArray(config.TAP_USE_DURATION_DATABASE)
        self.__database["TAP_FLOWRATE_DATABASE"] = CD.createInputArray(config.TAP_FLOWRATE_DATABASE)


        self.__database["FRACTION_TOP_LOADERS_IN_AUST"] = config.FRACTION_TOP_LOADERS_IN_AUST
        self.__database["DISTRIBUTIONS"] = config.DISTRIBUTIONS


        self.__database["TOILET_FLUSH_VOLUME_DATABASE"] = CD.createInputArray(config.TOILET_FLUSH_VOLUME_DATABASE)
        self.__database["SHOWER_FLOWRATE_DATABASE"] = CD.createInputArray(config.SHOWER_FLOWRATE_DATABASE)
        self.__database["TOP_LOADER_WM_VOLUME_DATABASE"] = CD.createInputArray(config.TOP_LOADER_WM_VOLUME_DATABASE)
        self.__database["FRONT_LOADER_WM_VOLUME_DATABASE"] = CD.createInputArray(config.FRONT_LOADER_WM_VOLUME_DATABASE)
        self.__database["DISH_WASHER_VOLUME_DATABASE"] = CD.createInputArray(config.DISH_WASHER_VOLUME_DATABASE)

    def getDB(self, data):
        """
        Return simulation based on the current session id

        location = where the demand is being generated e.g. house, commercial building
        data = data required e.g. diurnal pattern, shower frequency etc for that particular location
        """
        if not self.__database.has_key(data):
            print "error - this database does not exist"
            return []

        return self.__database[data]


    def getDBTest(self, location,data):
        """
        Return simulation based on the current session id

        location = where the demand is being generated e.g. house, commercial building
        data = data required e.g. diurnal pattern, shower frequency etc for that particular location
        """
        if not self.__database.has_key(location):
            print "error - this database does not exist"
            return []
        return self.__database[location][data]




