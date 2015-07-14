# -*- coding: utf-8 -*- 

"""
@file
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2015  Christian Urich

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
"""

import sys

from pydynamind import *


class DM_ImportSWMM(Module):
    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("filename", FILENAME, "Name of SWMM File")
        self.filename = ""

        self.createParameter("name_outlet", STRING, "Identifier Outlet")
        self.name_outlet = ""


        # self.conduits.addLinkAttribute("XSECTION", "XSECTION", WRITE)

        # self.junctions = View("JUNCTION", NODE, WRITE)
        # self.junctions.addAttribute("Z", DOUBLE, WRITE)
        # self.junctions.addAttribute("D", DOUBLE, WRITE)
        # self.junctions.addAttribute("invert_elevation", DOUBLE, WRITE)
        # self.junctions.addAttribute("built_year", DOUBLE, WRITE)
        #
        # self.outfalls = View("OUTFALL", NODE, WRITE)
        # self.outfalls.addAttribute("Z", DOUBLE, WRITE)
        # Not imported
        # self.inlets = View("INLET", NODE, WRITE)


        # self.wwtps = View("WWTP", NODE, WRITE)
        #
        # self.storages = View("STORAGE", NODE, WRITE)
        # self.storages.addAttribute("Z", DOUBLE, WRITE)
        # self.storages.addAttribute("max_depth", DOUBLE, WRITE)
        # self.storages.addAttribute("type", STRING, WRITE)
        # self.storages.addAttribute("storage_x", DOUBLE, WRITE)
        # self.storages.addAttribute("storage_y", DOUBLE, WRITE)
        #
        # self.weirs = View("WEIR", EDGE, WRITE)
        # self.weirs.addAttribute("type", STRING, WRITE)
        # self.weirs.addAttribute("crest_height", DOUBLE, WRITE)
        # self.weirs.addAttribute("discharge_coefficient", DOUBLE, WRITE)
        # self.weirs.addAttribute("end_coefficient", DOUBLE, WRITE)
        #
        # self.pumps = View("PUMPS", EDGE, WRITE)
        # self.pumps.addAttribute("type", STRING, WRITE)
        # self.pumps.addAttribute("pump_x", DOUBLE, WRITE)
        # self.pumps.addAttribute("pump_y", DOUBLE, WRITE)

        # views.append(self.conduits)
        # views.append(self.nodes)
        # views.append(self.outfalls)
        # views.append(self.junctions)
        # views.append(self.inlets)
        # views.append(self.wwtps)
        # views.append(self.storages)
        # views.append(self.weirs)
        # views.append(self.xsections)
        # views.append(self.pumps)

        # self.registerViewContainers(views)


        # self.createParameter("NameWWTP", STRING, "Identifier WWTP")
        # self.NameWWTP = "MD020"

        # self.createParameter("defaultBuiltYear", INT, "Default_Built_Year")
        # self.defaultBuiltYear = 1900
        #
        # self.curves = {}
        # self.curves_types = {}

    def readCurves(self):
        try:
            f = open(self.filename)
            startReading = False

            for line in f:
                line = line.strip()
                if line is '':
                    continue
                if line[0] is ';':
                    continue
                if startReading == True and line[0] is '[':
                    startReading = False
                    break
                if startReading == True:
                    # print line
                    content = line.split()
                    if content[0] not in self.curves:
                        self.curves[content[0]] = []
                    values = self.curves[content[0]]
                    if (len(content) == 4):
                        values.append((float(content[2]), float(content[3])))
                    if (len(content) == 3):
                        values.append((float(content[1]), float(content[2])))
                    self.curves[content[0]] = values

                    if (len(content) == 4):
                        if content[1] != "":
                            self.curves_types[content[0]] = str(content[1])

                if line == "[CURVES]":
                    startReading = True
            f.close()

        except Exception, e:
            print e
            print sys.exc_info()

    def init(self):
        self.conduits = ViewContainer("conduit", EDGE, WRITE)
        self.conduits.addAttribute("start_id", Attribute.INT, WRITE)
        self.conduits.addAttribute("end_id", Attribute.INT, WRITE)

        self.pumps = ViewContainer("pump", EDGE, WRITE)
        self.pumps.addAttribute("start_id", Attribute.INT, WRITE)
        self.pumps.addAttribute("end_id", Attribute.INT, WRITE)

        self.weirs = ViewContainer("weir", EDGE, WRITE)
        self.weirs.addAttribute("start_id", Attribute.INT, WRITE)
        self.weirs.addAttribute("end_id", Attribute.INT, WRITE)
        # self.conduits.addAttribute("inlet_offset", Attribute.DOUBLE, WRITE)
        # self.conduits.addAttribute("outlet_offset", Attribute.DOUBLE, WRITE)
        # self.conduits.addAttribute("diameter", Attribute.DOUBLE, WRITE)

        # self.dummy = ViewContainer("dummy", SUBSYSTEM, MODIFY)

        # self.xsections = ViewContainer("xsection",COMPONENT,WRITE)
        # self.xsections.addAttribute("type", STRING, WRITE)
        # self.xsections.addAttribute("shape", STRING, WRITE)
        self.nodes_container = ViewContainer("node", NODE, WRITE)
        views = [self.nodes_container, self.conduits, self.pumps, self.weirs]
        if self.name_outlet != "":
            self.outlet = ViewContainer("outlet", NODE, WRITE)
            self.outlet.addAttribute("node_id", Attribute.INT, WRITE)
            views.append(self.outlet)
        self.registerViewContainers(views)

    def run(self):
        # try:
        # sewer = self.getData("Sewer")
        results = {}
        f = open(self.filename)
        currentContainer = ""
        for line in f:
            # print line
            line = line.strip()
            if line is '':
                continue
            if line[0] is ';':
                continue
            if line[0] is '[':
                results[line] = {}
                currentContainer = line
            if line is '':
                continue
            # First Section is always the Name
            content = line.split()
            container = []
            counter = 0
            if len(content) < 2:
                continue
            for c in content:
                counter = counter + 1
                if counter is 1:
                    continue
                container.append(c)
            ress = results[currentContainer]
            ress[content[0]] = container
            results[currentContainer] = ress
        f.close()
        # print "done reading"
        # self.readCurves()

        # "Create Nodes"
        nodes = {}
        node_ids = set()
        # Add Coordinates
        node_id = 0  # We assume that the node id increases incrementally
        ress = results["[COORDINATES]"]
        for c in ress:
            node_id += 1
            coords = ress[c]
            node = self.nodes_container.create_feature()

            # Create geometry
            n_pt = ogr.Geometry(ogr.wkbPoint)
            x1 = float(coords[0])
            y1 = float(coords[1])

            n_pt.SetPoint_2D(0, x1, y1)

            # Set geometry in feature
            node.SetGeometry(n_pt)

            nodes[c] = (node_id, x1, y1)
            node_ids.add(c)

            if self.name_outlet == c:
                outfall = self.outlet.create_feature()
                outfall.SetGeometry(n_pt)
                outfall.SetField("node_id", node_id)
        self.nodes_container.finalise()
        if self.name_outlet != "":
            self.outlet.finalise()



        # #Add Nodes
        # junctions = results["[JUNCTIONS]"]
        # for c in junctions:
        #     attributes = junctions[c]
        #     juntion = nodes[c]
        #     sewer.addComponentToView(juntion, self.junctions)
        #
        #     juntion.addAttribute("SWMM_ID", str(c))
        #     juntion.addAttribute("invert_elevation", (float(attributes[0])))
        #     juntion.addAttribute("D", (float(attributes[1])))
        #     juntion.addAttribute("Z", (float(attributes[0])) + (float(attributes[1])))
        #     juntion.addAttribute("built_year", self.defaultBuiltYear)
        #     if (c == self.NameWWTP):
        #         print "wwtp found"
        #         sewer.addComponentToView(juntion, self.wwtps)
        #
        # #Write Outfalls
        # outfalls = results["[OUTFALLS]"]
        # for o in outfalls:
        #     vals = outfalls[o]
        #     attributes = outfalls[o]
        #     outfall = nodes[o]
        #     sewer.addComponentToView(outfall, self.outfalls)
        #     outfall.addAttribute("Z", float(vals[0]))
        #     if (o == self.NameWWTP):
        #         print "wwtp found"
        #         sewer.addComponentToView(outfall, self.wwtps)
        #         outfall.addAttribute("WWTP", 1.0)
        # #Write Storage Units
        # if "[STORAGE]" in results:
        #     storages = results["[STORAGE]"]
        #     for s in storages:
        #         vals = storages[s]
        #         storage = nodes[s]
        #         sewer.addComponentToView(storage, self.storages)
        #         storage.addAttribute("Z", float(vals[0]))
        #         storage.addAttribute("max_depth", float(vals[1]))
        #         storage.addAttribute("type", vals[3])
        #         if  vals[3] == "TABULAR":
        #             curve = self.curves[vals[4]]
        #             storage_x = doublevector()
        #             storage_y = doublevector()
        #             for c in curve:
        #                 storage_x.append(c[0])
        #                 storage_y.append(c[1])
        #             storage.getAttribute("storage_x").setDoubleVector(storage_x)
        #             storage.getAttribute("storage_y").setDoubleVector(storage_y)
        #
        #
        #
        # if "[XSECTIONS]" in results:
        #     xsections = results["[XSECTIONS]"]
        #
        ress = results["[CONDUITS]"]
        counter = 0
        for c in ress:
            counter += 1
            vals = ress[c]
            end_id = nodes[vals[0]]
            start_id = nodes[vals[1]]

            # if end_id not in node_ids:
            #     continue
            # if start_id not in node_ids:
            #     continue

            conduit = self.conduits.create_feature()
            line = ogr.Geometry(ogr.wkbLineString)
            # print start_id
            # print nodes[start_id][1], nodes[start_id][2]
            line.SetPoint_2D(0, nodes[vals[0]][1], nodes[vals[0]][2])
            line.SetPoint_2D(1, nodes[vals[1]][1], nodes[vals[1]][2])

            conduit.SetGeometry(line)

            # Create XSection
            conduit.SetField("start_id", nodes[vals[0]][0])
            conduit.SetField("end_id", nodes[vals[1]][0])
            # conduit.SetField("inlet_offset", float(vals[4]))
            # conduit.SetField("outlet_offset", float(vals[5]))
            # e.addAttribute("built_year", self.defaultBuiltYear)
            # if c in xsections:
            #     e.addAttribute("Diameter", float(xsections[c][1]))
            #     xsection = self.createXSection(sewer, xsections[c])
            #     e.getAttribute("XSECTION").addLink(xsection, "XSECTION")
        self.conduits.finalise()
        if "[WEIRS]" in results:
            c_weirs = results["[WEIRS]"]
            for c in c_weirs:
                vals = c_weirs[c]
                end_id = nodes[vals[0]]
                start_id = nodes[vals[1]]

                # if end_id not in node_ids:
                #     continue
                # if start_id not in node_ids:
                #     continue

                weir = self.weirs.create_feature()
                line = ogr.Geometry(ogr.wkbLineString)
                # print start_id
                # print nodes[start_id][1], nodes[start_id][2]
                line.SetPoint_2D(0, nodes[vals[0]][1], nodes[vals[0]][2])
                line.SetPoint_2D(1, nodes[vals[1]][1], nodes[vals[1]][2])

                weir.SetGeometry(line)

                # Create XSection
                weir.SetField("start_id", nodes[vals[0]][0])
                weir.SetField("end_id", nodes[vals[1]][0])
        self.weirs.finalise()
                # vals = c_weirs[c]
                # start = nodes[vals[0]]
                # end = nodes[vals[1]]
                # e = sewer.addEdge(start, end, self.weirs)
                #
                # e.addAttribute("type",vals[2] )
                # e.addAttribute("crest_height",float(vals[3]))
                # e.addAttribute("discharge_coefficient",float(vals[4]))
                # e.addAttribute("end_coefficient",float(vals[7]))
                # #Create XSection
                # e.addAttribute("Diameter", float(xsections[c][1]))
                #
                # xsection = self.createXSection(sewer, xsections[c])
                # e.getAttribute("XSECTION").addLink(xsection, "XSECTION")

        if "[PUMPS]" in results:
            c_pumps = results["[PUMPS]"]
            for c in c_pumps:
                vals = c_pumps[c]
                end_id = nodes[vals[0]]
                start_id = nodes[vals[1]]

                # if end_id not in node_ids:
                #     continue
                # if start_id not in node_ids:
                #     continue

                pump = self.pumps.create_feature()
                line = ogr.Geometry(ogr.wkbLineString)
                # print start_id
                # print nodes[start_id][1], nodes[start_id][2]
                line.SetPoint_2D(0, nodes[vals[0]][1], nodes[vals[0]][2])
                line.SetPoint_2D(1, nodes[vals[1]][1], nodes[vals[1]][2])

                pump.SetGeometry(line)

                # Create XSection
                pump.SetField("start_id", nodes[vals[0]][0])
                pump.SetField("end_id", nodes[vals[1]][0])
        self.pumps.finalise()
        #         vals = c_pumps[c]
        #         start = nodes[vals[0]]
        #         end = nodes[vals[1]]
        #         e = sewer.addEdge(start, end, self.pumps)
        #
        #         e.addAttribute("type", self.curves_types[vals[2]] )
        #
        #         curve = self.curves[vals[2]]
        #         pump_x = doublevector()
        #         pump_y = doublevector()
        #         for c in curve:
        #             pump_x.append(c[0])
        #             pump_y.append(c[1])
        #
        #         e.getAttribute("pump_x").setDoubleVector(pump_x)
        #         e.getAttribute("pump_y").setDoubleVector(pump_y)
        #
        # except Exception, e:
        #     print e
        #     print sys.exc_info()

        # self.nodes_container.finalise()


        # def createXSection(self, sewer, attributes):
        #     c_xsection = Component()
        #     xsection = sewer.addComponent(c_xsection, self.xsections)
        #     xsection.addAttribute("type", str(attributes[0]))
        #     diameters = doublevector()
        #     diameters.push_back(float(attributes[1]))
        #     #print self.curves
        #     if str(attributes[0]) != "CUSTOM":
        #         diameters.push_back(float(attributes[2]))
        #         diameters.push_back(float(attributes[3]))
        #         diameters.push_back(float(attributes[4]))
        #     else:
        #         shape_x = doublevector()
        #         shape_y = doublevector()
        #         #print attributes
        #         cv = self.curves[attributes[2]]
        #
        #         #xsection.getAttribute("shape_type").setString(vd)
        #         for c in cv:
        #             shape_x.append(c[0])
        #             shape_y.append(c[1])
        #         xsection.getAttribute("shape_x").setDoubleVector(shape_x)
        #         xsection.getAttribute("shape_y").setDoubleVector(shape_y)
        #         xsection.getAttribute("shape_type").setString(self.curves_types[attributes[2]])
        #
        #     xsection.getAttribute("diameters").setDoubleVector(diameters)
        #
        #     return xsection
