from osgeo import ogr, osr
from pydynamind import *

class ImportOriginDesinationMatrix(Module):

    display_name = "Import Origin Destination"
    group_name = "Transport"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("file_name", FILENAME)
        self.file_name = ""

        self.createParameter("view_name", STRING)
        self.view_name = "station"

        self.createParameter("append", BOOL)
        self.append = False

    def init(self):

        viewvector = []
        self.matrix = ViewContainer(self.view_name, COMPONENT, WRITE)
        self.matrix.addAttribute("origin_id", Attribute.INT, WRITE)
        self.matrix.addAttribute("destination_id", Attribute.INT, WRITE)
        self.matrix.addAttribute("value", Attribute.DOUBLE, WRITE)

        viewvector.append(self.matrix)

        if self.append:
            self.dummy = ViewContainer("dummy", SUBSYSTEM, MODIFY)
            viewvector.append(self.dummy)

        self.registerViewContainers(viewvector)

    def run(self):
        headers = None
        first_line = True
        with open(self.file_name) as f:
            for line in f:
                if first_line:
                    first_line = False
                    headers = line.split(",")
                    continue

                origin_id = None
                for idx, val in enumerate(line.split(",")):
                    if idx == 0:
                        try:
                            origin_id = int(val)
                        except:
                            break
                        continue
                    try:
                        destination_id = int(headers[idx])
                    except:
                        continue

                    element = self.matrix.create_feature()
                    element.SetField("origin_id", int(origin_id))
                    element.SetField("destination_id", destination_id)
                    try:
                        value = float(val)
                    except:
                        value = 0
                    element.SetField("value", value)

        self.matrix.finalise()




