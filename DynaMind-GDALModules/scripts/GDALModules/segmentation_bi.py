from osgeo import ogr
import csv
from pydynamind import *

import pdb

class SegmentationUndirected(Module):

        display_name = "Undirected Segmentation"
        group_name = "Network Analysis"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("view_name", STRING)
            self.view_name = "conduit"

            self.createParameter("segmentation_parameter", STRING)
            self.segmentation_parameter = "strahler_order"

        def getHelpUrl(self):
            return "/DynaMind-GDALModules/segmentation.html"

        def init(self):
            self.network = ViewContainer(self.view_name , EDGE, READ)
            self.network.addAttribute("start_id", Attribute.INT, READ)
            self.network.addAttribute("end_id", Attribute.INT, READ)
            self.network.addAttribute(self.segmentation_parameter, Attribute.INT, READ)
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
            self.node_list = set()

            self.network.reset_reading()

            # Prepare search data
            for n in self.network:
                f_id = n.GetFID()
                start_node = n.GetFieldAsInteger("start_id")
                end_node = n.GetFieldAsInteger("end_id")

                if start_node == end_node:
                    continue
                strahler_order = n.GetFieldAsInteger(self.segmentation_parameter)

                self.edge_list[f_id] = (start_node, end_node, strahler_order, f_id)
                if start_node not in self.node_list:
                    self.node_edge_list[start_node] = []
                    self.node_list.add(start_node)
                if end_node not in self.node_list:
                    self.node_edge_list[end_node] = []
                    self.node_list.add(end_node)

                self.node_edge_list[start_node].append(f_id)
                self.node_edge_list[end_node].append(f_id)

            print len(self.node_list)
            self.deep = 0
            self.deeper = 0
            for edge_id in self.edge_list.keys():
                elements = []
                attribute = self.edge_list[edge_id][2]
                self.get_next_elements(edge_id, True, attribute, elements)
                while len(elements) > 0:
                    next_elements = []
                    for e in elements:
                        self.get_next_elements(e, False, attribute, next_elements)
                    elements = next_elements
                    # print len(elements)
            #
            #
            # print self.cluster_id
            # print len(self.visited_edges)
            # print len(self.cluster_list)
            # print self.deep
            # print self.deeper
            for p in self.network:
                f_id = p.GetFID()
                try:
                    cluster = self.cluster_list[f_id]
                except KeyError:
                    continue
                log(str(cluster), Debug)
                p.SetField("segment_id", cluster)
            self.network.finalise()

        def get_next_elements(self, edge_id, top_level, prev_attribute, next_elements):
            if edge_id in self.visited_edges:
                return
            if prev_attribute != self.edge_list[edge_id][2]:
                return
            if top_level:
                self.cluster_id += 1
            # id does not exist
            self.cluster_list[edge_id] = self.cluster_id
            self.visited_edges.add(edge_id)

            edge = self.edge_list[edge_id]
            for i in range(2):
                for e in self.node_edge_list[edge[i]]:
                    # continue while search criteria is fullfilled

                    self.deep+=1
                    if not top_level:
                        self.deeper+=1
                    next_elements.append(e)
                    # self.get_next_elements(e, False)








