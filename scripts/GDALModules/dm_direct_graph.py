from osgeo import ogr
import csv
from pydynamind import *

import pdb

class DM_DirectGraph(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("view_name", STRING)
            self.view_name = "conduit"

            self.createParameter("root_node", INT)
            self.root_node = 0

        def init(self):
            self.pipes = ViewContainer(self.view_name, EDGE, READ)
            self.pipes.addAttribute("start_id", Attribute.INT, MODIFY)
            self.pipes.addAttribute("end_id", Attribute.INT, MODIFY)
            self.pipes.addAttribute("visited", Attribute.INT, WRITE)

            self.registerViewContainers([self.pipes])

        def run(self):
            #Go from end_node up to every leaf, every conduit can only be accessed once.
            self.visited = {}
            self.node_edge_list = {}
            self.edge_list = {}
            self.corrected_list = {}
            self.pipes.reset_reading()
            log("start building search structure", Debug)
            counter = 0
            for p in self.pipes:
                counter+=1

                f_id = p.GetFID()
                if counter % 10000 == 0:
                    log(str(f_id), Debug)
                start_node = p.GetFieldAsInteger("start_id")
                end_node = p.GetFieldAsInteger("end_id")
                self.edge_list[f_id] = (start_node, end_node)
                self.add_edge(start_node, f_id)
                self.add_edge(end_node, f_id)

            log("start search", Debug)

            leafs = [self.root_node]
            counter = 0
            self.visited_edges = set()
            while len(leafs) > 0:
                if counter % 1000 == 0:
                    log(str(len(leafs)), Debug)

                new_leafs = []
                for l in leafs:
                    self.get_leafs(l, new_leafs)
                leafs = new_leafs


            log("write result", Debug)
            #Write list

            counter = 0
            for p in self.pipes:
                f_id = p.GetFID()
                if f_id in self.visited_edges:
                    edge = self.visited[f_id]
                    p.SetField("start_id", edge[0])
                    p.SetField("end_id", edge[1])
                    p.SetField("visited", 1)
            self.pipes.finalise()

        def add_edge(self, node_id, edge_id):
            #if node_id not in self.node_edge_list.keys():
            #    self.node_edge_list[node_id] = []
            try:
                self.node_edge_list[node_id].append(edge_id)
            except KeyError:
                self.node_edge_list[node_id] = []
                self.node_edge_list[node_id].append(edge_id)

        def get_leafs(self, start_id, new_leafs):
            potential_leafs = self.node_edge_list[start_id]

            leafs = []
            for pl in potential_leafs:
                if pl in self.visited_edges:
                    continue
                # Retrieve node
                edge = self.edge_list[pl]
                if start_id == edge[0]:
                    end_id = edge[1]
                else:
                    end_id = edge[0]

                new_leafs.append(end_id)
                self.visited_edges.add(pl)
                self.visited[pl] = (start_id, end_id)





