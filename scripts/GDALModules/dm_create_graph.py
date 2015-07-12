from osgeo import ogr
from pydynamind import *

import pdb

class DM_CreateNeighbourhoodNetwork(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("view_name", STRING)
            self.view_name = "district"

            self.createParameter("weight", STRING)
            self.weight = "weight"

            self.createParameter("neigbourhood_table", STRING)
            self.neigbourhood_table = ""

            self.createParameter("network_name", STRING)

        def init(self):
            self.node_view = ViewContainer(self.view_name, EDGE, READ)
            self.node_view.addAttribute(self.weight, Attribute.DOUBLE, READ)

            self.neigh = ViewContainer(self.neigbourhood_table, COMPONENT, READ)
            self.neigh.addAttribute(str(self.view_name) + "_id1")
            self.neigh.addAttribute(str(self.view_name) + "_id2")

            self.network = ViewContainer(self.neigbourhood_table, EDGE, WRITE)

            self.registerViewContainers([self.view_name, self.neigh, self.network])

        def run(self):
            node_weights = {}

            for n in self.node_view:
                n_id = n.GetFID()
                weight = n.GetFieldAsDouble(self.weight)

                node_weights[n_id] = weight

            print node_weights





