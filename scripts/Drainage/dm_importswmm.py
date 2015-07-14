# -*- coding: utf-8 -*- 

"""
@file
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2012  Christian Urich

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
class ImportSWMM(Module):
    def __init__(self):
        Module.__init__(self)
        self.conduits = View("CONDUIT", EDGE, WRITE)
        self.conduits.addAttribute("Diameter", DOUBLE, WRITE)
        self.conduits.addAttribute("built_year", DOUBLE, WRITE)
        
        self.xsections = View("XSECTION",COMPONENT,WRITE)     
        self.xsections.addAttribute("type", STRING, WRITE)
        self.xsections.addAttribute("shape", STRING, WRITE)
        
        
        self.conduits.addLinkAttribute("XSECTION", "XSECTION", WRITE)
        
        self.junctions = View("JUNCTION", NODE, WRITE)
        self.junctions.addAttribute("Z", DOUBLE, WRITE)
        self.junctions.addAttribute("D", DOUBLE, WRITE)
        self.junctions.addAttribute("invert_elevation", DOUBLE, WRITE)
        self.junctions.addAttribute("built_year", DOUBLE, WRITE)

        self.outfalls = View("OUTFALL", NODE, WRITE)
        self.outfalls.addAttribute("Z", DOUBLE, WRITE)
        #Not imported
        #self.inlets = View("INLET", NODE, WRITE)

        
        self.wwtps = View("WWTP", NODE, WRITE)
        
        self.storages = View("STORAGE", NODE, WRITE)
        self.storages.addAttribute("Z", DOUBLE, WRITE)
        self.storages.addAttribute("max_depth", DOUBLE, WRITE)
        self.storages.addAttribute("type", STRING, WRITE)
        self.storages.addAttribute("storage_x", DOUBLE, WRITE)
        self.storages.addAttribute("storage_y", DOUBLE, WRITE)

        self.weirs = View("WEIR", EDGE, WRITE)
        self.weirs.addAttribute("type", STRING, WRITE)
        self.weirs.addAttribute("crest_height", DOUBLE, WRITE)
        self.weirs.addAttribute("discharge_coefficient", DOUBLE, WRITE)
        self.weirs.addAttribute("end_coefficient", DOUBLE, WRITE)
        
        self.pumps = View("PUMPS", EDGE, WRITE)
        self.pumps.addAttribute("type", STRING, WRITE)
        self.pumps.addAttribute("pump_x", DOUBLE, WRITE)
        self.pumps.addAttribute("pump_y", DOUBLE, WRITE)
        
        views = []
        views.append(self.conduits)
        views.append(self.outfalls)
        views.append(self.junctions)
        #views.append(self.inlets)
        views.append(self.wwtps)
        views.append(self.storages)
        views.append(self.weirs)
        views.append(self.xsections)
        views.append(self.pumps)
        
        self.addData("Sewer", views)        
        self.createParameter("filename", FILENAME, "Name of SWMM File")
        self.filename = ""
        self.createParameter("NameWWTP", STRING, "Identifier WWTP")
        self.NameWWTP = "MD020"

        self.createParameter("defaultBuiltYear", INT, "Default_Built_Year")
        self.defaultBuiltYear = 1900
        
        self.curves = {}
        self.curves_types = {}      
        
        
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
                    #print line 
                    content = line.split()
                    if content[0] not in self.curves:
                        self.curves[ content[0] ] = []
                    values = self.curves[ content[0] ]
                    if (len(content) == 4):
                        values.append((float(content[2]), float(content[3])))
                    if (len(content) == 3):
                        values.append((float(content[1]), float(content[2])))
                    self.curves[ content[0] ] = values
                    
                    if (len(content) == 4):
                        if content[1] != "":
                            self.curves_types[content[0]] = str(content[1])
                        
                if line == "[CURVES]":
                    startReading = True
            f.close()

        except Exception, e:
            print e
            print sys.exc_info()            
                
    def run(self):        
        try:
            sewer = self.getData("Sewer")
            results = {}
            f = open(self.filename) 
            currentContainer = ""        
            for line in f:
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
                #First Section is always the Name
                content = line.split()
                container = []
                counter = 0
                if len(content) < 2:
                    continue
                for c in content:
                    counter = counter+1
                    if counter is 1:
                        continue
                    container.append(c)
                ress = results[currentContainer]
                ress[content[0]] = container
                results[currentContainer] = ress
            f.close()
            self.readCurves()
            
            "Create Nodes"
            nodes = {}
            
            #Add Coordinates
            ress = results["[COORDINATES]"]
            for c in ress:
                coords = ress[c]
                nodes[c] = sewer.addNode(float(coords[0]), float(coords[1]),0.)


            #Add Nodes
            junctions = results["[JUNCTIONS]"]
            for c in junctions:
                attributes = junctions[c]
                juntion = nodes[c]
                sewer.addComponentToView(juntion, self.junctions) 
                
                juntion.addAttribute("SWMM_ID", str(c))
                juntion.addAttribute("invert_elevation", (float(attributes[0])))
                juntion.addAttribute("D", (float(attributes[1])))
                juntion.addAttribute("Z", (float(attributes[0])) + (float(attributes[1])))
                juntion.addAttribute("built_year", self.defaultBuiltYear)
                if (c == self.NameWWTP):  
                    print "wwtp found"
                    sewer.addComponentToView(juntion, self.wwtps)                    
            #Write Outfalls
            outfalls = results["[OUTFALLS]"]
            for o in outfalls:
                vals = outfalls[o]
                attributes = outfalls[o]
                outfall = nodes[o]
                sewer.addComponentToView(outfall, self.outfalls) 
                outfall.addAttribute("Z", float(vals[0]))
                if (o == self.NameWWTP):  
                    print "wwtp found"
                    sewer.addComponentToView(outfall, self.wwtps)    
                    outfall.addAttribute("WWTP", 1.0)
            #Write Storage Units
            if "[STORAGE]" in results:
                storages = results["[STORAGE]"]
                for s in storages:
                    vals = storages[s]
                    storage = nodes[s]
                    sewer.addComponentToView(storage, self.storages) 
                    storage.addAttribute("Z", float(vals[0]))
                    storage.addAttribute("max_depth", float(vals[1]))
                    storage.addAttribute("type", vals[3])
                    if  vals[3] == "TABULAR":
                        curve = self.curves[vals[4]]
                        storage_x = doublevector()
                        storage_y = doublevector()
                        for c in curve:
                            storage_x.append(c[0])
                            storage_y.append(c[1])
                        storage.getAttribute("storage_x").setDoubleVector(storage_x)
                        storage.getAttribute("storage_y").setDoubleVector(storage_y)
                    
                
                
            if "[XSECTIONS]" in results:
                xsections = results["[XSECTIONS]"]     

            ress = results["[CONDUITS]"]            
            for c in ress:
                vals = ress[c]
                end = nodes[vals[0]]
                start = nodes[vals[1]]
                e = sewer.addEdge(start, end, self.conduits)
                e.addAttribute("SWMM_ID", str(c))
                #Create XSection

                e.addAttribute("inlet_offset", float(vals[4]))
                e.addAttribute("outlet_offset", float(vals[5]))
                e.addAttribute("built_year", self.defaultBuiltYear)
                if c in xsections:
                    e.addAttribute("Diameter", float(xsections[c][1]))
                    xsection = self.createXSection(sewer, xsections[c])
                    e.getAttribute("XSECTION").addLink(xsection, "XSECTION")
                
            if "[WEIRS]" in results:
                c_weirs = results["[WEIRS]"]     
                for c in c_weirs:
                    vals = c_weirs[c]
                    start = nodes[vals[0]]
                    end = nodes[vals[1]]
                    e = sewer.addEdge(start, end, self.weirs)
                    
                    e.addAttribute("type",vals[2] )
                    e.addAttribute("crest_height",float(vals[3]))
                    e.addAttribute("discharge_coefficient",float(vals[4]))
                    e.addAttribute("end_coefficient",float(vals[7]))
                    #Create XSection
                    e.addAttribute("Diameter", float(xsections[c][1]))
                    
                    xsection = self.createXSection(sewer, xsections[c])
                    e.getAttribute("XSECTION").addLink(xsection, "XSECTION")                
                
            if "[PUMPS]" in results:
                c_pumps = results["[PUMPS]"]     
                for c in c_pumps:
                    vals = c_pumps[c]
                    start = nodes[vals[0]]
                    end = nodes[vals[1]]
                    e = sewer.addEdge(start, end, self.pumps)
                    
                    e.addAttribute("type", self.curves_types[vals[2]] )                
                    
                    curve = self.curves[vals[2]]
                    pump_x = doublevector()
                    pump_y = doublevector()
                    for c in curve:
                        pump_x.append(c[0])
                        pump_y.append(c[1])  
                    
                    e.getAttribute("pump_x").setDoubleVector(pump_x)
                    e.getAttribute("pump_y").setDoubleVector(pump_y)
                
        except Exception, e:
            print e
            print sys.exc_info()
            
    def createXSection(self, sewer, attributes):
        c_xsection = Component()
        xsection = sewer.addComponent(c_xsection, self.xsections)
        xsection.addAttribute("type", str(attributes[0]))   
        diameters = doublevector()
        diameters.push_back(float(attributes[1]))
        #print self.curves
        if str(attributes[0]) != "CUSTOM":
            diameters.push_back(float(attributes[2]))
            diameters.push_back(float(attributes[3]))
            diameters.push_back(float(attributes[4]))
        else:
            shape_x = doublevector()
            shape_y = doublevector()
            #print attributes
            cv = self.curves[attributes[2]]
            
            #xsection.getAttribute("shape_type").setString(vd)
            for c in cv: 
                shape_x.append(c[0])
                shape_y.append(c[1])
            xsection.getAttribute("shape_x").setDoubleVector(shape_x)
            xsection.getAttribute("shape_y").setDoubleVector(shape_y)
            xsection.getAttribute("shape_type").setString(self.curves_types[attributes[2]])
            
        xsection.getAttribute("diameters").setDoubleVector(diameters)
        
        return xsection
        
        

                
