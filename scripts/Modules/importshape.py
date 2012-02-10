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

class ImportShapeFile(Module):
        def __init__(self):            
            Module.__init__(self)            
            self.createParameter("FileName", FILENAME, "filename")
            self.FileName = ""
            self.createParameter("Identifier", STRING, "Name")
            self.Identifier = ""
            self.vec = View("STREET", EDGE, WRITE)
            views = []
            views.append(self.vec)
            self.addData("Vec", views)
        def run(self):
            city = self.getData("Vec")
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
            feature = layer.GetNextFeature()
            #print layer.GetFeatureCount()
            #while there are more features,
            while feature:
                    shapelyGeometries.append(wkb.loads(feature.GetGeometryRef().ExportToWkb()))
                    fieldPacks.append([feature.GetField(x) for x in fieldIndices])
                    # Get the next feature
                    feature = layer.GetNextFeature()
         
            counter = 1

            for i in range(len(shapelyGeometries)):
                shapelyGeometry = shapelyGeometries[i]
                geoms = []
                #print shapelyGeometry.type
                if shapelyGeometry.type == 'Point':            
                        n = city.addNode(shapelyGeometry.x , shapelyGeometry.y, 0, View())                    
                        for j in range(len(fieldDefinitions)):
                            attr = Attribute(fieldDefinitions[j][0])
                            print type(fieldPacks[i][j])
                            if type(fieldPacks[i][j]) is 'string':                            
                                attr.setString(str(fieldPacks[i][j]))
                            if type(fieldPacks[i][j]) is 'float':                            
                                attr.setDouble(fieldPacks[i][j])
                
                elif shapelyGeometry.type == 'Polygon' or shapelyGeometry.type == 'LineString':
                        geoms.append(shapelyGeometry)
                else:
                        geoms = list(shapelyGeometry.geoms)
                if shapelyGeometry.type != 'Point':            
                    for geo in geoms:
                        if shapelyGeometry.type != 'LineString':
                            r = geo.exterior
                            coordinates = list(r.coords)
                        else:
                            coordinates = geo.coords
                        numberOfPoints = 0
                        pl = []
                        el = edgevector()
                        for coords in coordinates:
                            n = TBVectorData_addNodeToSystem2D(city,View(), Node(coords[0], coords[1], 0), 0.1)
                            pl.append(n)
                            if numberOfPoints > 0:
                                e = city.addEdge(pl[numberOfPoints - 1], pl[numberOfPoints], self.vec)
                                el.append(e)
                                counter += 1
                            numberOfPoints += 1     
                        
                        #print counter

                        for j in range(len(fieldDefinitions)):
                            attr = Attribute(fieldDefinitions[j][0])
                            #print type(fieldPacks[i][j])
                            if type(fieldPacks[i][j]) is 'string':                            
                                attr.setString(str(fieldPacks[i][j]))
                            if type(fieldPacks[i][j]) is 'float' or type(fieldPacks[i][j]) is 'int':                            
                                attr.setDouble(fieldPacks[i][j])
                            e.addAttribute(attr)
                        if shapelyGeometry.type is 'LineString':
                            continue
                        #Create Faces
                        
                        city.addFace(el, self.vec)
            print "Imported points: " + str( len(city.getAllNodes()))
            print "Imported elements: " + str( len(city.getNamesOfComponentsInView(self.vec)) )
            print "Edges Created: " + str( counter )
            