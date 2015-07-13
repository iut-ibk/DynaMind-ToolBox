from osgeo import ogr
import csv
from pydynamind import *

import pdb

class Segmentation(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("view_name", STRING)
            self.view_name = "conduit"

            self.createParameter("root_node", INT)
            self.root_node = 0

        def init(self):
            self.network = ViewContainer(self.view_name , EDGE, READ)
            self.network.addAttribute("start_id", Attribute.INT, READ)
            self.network.addAttribute("end_id", Attribute.INT, READ)
            self.network.addAttribute("strahler_order", Attribute.INT, READ)
            self.network.addAttribute("segment_id", Attribute.INT, WRITE)

            self.registerViewContainers([self.network])

        def run(self):
            #Go from end_node up to every leaf, every conduit can only be accessed once.
            self.visited = {}
            self.node_edge_list = {}
            self.edge_list = {}
            self.cluster_list = {}
            self.cluster_id = 0
            self.visited_edges = set()

            self.network.reset_reading()



            for n in self.network:
                f_id = n.GetFID()
                start_node = n.GetFieldAsInteger("start_id")
                end_node = n.GetFieldAsInteger("end_id")
                strahler_order = n.GetFieldAsInteger("strahler_order")

                self.edge_list[f_id] = (start_node, end_node, strahler_order, f_id)
                self.add_edge(start_node, f_id)

            # print "start leave search"
            self.get_leafs(self.root_node)

            for p in self.network:
                f_id = p.GetFID()
                #if f_id in self.visited_edges:
                try:
                    cluster = self.cluster_list[f_id]
                except KeyError:
                    continue
                log(str(cluster), Debug)
                p.SetField("segment_id", cluster)
            self.network.finalise()


        def add_edge(self, node_id, edge_id):
            try:
                self.node_edge_list[node_id].append(edge_id)
            except KeyError:
                self.node_edge_list[node_id] = []
                self.node_edge_list[node_id].append(edge_id)

        def get_leafs(self, start_id, strahler_order=-1, cluster_id=0, prev_node_id=-1, node_from=-1):
            # Current height

            log(str(start_id), Debug)
            current_node_id = start_id


            # Check if node has a follow up
            # if start_id not in self.node_edge_list.keys():
            #     return

            try:
                potential_leafs = self.node_edge_list[start_id]
            except KeyError:
                return
            # print potential_leafs
            for pl in potential_leafs:
                if pl in self.visited_edges:
                    continue
                # Retrieve node
                edge = self.edge_list[pl]
                end_id = edge[1]
                strahler_order_next = edge[2]
                cluster_id_next = cluster_id
                if strahler_order != strahler_order_next:
                    self.cluster_id += 1
                    cluster_id_next = self.cluster_id
                self.cluster_list[pl] = cluster_id_next
                self.visited_edges.add(pl)
                self.visited[pl] = (start_id, end_id)
                self.get_leafs(end_id, strahler_order_next, cluster_id_next, current_node_id, start_id)

