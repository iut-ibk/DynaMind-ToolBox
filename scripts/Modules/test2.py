from pydynamite import *


class WhiteNoise2(Module):
        def __init__(self):
            Module.__init__(self)
	    print "GUGUG"
    
        def run(self):
            print "Hello World4!"
	    print self.__module__.split(".")[0]

        def getClassName(self):
            return self.__class__.__name__

        def getFileName(self):
            return self.__module__.split(".")[0]
