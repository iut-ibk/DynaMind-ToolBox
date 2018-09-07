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
import gdal, osr
import paramiko

class DM_ImportSWMM(Module):


    display_name = "Import SWMM File"
    group_name = "Performance Assessment"

    def getHelpUrl(self):
        return "/DynaMind-GDALModules/dm_importswmm.html"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("filename", STRING, "Name of SWMM File")
        self.filename = ""

        self.createParameter("name_outlet", STRING, "Identifier Outlet")
        self.name_outlet = ""

        self.createParameter("epsg_from", INT, "EPSG Coce")
        self.epsg_from = -1

        self.createParameter("username", STRING)
        self.username = ""

        self.createParameter("password", STRING)
        self.password = ""

        self.createParameter("port", INT)
        self.port = 22

        self.createParameter("host", STRING)
        self.host = ""

        self.transport = None
        self.sftp = None

        self.downloaded_file = ""

        self.real_file_name = ""

    def generate_downloaded_file_name(self):
              return self.filename + self.username + self.password+self.host

    def connect(self):
            established = False
            try:
                log(str(self.host) + " " + str(self.port) + " " + str(self.username) + " " + str(self.password), Standard)
                self.transport = paramiko.Transport((self.host, self.port))

                self.transport.connect(username=self.username, password=self.password)
                established = True
            except:
                return False

            log("connected", Standard)
            self.sftp = paramiko.SFTPClient.from_transport(self.transport)
            return True


    def close(self):
            log("close connection", Standard)
            self.sftp.close()
            self.transport.close()

            self.sftp = None
            self.transport = None

    def get_file(self, file_name):
            if self.real_file_name:
                os.remove(self.real_file_name)
            self.real_file_name = "/tmp/" + str(uuid.uuid4())
            try:
                self.sftp.get(file_name,  self.real_file_name)
            except Exception as e:
                print e
                self.real_file_name = ""
                return False
            self.downloaded_file = self.generate_downloaded_file_name()
            return True


    def init(self):

        self.conduits = ViewContainer("conduit", EDGE, WRITE)
        self.conduits.addAttribute("start_id", Attribute.INT, WRITE)
        self.conduits.addAttribute("end_id", Attribute.INT, WRITE)

        self.conduits.addAttribute("inlet_offset", Attribute.DOUBLE, WRITE)
        self.conduits.addAttribute("outlet_offset", Attribute.DOUBLE, WRITE)
        self.conduits.addAttribute("diameter", Attribute.DOUBLE, WRITE)
        self.conduits.addAttribute("width", Attribute.DOUBLE, WRITE)
        self.conduits.addAttribute("type", Attribute.STRING, WRITE)
        self.conduits.addAttribute("cscol1", Attribute.DOUBLEVECTOR, WRITE)
        self.conduits.addAttribute("cscol2", Attribute.DOUBLEVECTOR, WRITE)
        self.conduits.addAttribute("cscol3", Attribute.DOUBLEVECTOR, WRITE)

        self.junctions = ViewContainer("junction", NODE, WRITE)
        self.junctions.addAttribute("node_id", Attribute.INT, WRITE)
        self.junctions.addAttribute("swmm_id", Attribute.STRING, WRITE)

        self.junctions.addAttribute("d", Attribute.DOUBLE, WRITE)
        self.junctions.addAttribute("invert_elevation", Attribute.DOUBLE, WRITE)


        self.pumps = ViewContainer("pump", EDGE, WRITE)
        self.pumps.addAttribute("start_id", Attribute.INT, WRITE)
        self.pumps.addAttribute("end_id", Attribute.INT, WRITE)

        self.weirs = ViewContainer("weir", EDGE, WRITE)
        self.weirs.addAttribute("start_id", Attribute.INT, WRITE)
        self.weirs.addAttribute("end_id", Attribute.INT, WRITE)
        self.weirs.addAttribute("crest_height", Attribute.DOUBLE, WRITE)
        self.weirs.addAttribute("discharge_coefficient", Attribute.DOUBLE, WRITE)
        self.weirs.addAttribute("end_coefficient", Attribute.DOUBLE, WRITE)
        self.outfall = ViewContainer("outfall", NODE, WRITE)
        self.outfall.addAttribute("node_id", Attribute.INT, WRITE)
        self.outfall.addAttribute("invert_elevation", Attribute.DOUBLE, WRITE)

        self.dummy = ViewContainer("dummy", SUBSYSTEM, MODIFY)

        # self.xsections = ViewContainer("xsection",COMPONENT,WRITE)
        # self.xsections.addAttribute("type", STRING, WRITE)
        # self.xsections.addAttribute("shape", STRING, WRITE)
        self.nodes_container = ViewContainer("node", NODE, WRITE)

        views = [self.nodes_container,
                 self.junctions,
                 self.conduits,
                 self.pumps,
                 self.weirs,
                 self.outfall,
                 self.dummy]

        if self.name_outlet != "":
            self.outlet = ViewContainer("outlet", NODE, WRITE)
            self.outlet.addAttribute("node_id", Attribute.INT, WRITE)
            views.append(self.outlet)
        self.registerViewContainers(views)

    def run(self):
        if self.host:
            if not self.connect():
                log("Connection to host failed", Error)
                return
            if not self.get_file(self.filename):
                log("Failed to download file " + str(self.filename), Error)
                return
        else:
            self.real_file_name = self.filename

        results = {}
        f = open(self.real_file_name)
        currentContainer = ""
        # this is needed to load transects properly
        gr = []
        transect_id = None
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

            if currentContainer == "[TRANSECTS]":
                # print content
                if content[0] == "X1":

                    if transect_id:
                        ress = results[currentContainer]
                        ress[transect_id] = [gr[0:][::2], gr[1:][::2]]
                        results[currentContainer] = ress
                    gr = []
                    transect_id = content[1]
                    continue

                if content[0] == "GR":
                    for c in content[1:]:
                        gr.append(float(c))
                continue
            else:
                if transect_id:
                    print "write me", transect_id
                    ress = results["[TRANSECTS]"]
                    ress[transect_id] = [gr[0:][::2], gr[1:][::2]]
                    results["[TRANSECTS]"] = ress
                    gr = []
                    transect_id = None

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

        target_srs = osr.SpatialReference()
        target_srs.ImportFromEPSG(self.getSimulationConfig().getCoorindateSystem())

        source_srs = osr.SpatialReference()
        if self.epsg_from > -1:
            source_srs.ImportFromEPSG(self.epsg_from)
        else:
            source_srs.ImportFromEPSG(self.getSimulationConfig().getCoorindateSystem())
        # (353136,5776456)
        ct = osr.CoordinateTransformation(source_srs, target_srs)

        for c in ress:
            node_id += 1
            coords = ress[c]
            node = self.nodes_container.create_feature()

            # Create geometry

            n_pt = ogr.Geometry(ogr.wkbPoint)
            #n_pt.Transform(ct)

            x1 = float(coords[0])
            y1 = float(coords[1])

            n_pt.SetPoint_2D(0, x1, y1)
            n_pt.Transform(ct)
             
            # Set geometry in feature
            
            n_tc = ogr.Geometry(ogr.wkbPoint)
            n_tc.SetPoint_2D(0, n_pt.GetX(), n_pt.GetY())
            node.SetGeometry(n_tc)

            nodes[c] = (node_id, n_pt.GetX(), n_pt.GetY())
            node_ids.add(c)

            if self.name_outlet == c:
                outfall = self.outlet.create_feature()
                outfall.SetGeometry(n_pt)
                outfall.SetField("node_id", node_id)

        self.nodes_container.finalise()
        if self.name_outlet != "":
            self.outlet.finalise()



        # #Add Nodes
        junctions = results["[JUNCTIONS]"]
        for c in junctions:
            attributes = junctions[c]
            #juntion = nodes[c]
            juntion = self.junctions.create_feature()

            n_pt = ogr.Geometry(ogr.wkbPoint)

            n_pt.SetPoint_2D(0, nodes[c][1], nodes[c][2])
            #n_pt.Transform(ct) 
            juntion.SetGeometry(n_pt)
            juntion.SetField("node_id", nodes[c][0])

            juntion.SetField("invert_elevation", (float(attributes[0])))
            juntion.SetField("d", (float(attributes[1])))
            juntion.SetField("swmm_id", str(c))
        self.junctions.finalise()
            # juntion.addAttribute("Z", (float(attributes[0])) + (float(attributes[1])))
            # juntion.addAttribute("built_year", self.defaultBuiltYear)
            # juntion.addAttribute("SWMM_ID", str(c))
            # if (c == self.NameWWTP):
            #     print "wwtp found"
            #     sewer.addComponentToView(juntion, self.wwtps)

        #
        # #Write Outfalls
        outfalls = results["[OUTFALLS]"]
        for o in outfalls:
            vals = outfalls[o]
            attributes = outfalls[o]
            # outfall = nodes[o]
            outfall = self.outfall.create_feature()

            n_pt = ogr.Geometry(ogr.wkbPoint)

            n_pt.SetPoint_2D(0, nodes[o][1], nodes[o][2])
            outfall.SetGeometry(n_pt)
            outfall.SetField("node_id", nodes[o][0])
            outfall.SetField("invert_elevation", (float(attributes[0])))
            # sewer.addComponentToView(outfall, self.outfalls)
            # outfall.addAttribute("Z", float(vals[0]))
            # if (o == self.NameWWTP):
            #     print "wwtp found"
            #     sewer.addComponentToView(outfall, self.wwtps)
            #     outfall.addAttribute("WWTP", 1.0)

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
        if "[XSECTIONS]" in results:
            xsections = results["[XSECTIONS]"]

        ress = results["[CONDUITS]"]
        counter = 0
        for c in ress:
            counter += 1
            vals = ress[c]

            # if end_id not in node_ids:
            #     continue
            # if start_id not in node_ids:
            #     continue

            conduit = self.conduits.create_feature()
            line = ogr.Geometry(ogr.wkbLineString)

            line.SetPoint_2D(0, nodes[vals[0]][1], nodes[vals[0]][2])
            line.SetPoint_2D(1, nodes[vals[1]][1], nodes[vals[1]][2])

            conduit.SetGeometry(line)

            # Create XSection
            conduit.SetField("start_id", nodes[vals[0]][0])
            conduit.SetField("end_id", nodes[vals[1]][0])
            conduit.SetField("inlet_offset", float(vals[4]))
            conduit.SetField("outlet_offset", float(vals[5]))

            # log(str(vals), DM.Standard)
            # e.addAttribute("built_year", self.defaultBuiltYear)
            if c in xsections:
                section_type = str(xsections[c][0])
                conduit.SetField("type", section_type)
                if section_type == "IRREGULAR":
                    # print results["[TRANSECTS]"][xsections[c][1]]
                    # print results["[TRANSECTS]"][xsections[c][1]][0]
                    # print results["[TRANSECTS]"][xsections[c][1]][1]
                    dm_set_double_list(conduit, "cscol1", results["[TRANSECTS]"][xsections[c][1]][0])
                    dm_set_double_list(conduit, "cscol2", results["[TRANSECTS]"][xsections[c][1]][1])
                    # self.conduits.addAttribute("cscol1", Attribute.DOUBLEVECTOR, WRITE)
                    # self.conduits.addAttribute("cscol2", Attribute.DOUBLEVECTOR, WRITE)
                else:
                    conduit.SetField("diameter", float(xsections[c][1]))
                    conduit.SetField("width", float(xsections[c][2]))

                # xsection = self.createXSection(sewer, xsections[c])
                # e.getAttribute("XSECTION").addLink(xsection, "XSECTION")
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
