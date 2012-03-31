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


from pydynamind import *
class ImportSWMM(Module):
    def __init__(self):
        Module.__init__(self)
        self.conduits = View("CONDUIT", EDGE, WRITE)
        self.junctions = View("JUNCTION", NODE, WRITE)
        self.inlets = View("INLET", NODE, WRITE)
        self.wwtps = View("WWTP", NODE, WRITE)
        
        views = []
        views.append(self.conduits)
        views.append(self.junctions)
        views.append(self.inlets)
        views.append(self.wwtps)
        
        self.addData("Sewer", views)        
        self.createParameter("filename", STRING, "Name of SWMM File")
        self.filename = "/home/christian/Documents/DynaMind/data/networks/SWMM_innsbruck.inp"
        self.createParameter("NameWWTP", STRING, "Identifier WWTP")
        self.NameWWTP = "MD020"
        
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
            
            
            "Create Nodes"
            UUIDTranslator = {}

            #Add Coordinates
            ress = results["[COORDINATES]"]
            for c in ress:
                coords = ress[c]
                n = sewer.addNode(float(coords[0]), float(coords[1]),0.)
                UUIDTranslator[c] = n.getName()

            #Add Nodes

            ress = results["[JUNCTIONS]"]
            for c in ress:
                coords = ress[c]
                n = sewer.getNode(UUIDTranslator[c])
                sewer.addComponentToView(n, self.junctions) 
                if (c == self.NameWWTP):  
                    print "wwtp found"
                    sewer.addComponentToView(n, self.wwtps)
                    
            xsections = results["[XSECTIONS]"]               
            ress = results["[CONDUITS]"]
            for c in ress:
                vals = ress[c]
                start = sewer.getNode(UUIDTranslator[vals[0]])
                end = sewer.getNode(UUIDTranslator[vals[1]])
                e = sewer.addEdge(start, end, self.conduits)
                e.addAttribute("DIAMETER", float(xsections[c][1]))
                
        except Exception, e:
            print e
            print "Unexpected error:"

                