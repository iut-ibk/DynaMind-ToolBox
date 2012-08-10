# -*- coding: utf-8 -*- 

"""
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2011-2012  Christian Urich

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
from pydmtoolbox import *
from osgeo import ogr, osr
from shapely import wkb, geometry, coords
import itertools
from shapely.geometry import asShape
import sys

class ImportShapeFile(Module):
        def __init__(self):            
            Module.__init__(self)            
            self.createParameter("FileName", FILENAME, "filename")
            self.FileName = ""
            self.createParameter("Identifier", STRING, "Name")
            self.createParameter("Snap", DOUBLE, "Snap")
            
            self.Identifier = ""
            self.IdentifierOld = ""
            self.createParameter("AppendToExistingSystem", BOOL, "If ShapeFile is appended to existing System")
            self.createParameter("offsetX", DOUBLE, "OffsetX")
            self.createParameter("offsetY", DOUBLE, "OffsetY")
            self.createParameter("isPoint", BOOL, "ShapeFile is Points")
            self.createParameter("isEdge", BOOL, "ShapeFile is Edges")
            self.createParameter("isFace", BOOL, "ShapeFile is Faces")
            
            self.AppendToExistingSystem = False
            self.isPoint = False
            self.isEdge = False
            self.isFace = False      
            self.PointMap = {}
            self.PointmapTol = 1.
            self.Snap = 0.01
            self.offsetX = 0
            self.OffsetY = 0
            
    
        def init(self):
            if self.Identifier == "":
                return
            self.NodeView = View("", NODE, WRITE)
            self.EdgeView = View("", EDGE, WRITE)
            self.FaceView = View("", FACE, WRITE)
            if self.isPoint:
                self.NodeView = View(self.Identifier, NODE, WRITE)
            if self.isEdge:    
                self.EdgeView = View(self.Identifier, EDGE, WRITE)
            if self.isFace:
                self.FaceView = View(self.Identifier, FACE, WRITE)
                self.FaceView.addAttribute("MinX")
                self.FaceView.addAttribute("MaxX")
                self.FaceView.addAttribute("MinY")
                self.FaceView.addAttribute("MaxY")            
            data = []

            shapelyGeometries, fieldPacks, fieldDefinitions = [], [], []
            sourcePath = self.FileName
            dataSource = ogr.Open(sourcePath)
            layer = dataSource.GetLayer()
                      
            featureDefinition = layer.GetLayerDefn()
            fieldIndices = xrange(featureDefinition.GetFieldCount())
            for fieldIndex in fieldIndices:
                    fieldDefinition = featureDefinition.GetFieldDefn(fieldIndex)
                    fieldDefinitions.append((fieldDefinition.GetName(), fieldDefinition.GetType()))            
            for j in range(len(fieldDefinitions)):                           
                    attributename = str(fieldDefinitions[j][0])
                    self.NodeView.addAttribute(attributename)
                    self.EdgeView.addAttribute(attributename)
                    self.FaceView.addAttribute(attributename)
            if self.isPoint:
                data.append(self.NodeView)
            if self.isEdge:    
                data.append(self.EdgeView)
            if self.isFace:
                data.append(self.FaceView)  
		print self.FaceView
            if self.AppendToExistingSystem:
                dummy = View("dummy", SUBSYSTEM, MODIFY)
                data.append(dummy)   
                       
            self.addData("Vec", data)
            
            self.MinX = 0.0
            self.MaxX = 0.0
            self.MinY = 0.0
            self.MaxY = 0.0
            
            self.firstPoint = True
            
            #self.IdentifierOld = self.Identifier
        
        """The method compares the existing nodes with the new Node.
        If a node already exists within the error distance this node is
        returned otherwise a new node is created and returend"""
        def addPoint2d(self, city, x, y, view, error):
            newNode = Node(x,y,0)
            if (self.firstPoint is True):
                self.MinX = x
                self.MaxX = x
                self.MinY = y
                self.MaxY = y  
                self.firstPoint = False
             
            if (self.MinX > x):
                self.MinX = x
                
            if (self.MinY > y):
                self.MinY = y
                
            if (self.MaxX < x):
                self.MaxX = x     
                           
            if (self.MaxY < y):
                self.MaxY = y               
            
            idx = ('%.0f') % (x/self.PointmapTol)
            idy = ('%.0f') % (y/self.PointmapTol)
            id = idx+idy
            existingPoints = []
            if id in self.PointMap:
                existingPoints = self.PointMap[id]
            else:
                self.PointMap[id] = existingPoints 
            createNewNode = True
            #check if Point already exists
            for n in existingPoints:
                if newNode.compare2d(n, error):
                    return n
            n = city.addNode(x,y,0,view)
            existingPoints.append(n)
            return n
        
        """Init PointMap that is used to check is a node already exists"""
        def initNodeList(self, city):
            nodes = city.getAllNodes()
            self.PointMap = {}
            for n in nodes:
                n1 = Node(nodes[n])
                x = n1.getX()
                y = n1.getY()
                idx = ('%.0f') % (x/self.PointmapTol)
                idy = ('%.0f') % (y/self.PointmapTol)
                id = idx+idy
                self.PointMap[id] = [] 
                self.PointMap[id].append(nodes[n])
                
        
        def run(self):            
            try:    
                self.firstPoint = True
                while int(self.Snap/self.PointmapTol) is not 0:
                   self.PointmapTol = self.PointmapTol*10                   
                       
                city = self.getData("Vec")
                if self.AppendToExistingSystem:
                    self.initNodeList(city)
                shapelyGeometries, fieldPacks, fieldDefinitions = [], [], []
                sourcePath = self.FileName
                dataSource = ogr.Open(sourcePath)
                layer = dataSource.GetLayer()
                featureDefinition = layer.GetLayerDefn()
                fieldIndices = xrange(featureDefinition.GetFieldCount())
                #print fieldIndices
                for fieldIndex in fieldIndices:
                        fieldDefinition = featureDefinition.GetFieldDefn(fieldIndex)
                        fieldDefinitions.append((fieldDefinition.GetName(), fieldDefinition.GetType()))
                #while there are more features,
                feature = layer.GetNextFeature()
                while feature:
                        shapelyGeometries.append(wkb.loads(feature.GetGeometryRef().ExportToWkb()))
                        fieldPacks.append([feature.GetField(x) for x in fieldIndices])
                        # Get the next feature
                        feature = layer.GetNextFeature() 
                counter = 1
                InlcudedEdges = {}
                for i in range(len(shapelyGeometries)):
                    shapelyGeometry = shapelyGeometries[i]
                    geoms = []
                    #print shapelyGeometry.type
                    if shapelyGeometry.type == 'Point':            
                            n = self.addPoint2d(city, shapelyGeometry.x+self.offsetX , shapelyGeometry.y+self.offsetY,  self.NodeView, self.Snap)       
                            city.addComponentToView(n, self.NodeView)
                            attrvec = []
                            for j in range(len(fieldDefinitions)):
                                attr = Attribute(fieldDefinitions[j][0])
                                attributename = str(fieldDefinitions[j][0])
                                if attributename == 'PLAN_DATE':
                                    stringvalue = str(fieldPacks[i][j])
                                    stringvec = stringvalue.split("/")                                            
                                    try:
                                        srtingval = stringvec[len(stringvec)-1]
                                        val = int(srtingval)
                                        if val < 100:
                                            val = 1900+val
                                        if val > 2020:
                                            val = 0
                                        if val < 1800:
                                            val = 0
                                        attr.setDouble(val)
                                    except ValueError:
                                            pass                                       
                                
                                elif type(fieldPacks[i][j]).__name__ == 'str':                        
                                        attr.setString(str(fieldPacks[i][j]))
                                elif type(fieldPacks[i][j]).__name__ == 'float' or type(fieldPacks[i][j]).__name__ == 'int': 
                                        attr.setDouble(fieldPacks[i][j])

                                attrvec.append(attr)
                            for attr in attrvec: 
                           	n.addAttribute(attr)     
              
                    elif shapelyGeometry.type == 'Polygon' or shapelyGeometry.type == 'LineString':
                            geoms.append(shapelyGeometry)
                    else:
                            geoms = list(shapelyGeometry.geoms)
                    #Add Lines
                    if shapelyGeometry.type != 'Point':            
                        for geo in geoms:
                            if shapelyGeometry.type != 'LineString' and shapelyGeometry.type != 'MultiLineString' :
                                r = geo.exterior
                                coordinates = list(r.coords)
                            else:
                                coordinates = geo.coords
                            numberOfPoints = -1
                            offset = -1
                            pl = nodevector()
                            el = edgevector()
                            attrvec = []
                            for j in range(len(fieldDefinitions)):                           
                                attr = Attribute(fieldDefinitions[j][0])
                                attributename = str(fieldDefinitions[j][0])
                                if attributename == 'PLAN_DATE':
                                    stringvalue = str(fieldPacks[i][j])
                                    stringvec = stringvalue.split("/")                                            
                                    try:
                                        srtingval = stringvec[len(stringvec)-1]
                                        val = int(srtingval)
                                        if val < 100:
                                            val = 1900+val
                                        if val > 2020:
                                            val = 0
                                        if val < 1800:
                                            val = 0
                                        attr.setDouble(val)
                                    except ValueError:
                                            pass                                       
                                
                                elif type(fieldPacks[i][j]).__name__ == 'str':                        
                                        attr.setString(str(fieldPacks[i][j]))
                                elif type(fieldPacks[i][j]).__name__ == 'float' or type(fieldPacks[i][j]).__name__ == 'int': 
                                        attr.setDouble(fieldPacks[i][j])
                                attrvec.append(attr)
                            for coords in coordinates:
                                n = self.addPoint2d(city, coords[0]+self.offsetX, coords[1]+self.offsetY, self.NodeView, self.Snap)
                                pl.append(n)
                                numberOfPoints += 1     
                                offset +=1

                                if numberOfPoints > 0:    
                                        if self.isEdge:
                                            if str(pl[numberOfPoints - offset].getUUID()) != str(pl[numberOfPoints].getUUID()):                                                 
                                                startnode =   pl[numberOfPoints - offset]
                                                endnode = pl[numberOfPoints]
                                                e = city.addEdge(pl[numberOfPoints - offset], pl[numberOfPoints],  self.EdgeView)
                                                el.append(e)
                                                InlcudedEdges[startnode.getUUID()] = endnode.getUUID()
                                                offset = 0
                                                for attr in attrvec:      
                                                    e.addAttribute(attr)
                            if self.isFace:
                                    f = city.addFace(pl, self.FaceView) 
                                    for attr in attrvec: 
                                        f.addAttribute(attr)
                                    f.addAttribute("MinX", self.MinX)
                                    f.addAttribute("MaxX", self.MaxX)
                                    f.addAttribute("MinY", self.MinY)
                                    f.addAttribute("MaxY", self.MaxY)
                                        
                                    self.firstPoint = True

                print "Imported points: " + str( len(city.getAllNodes()))
                print "Imported edges: " + str( len(city.getUUIDsOfComponentsInView( self.EdgeView)) )
                print "Imported faces: " + str( len(city.getUUIDsOfComponentsInView( self.FaceView)) )
                print "Edges Created: " + str( counter )
		#clear some stuff
		self.PointMap = {}

            except Exception, e:
                print e
		print sys.exc_info()
                print "Unexpected error:"
            
            

            
