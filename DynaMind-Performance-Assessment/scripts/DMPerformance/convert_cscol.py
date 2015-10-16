from osgeo import ogr
import csv
from pydynamind import *

import pdb

class ConvertCsCOL(Module):

        display_name = "ConvertCsCOL"
        group_name = "Little Helper"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)


        def init(self):
            self.pipes = ViewContainer("conduit", EDGE, READ)
            self.pipes.addAttribute("cscol1", Attribute.STRING, READ)
            self.pipes.addAttribute("cscol2", Attribute.STRING, READ)
            self.pipes.addAttribute("type", Attribute.STRING, MODIFY)
            self.pipes.addAttribute("diameter", Attribute.DOUBLE, WRITE)
            self.pipes.addAttribute("width", Attribute.DOUBLE, WRITE)
            self.registerViewContainers([self.pipes])

        def run(self):
            #Go from end_node up to every leaf, every conduit can only be accessed once.

            self.pipes.reset_reading()
            for p in self.pipes:
                # if str(p.GetFieldAsString("type")) != 'HW-closed':
                #     continue
                #print p.GetFieldAsString("cscol1")
                height = map(float, str(p.GetFieldAsString("cscol1")).split())
                width = map(float, str(p.GetFieldAsString("cscol2")).split())
                #print width
                #print height
                p.SetField("width", max(width) - min(width))
                p.SetField("diameter", max(height) - min(height))
                p.SetField("type", "RECT_CLOSED")


            self.pipes.finalise()

