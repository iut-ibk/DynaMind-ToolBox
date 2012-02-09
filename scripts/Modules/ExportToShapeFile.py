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

class ExportToShapeFile(Module):
            def __init__(self):
                Module.__init__(self)
                self.Points = True
                self.Lines = True
                self.Faces = True
 
                #self.addParameter("Name", STRING, self.FileName)
                self.createParameter("FileName", STRING,  "test")
                self.FileName = "Shapefile"
                self.createParameter("Name", STRING,  "test")
                self.Name = ""    
                self.createParameter("Points", BOOL,  "test")
                self.Points = True
                self.createParameter("Lines", BOOL,  "test")
                self.Lines = True
                self.createParameter("Faces", BOOL,  "test")
                self.Faces = True
                self.vec = View("STREET", EDGE, READ)
                views = []
                views.append(self.vec)
                self.addData("vec", views)
                
            def run(self):
                if self.Lines:
                    self.exportPolyline()      
                
            def exportPolyline(self):
                city = self.getData("vec")
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
                System
                names = city.getNamesOfComponentsInView(self.vec)
                if len(names) > 0:
                    attributemap = city.getComponent(names[0]).getAllAttributes()
                    for key in attributemap.keys():
                        attr.append(key)
                for i in range(len(names)): 
                    #Append Attributes
                    if names[i] in attr:
                        alist = city.getComponent(names[0]).getAllAttributes().keys()
                        #Check if attribute exists             
                        for j in range(len(alist)):
                            hasAttribute = True                                  
                            if (alist[j] in AttributeList) == False:
                                fielddef = osgeo.ogr.FieldDefn(alist[j], osgeo.ogr.OFTReal)
                                layer.CreateField(fielddef)
                                layerDefinition = layer.GetLayerDefn()  
                                log(alist[j])
                                AttributeList.append(alist[j]) 
                    
                            
                    line = osgeo.ogr.Geometry(osgeo.ogr.wkbLineString)
                    print names[i]
                    edge = city.getEdge(names[i])
                    p1 = city.getNode(edge.getStartpointName())
                    p2 = city.getNode(edge.getEndpointName())
                    line.AddPoint(p1.getX(),p1.getY())
                    line.AddPoint(p2.getX(),p2.getY())

                    featureIndex = 0
                    feature = osgeo.ogr.Feature(layerDefinition)
                    feature.SetGeometry(line)
                    feature.SetFID(featureIndex)  
                    #Append Attributes
                    if hasAttribute:        
                        for k in range(len(alist)):
                            value = edge.getAttribute(alist[k]).getDouble()
                            feature.SetField(alist[k],value)
                    layer.CreateFeature(feature)    
                shapeData.Destroy()               
               
