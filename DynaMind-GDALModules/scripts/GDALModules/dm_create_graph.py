from osgeo import ogr
from pydynamind import *

import pdb

class DM_CreateNeighbourhoodNetwork(Module):
        display_name = "Create Neighbourhood Network"
        group_name = "Network Generation"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("view_name", STRING)
            self.view_name = "district"

            self.createParameter("weight", STRING)
            self.weight = "weight"

            self.createParameter("node_view_name", STRING)
            self.node_view_name = ""

            self.createParameter("neigbourhood_table", STRING)
            self.neigbourhood_table = ""

            self.createParameter("network_name", STRING)
            self.network_name = ""

        def init(self):
            self.leading_view = ViewContainer(self.view_name, COMPONENT, READ)
            self.leading_view.addAttribute(self.weight, Attribute.DOUBLE, READ)

            self.node_view = ViewContainer(self.node_view_name, NODE, READ)
            self.node_view.addAttribute(str(self.view_name) + "_id", DM.Attribute.INT, READ)

            self.neigh = ViewContainer(self.neigbourhood_table, COMPONENT, READ)

            self.neigh.addAttribute(str(self.view_name) + "_id1", DM.Attribute.INT, READ)
            self.neigh.addAttribute(str(self.view_name) + "_id2", DM.Attribute.INT, READ)

            self.network = ViewContainer(self.network_name, EDGE, WRITE)

            self.registerViewContainers([self.leading_view, self.neigh, self.network, self.node_view])

        def run(self):
            node_weights = {}

            for n in self.leading_view:
                n_id = n.GetFID()
                weight = n.GetFieldAsDouble(self.weight)

                node_weights[n_id] = weight

            coordinates = {}
            for n in self.node_view:
                n_id = n.GetFieldAsInteger(str(self.view_name) + "_id")
                geom = n.GetGeometryRef()
                coordinates[n_id] = (geom.GetX(), geom.GetY())

            connections = {}
            for n in self.neigh:
                start_id = n.GetFieldAsInteger(str(self.view_name) + "_id1")
                end_id = n.GetFieldAsInteger(str(self.view_name) + "_id2")
                if not connections.has_key(start_id):
                    connections[start_id] = []
                con_vec = connections[start_id]
                con_vec.append(end_id)

            for n_id in node_weights.keys():

                try:
                    neigh = connections[n_id]
                except KeyError:
                    continue

                n_next = -1
                n_height = node_weights[n_id]

                for n in neigh:
                    if node_weights[n] < n_height:
                        n_next = n
                        n_height = node_weights[n]
                if n_next == -1:
                    continue
                        #Create new feature
                network = self.network.create_feature()

                #Create geometry
                line = ogr.Geometry(ogr.wkbLineString)
                p1 = coordinates[n_id]
                p2 = coordinates[n_next]

                line.SetPoint_2D(0,p1[0], p1[1])
                line.SetPoint_2D(1, p2[0], p2[1])

                network.SetGeometry(line)


            self.network.finalise()
            self.node_view.finalise()
            self.neigh.finalise()
            self.leading_view.finalise()

