from osgeo import ogr
import csv
from pydynamind import *

import pdb

class DM_DirectGraph(Module):

        display_name = "Direct Graph"
        group_name = "Network Generation"

        def getHelpUrl(self):
            return "/DynaMind-GDALModules/dm_value_from_raster.html"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("view_name", STRING)
            self.view_name = "conduit"

            self.createParameter("root_node", INT)
            self.root_node = 0

            self.createParameter("node_name", STRING)
            self.node_name = ""

        def init(self):
            self.pipes = ViewContainer(self.view_name, EDGE, READ)
            self.pipes.addAttribute("start_id", Attribute.INT, MODIFY)
            self.pipes.addAttribute("end_id", Attribute.INT, MODIFY)
            self.pipes.addAttribute("visited", Attribute.INT, WRITE)

            containers = [self.pipes]
            if self.node_name != "":
                self.nodes = ViewContainer(self.node_name, NODE, READ)
                self.nodes.addAttribute("node_id", Attribute.INT, READ)
                containers.append(self.nodes)

            self.registerViewContainers(containers)

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

            start_nodes = []
            if self.node_name == "":
                start_nodes.append(self.root_node)
            else:
                self.nodes.reset_reading()
                for n in self.nodes:
                    node_id  = n.GetFieldAsInteger("node_id")
                    start_nodes.append(node_id)

            self.visited_edges = set() # That is global ... I guess
            for s in start_nodes:
                print s
                leafs = [s]
                counter = 0

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
            if self.node_name != "":
                self.nodes.finalise()

        def add_edge(self, node_id, edge_id):
            try:
                self.node_edge_list[node_id].append(edge_id)
            except KeyError:
                self.node_edge_list[node_id] = []
                self.node_edge_list[node_id].append(edge_id)

        # start_id -----> end_id
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





