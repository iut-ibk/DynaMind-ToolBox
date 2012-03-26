"""
@file
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

import osgeo.ogr, osgeo.osr
import os
from pydynamind import *
from pydmtoolbox import *

class ExportToShapeFile(Module):
            def __init__(self):
                Module.__init__(self)
 
                #self.addParameter("Name", STRING, self.FileName)
                self.createParameter("FileName", STRING,  "test")
                self.FileName = "Shapefile"
                self.createParameter("Name", STRING,  "test")
                self.Name = ""    
                self.createParameter("Points", BOOL,  "test")
                self.Points = False
                self.createParameter("Lines", BOOL,  "test")
                self.Lines = True
                self.createParameter("Faces", BOOL,  "test")
                self.Faces = False
                self.vec = View("dummy", SUBSYSTEM, READ)
                views = []
                views.append(self.vec)
                self.addData("City", views)
                
            def run(self):
                if self.Faces:                       
                    self.exportFaces()
                if self.Lines:
                    self.exportPolyline()       
                if self.Points:
                    self.exportPoints()         

            def exportFaces(self):
		city = self.getData("City")
                spatialReference = osgeo.osr.SpatialReference()
                spatialReference.ImportFromProj4('+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs')
                
                #Init Shape Files
                driver = osgeo.ogr.GetDriverByName('ESRI Shapefile')
                if os.path.exists(str(self.FileName+'_faces.shp')): os.remove(self.FileName+'_faces.shp')
                shapeData = driver.CreateDataSource(self.FileName+'_faces.shp')
                
                layer = shapeData.CreateLayer('layer1', spatialReference, osgeo.ogr.wkbPolygon)
                layerDefinition = layer.GetLayerDefn()               
                AttributeList = []
                attr = []
                hasAttribute = False
                #Get Data 

                 
                names = city.getNamesOfComponentsInView(View(self.Name, READ, FACE))
                for i in range(len(names)): 
                    attributemap = city.getComponent(names[i]).getAllAttributes()
                    for key in attributemap.keys():
			if (key in attr) == False:
                        	attr.append(key)
                for j in range(len(attr)):
		     hasAttribute = True
                     if (attr[j] in AttributeList) == False:
                         attribute = city.getComponent(names[i]).getAttribute(attr[j])
                         fielddef = osgeo.ogr.FieldDefn(attr[j], osgeo.ogr.OFTReal)
                         layer.CreateField(fielddef)
                         layerDefinition = layer.GetLayerDefn()  
                         AttributeList.append(attr[j])                         
		for i in range(len(names)): 
                    #Append Attributes
                    alist = city.getComponent(names[i]).getAllAttributes().keys() 

                    
                    face = city.getFace(names[i])
                            
                    line = osgeo.ogr.Geometry(osgeo.ogr.wkbPolygon)
                    ring = osgeo.ogr.Geometry(osgeo.ogr.wkbLinearRing)
                    nl =  TBVectorData.getNodeListFromFace(city, face)
                    for p in nl:
                        ring.AddPoint(p.getX(),p.getY())
                    line.AddGeometry(ring)

                    featureIndex = 0
                    feature = osgeo.ogr.Feature(layerDefinition)
                    feature.SetGeometry(line)
                    feature.SetFID(featureIndex)  
                    #Append Attributes
                    if hasAttribute:        
                        for k in range(len(alist)):
                              value = city.getComponent(names[i]).getAttribute(alist[k]).getDouble()
                              feature.SetField(alist[k],value)
                    layer.CreateFeature(feature)    

                shapeData.Destroy()    
                         
            def exportPolyline(self):
                city = self.getData("City")
                spatialReference = osgeo.osr.SpatialReference()
                spatialReference.ImportFromProj4('+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs')
                #Init Shape Files
                driver = osgeo.ogr.GetDriverByName('ESRI Shapefile')
                if os.path.exists(str(self.FileName+'_lines.shp')): os.remove(self.FileName+'_lines.shp')
                shapeData = driver.CreateDataSource(self.FileName+'_lines.shp')
                layer = shapeData.CreateLayer('layer1', spatialReference, osgeo.ogr.wkbLineString)
                layerDefinition = layer.GetLayerDefn()               
                AttributeList = []
                attr = []
                hasAttribute = False
                #Get Data 
		unique = 0
                names = city.getNamesOfComponentsInView(View(self.Name, READ, EDGE))
		NewAttriburteNamesForShape = {}
                if len(names) > 0:
                    attributemap = city.getComponent(names[0]).getAllAttributes()
                    for key in attributemap.keys():
                        attr.append(key)
                        unique = unique+1
                        NewAttriburteNamesForShape[key] = key[:7] + str(unique)
                for i in range(len(names)):
                    #Append Attributes
                    alist = city.getComponent(names[i]).getAllAttributes().keys() 
                    for j in range(len(alist)):
                        hasAttribute = True                                  
                        if (alist[j] in AttributeList) == False:                            
                            attribute = city.getComponent(names[0]).getAttribute(alist[j])
                            fielddef = osgeo.ogr.FieldDefn(NewAttriburteNamesForShape[alist[j]], osgeo.ogr.OFTReal)
                            layer.CreateField(fielddef)
                            layerDefinition = layer.GetLayerDefn()  
                            AttributeList.append(alist[j]) 
                    
                            
                    line = osgeo.ogr.Geometry(osgeo.ogr.wkbLineString)
                    edge = city.getEdge(names[i])
                    p1 = city.getNode(edge.getStartpointName())
                    p2 = city.getNode(edge.getEndpointName())
                    line.AddPoint(p1.getX(),p1.getY())
                    line.AddPoint(p2.getX(),p2.getY())

                    featureIndex = 0
                    feature = osgeo.ogr.Feature(layerDefinition)
                    feature.SetGeometry(line)
                    feature.SetFID(featureIndex)  
                    hasAttribute = True
                    if hasAttribute == True:        
                        for k in range(len(alist)):
                                 value = edge.getAttribute(alist[k]).getDouble()
                                 feature.SetField(NewAttriburteNamesForShape[alist[k]],value)
                    layer.CreateFeature(feature)    
                shapeData.Destroy()  
 
            def exportPoints(self):
		city = self.getData("City")
                spatialReference = osgeo.osr.SpatialReference()
                spatialReference.ImportFromProj4('+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs')
                
                #Init Shape Files
                driver = osgeo.ogr.GetDriverByName('ESRI Shapefile')
                if os.path.exists(str(self.FileName+'_points.shp')): os.remove(self.FileName+'_points.shp')
                shapeData = driver.CreateDataSource(self.FileName+'_points.shp')
                
                layer = shapeData.CreateLayer('layer1', spatialReference, osgeo.ogr.wkbPoint)
                #osgeo.ogr.wkbPolygon()
                layerDefinition = layer.GetLayerDefn()               
                AttributeList = []
                attr = []
                hasAttribute = False
          	unique = 0
 		NewAttriburteNamesForShape = {}      
                fielddef = osgeo.ogr.FieldDefn("Z", osgeo.ogr.OFTReal)
                layer.CreateField(fielddef)
                layerDefinition = layer.GetLayerDefn()  
                AttributeList.append("Z") 
                
                names = city.getNamesOfComponentsInView(View(self.Name, READ, NODE))
                for i in range(len(names)): 
                    attributemap = city.getComponent(names[i]).getAllAttributes()
                    for key in attributemap.keys():
			if (key in attr) == False:
                        	attr.append(key)
				unique = unique +1
				NewAttriburteNamesForShape[key] = key[:7] + str(unique)
                for j in range(len(attr)):
		     hasAttribute = True
                     if (attr[j] in AttributeList) == False:
                         attribute = city.getComponent(names[i]).getAttribute(attr[j])
                         fielddef = osgeo.ogr.FieldDefn(NewAttriburteNamesForShape[attr[j]], osgeo.ogr.OFTReal)
                         layer.CreateField(fielddef)
                         layerDefinition = layer.GetLayerDefn()  
                         AttributeList.append(attr[j])                         
		for i in range(len(names)): 
                    #Append Attributes
                    alist = city.getComponent(names[i]).getAllAttributes().keys()                          
                        
                            
                    #Addend Points
                    node = city.getNode(names[i])
                    point = osgeo.ogr.Geometry(osgeo.ogr.wkbPoint)
                    point.SetPoint(0, node.getX(),  node.getY())
                    featureIndex = 0
                    feature = osgeo.ogr.Feature(layerDefinition)
                    feature.SetGeometry(point)
                    feature.SetFID(featureIndex) 
                    feature.SetField("Z", node.getZ())
                    #Append Attributes
                    if hasAttribute:        
                        for k in range(len(alist)):
                              value = city.getComponent(names[i]).getAttribute(alist[k]).getDouble()
                              feature.SetField(NewAttriburteNamesForShape[alist[k]],value)
                    layer.CreateFeature(feature)  
                shapeData.Destroy()            
               
