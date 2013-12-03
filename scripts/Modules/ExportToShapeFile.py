"""
@file
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2011-2013  Christian Urich

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


class AttributeWriter:
    def __init__(self, city,view, layer):
        self.NewAttriburteNamesForShape = {}  
        self.attr = {}
        names = city.getUUIDsOfComponentsInView(view)
        unique = 0
        for i in range(len(names)): 
            self.attributemap = city.getComponent(names[i]).getAllAttributes()
            for key in self.attributemap.keys():
                if (key in self.attr.keys()) == False:
                    attribute = city.getComponent(names[i]).getAttribute(key)
                    if Attribute.NOTYPE == attribute.getType():
                        continue
                    self.attr[key] = attribute.getType()
                    if len(key) > 7:
                        unique = unique +1
                        self.NewAttriburteNamesForShape[key] = key[:7] + str(unique)
                    else:
                        self.NewAttriburteNamesForShape[key] = key

        for a in self.attr.keys():
            if self.attr[a] == Attribute.DOUBLE:
                fielddef = osgeo.ogr.FieldDefn(self.NewAttriburteNamesForShape[a], osgeo.ogr.OFTReal)
            elif self.attr[a] == Attribute.STRING:
                fielddef = osgeo.ogr.FieldDefn(self.NewAttriburteNamesForShape[a], osgeo.ogr.OFTString)
            else:
                print "Attribute type for " + a + " not supported"
                del self.attr[a]
                continue

            layer.CreateField(fielddef)

    def addAttriubte(self, feature, city, uuid):
        for attr_name in self.attr.keys():
            if self.attr[attr_name] == Attribute.STRING:
                value = city.getComponent(uuid).getAttribute(attr_name).getString()
                feature.SetField(self.NewAttriburteNamesForShape[attr_name],value)
            if self.attr[attr_name] == Attribute.DOUBLE:
                value = city.getComponent(uuid).getAttribute(attr_name).getDouble()
                feature.SetField(self.NewAttriburteNamesForShape[attr_name],value)


class ExportToShapeFile(Module):
            def __init__(self):
                Module.__init__(self)
 
                #self.addParameter("Name", STRING, self.FileName)
                self.createParameter("FileName", STRING,  "test")
                self.FileName = "Shapefile"
                self.createParameter("Name", STRING,  "test")
                self.Name = ""   
                self.createParameter("CoordinateSystemEPSG", INT, "EPSG Code")
                self.createParameter("offsetX", DOUBLE, "OffsetX")
                self.createParameter("offsetY", DOUBLE, "OffsetY") 
                self.CoordinateSystemEPSG = 32755
                self.vec = View("dummy", SUBSYSTEM, MODIFY)
                self.offsetX = 0
                self.offsetY = 0                
                views = []
                views.append(self.vec)
                self.addData("City", views)
                self.counter = 0

            def run(self):
                self.counter += 1
                city = self.getData("City")
                views = city.getViews()
                for view in views:
                    if view.getName() != self.Name:
                        continue
                    if view.getType() == FACE:                   
                        self.exportFaces()
                        return
                    if view.getType() == EDGE:
                        self.exportPolyline()
                        return       
                    if view.getType() == NODE:
                        self.exportPoints()
                        return     
                print "View doesn't exist"

            def exportFaces(self):
                city = self.getData("City")
                spatialReference = osgeo.osr.SpatialReference()
                spatialReference.ImportFromEPSG(self.CoordinateSystemEPSG)
                #Init Shape Files
                driver = osgeo.ogr.GetDriverByName('ESRI Shapefile')
                if os.path.exists(self.FileName+'_faces_' + str(self.counter) + '.shp'): os.remove(self.FileName+'_faces_' + str(self.counter) + '.shp')
                shapeData = driver.CreateDataSource(self.FileName+'_faces_' + str(self.counter) + '.shp')
                
                layer = shapeData.CreateLayer('layer1', spatialReference, osgeo.ogr.wkbPolygon)
                              
                v = View(self.Name, READ, FACE)

                attriutewriter = AttributeWriter(city, v,layer)
                layerDefinition = layer.GetLayerDefn()
                names = city.getUUIDsOfComponentsInView(v)
                for i in range(len(names)): 
                    face = city.getFace(names[i])
                    line = osgeo.ogr.Geometry(osgeo.ogr.wkbPolygon)
                    ring = osgeo.ogr.Geometry(osgeo.ogr.wkbLinearRing)
                    nl =  TBVectorData.getNodeListFromFace(city, face)
                    nl.append(nl[0]) #face is not closed in DyanMind
                    for p in nl:
                        ring.AddPoint(p.getX()+ self.offsetX,p.getY()+ self.offsetY)
                    line.AddGeometry(ring)

                    featureIndex = 0
                    feature = osgeo.ogr.Feature(layerDefinition)
                    feature.SetGeometry(line)
                    feature.SetFID(featureIndex)  

                    attriutewriter.addAttriubte(feature,city, names[i])     
                    layer.CreateFeature(feature)      
                shapeData.Destroy()    
                         
            def exportPolyline(self):
                city = self.getData("City")
                spatialReference = osgeo.osr.SpatialReference()
                spatialReference.ImportFromEPSG(self.CoordinateSystemEPSG)
                #Init Shape Files
                driver = osgeo.ogr.GetDriverByName('ESRI Shapefile')
                if os.path.exists(self.FileName+'_edges_' + str(self.counter) + '.shp'): os.remove(self.FileName+'_edges_' + str(self.counter) + '.shp')
                shapeData = driver.CreateDataSource(self.FileName+'_edges_' + str(self.counter) + '.shp')
                layer = shapeData.CreateLayer('layer1', spatialReference, osgeo.ogr.wkbLineString)
                
                v = View(self.Name, READ, EDGE)

                names = city.getUUIDsOfComponentsInView(v)

                attriutewriter = AttributeWriter(city, v,layer)
                layerDefinition = layer.GetLayerDefn()    
                    
                for i in range(len(names)):                             
                    line = osgeo.ogr.Geometry(osgeo.ogr.wkbLineString)
                    edge = city.getEdge(names[i])
                    p1 = city.getNode(edge.getStartpointName())
                    p2 = city.getNode(edge.getEndpointName())
                    line.AddPoint(p1.getX() + self.offsetX ,p1.getY() + self.offsetY)
                    line.AddPoint(p2.getX()+ self.offsetX ,p2.getY()+ self.offsetY)
                    featureIndex = 0
                    feature = osgeo.ogr.Feature(layerDefinition)
                    feature.SetGeometry(line)
                    feature.SetFID(featureIndex)  
                    attriutewriter.addAttriubte(feature,city, names[i])   
                    layer.CreateFeature(feature)    
                shapeData.Destroy()  
 
            def exportPoints(self):
                city = self.getData("City")
                spatialReference = osgeo.osr.SpatialReference()
                spatialReference.ImportFromEPSG(self.CoordinateSystemEPSG)
                
                #Init Shape Files
                driver = osgeo.ogr.GetDriverByName('ESRI Shapefile')
                if os.path.exists(self.FileName+'_nodes_' + str(self.counter) + '.shp'): os.remove(self.FileName+'_nodes_' + str(self.counter) + '.shp')
                shapeData = driver.CreateDataSource(self.FileName+'_nodes_' + str(self.counter) + '.shp')
                
                layer = shapeData.CreateLayer('layer1', spatialReference, osgeo.ogr.wkbPoint)
                v = View(self.Name, READ, NODE)

                names = city.getUUIDsOfComponentsInView(v)

                attriutewriter = AttributeWriter(city, v,layer)
                layerDefinition = layer.GetLayerDefn()     
                                    
                for i in range(len(names)): 
                    #Append Attributes
                    alist = city.getComponent(names[i]).getAllAttributes().keys()   
                    #Addend Points
                    node = city.getNode(names[i])
                    point = osgeo.ogr.Geometry(osgeo.ogr.wkbPoint)
                    point.SetPoint(0, node.getX()+ self.offsetX,  node.getY()+ self.offsetY)
                    featureIndex = 0
                    feature = osgeo.ogr.Feature(layerDefinition)
                    feature.SetGeometry(point)
                    feature.SetFID(featureIndex) 
                    attriutewriter.addAttriubte(feature,city, names[i])   
                    layer.CreateFeature(feature)    
                shapeData.Destroy()

            def getHelpUrl(self):
                return "https://github.com/iut-ibk/DynaMind-ToolBox/wiki/ExportToShapeFile"             