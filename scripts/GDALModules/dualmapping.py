from osgeo import ogr
import csv
from pydynamind import *

import pdb

class Dualmapping(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("file_name", STRING)
            self.file_name = "/tmp/link.csv"

            self.createParameter("view_name", STRING)
            self.view_name = "conduit"

        def init(self):
            self.network = ViewContainer(self.view_name, EDGE, READ)
            self.network.addAttribute("start_id", Attribute.INT, READ)
            self.network.addAttribute("end_id", Attribute.INT, READ)
            self.network.addAttribute("segment_id", Attribute.INT, READ)

            self.registerViewContainers([self.network])

        def run(self):
            #Go from end_node up to every leaf, every conduit can only be accessed once.
            self.visited = {}
            self.node_edge_list = {}
            self.edge_list = {}
            self.cluster_list = {}
            self.cluster_id = 0

            self.network.reset_reading()

            for n in self.network:
                f_id = n.GetFID()
                start_node = n.GetFieldAsInteger("start_id")
                end_node = n.GetFieldAsInteger("end_id")
                segment_id = n.GetFieldAsInteger("segment_id")

                self.edge_list[f_id] = (start_node, end_node, segment_id, f_id)
                self.add_edge(start_node, f_id)

            self.links = set()
            for n in self.network:
                f_id = n.GetFID()
                segment_id = n.GetFieldAsInteger("segment_id")
                start_node = n.GetFieldAsInteger("start_id")
                end_node = n.GetFieldAsInteger("end_id")
                try:
                    for e in self.node_edge_list[start_node]:
                        self.add_link(e, f_id, segment_id)

                except KeyError:
                    pass
                try:
                    for e in self.node_edge_list[end_node]:
                        self.add_link(e, f_id, segment_id)
                except KeyError:
                    pass
            counter = 0
            f = open(self.file_name, 'w')
            f.write( "id;start_id;end_id\n")
            for l in self.links:
                counter+=1
                # print counter, l[0], l[1]
                f.write(str(counter) + ";" + str(l[0]) + ";" + str(l[1]) + "\n")

            f.close()
            self.network.finalise()

        def add_link(self, e, f_id, segment_id):
            segment_id_connection = self.edge_list[e][2]
            if segment_id != segment_id_connection:
                link = (segment_id, segment_id_connection)
                if segment_id > segment_id_connection:
                    link = (segment_id_connection, segment_id)
                self.links.add(link)

        def add_edge(self, node_id, edge_id):
            try:
                self.node_edge_list[node_id].append(edge_id)
            except KeyError:
                self.node_edge_list[node_id] = []
                self.node_edge_list[node_id].append(edge_id)

