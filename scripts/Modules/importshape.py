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
            
            
        def init(self):
            pass
        def run(self):
            city = self.getData("Vec")
            registeredViews = self.getViews()
            self.NodeView = View();
            self.EdgeView = View();
            self.FaceView = View();
            
            for key in registeredViews.keys():
                for view in registeredViews[key]:
                    if view.getType() == NODE:
                        self.NodeView = view;
                    if view.getType() == EDGE:
                        self.EdgeView = view;
                    if view.getType() == FACE:
                        self.FaceView = view;
            print self.NodeView.getName()
            print self.EdgeView.getName()
            print self.FaceView.getName()
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
            InlcudedEdges = {}
            for i in range(len(shapelyGeometries)):
                shapelyGeometry = shapelyGeometries[i]
                geoms = []
                #print shapelyGeometry.type
                if shapelyGeometry.type == 'Point':            
                        n = city.addNode(shapelyGeometry.x , shapelyGeometry.y, 0, View())       
                        city.addComponentToView(n, self.NodeView)
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
                            
                        numberOfPoints = -1
                        offset = -1
                        pl = []
                        el = edgevector()
                        for coords in coordinates:
                            n = TBVectorData_addNodeToSystem2D(city, self.NodeView, Node(coords[0], coords[1], 0), 0.1)
                            pl.append(n)
                            numberOfPoints += 1     
                            offset +=1
                            if numberOfPoints > 0:
                                if pl[numberOfPoints - offset].getName() != pl[numberOfPoints].getName(): 
                                    startnode =   pl[numberOfPoints - offset]
                                    endnode = pl[numberOfPoints]
                                    """if InlcudedEdges.has_key(startnode.getName()):
                                        if InlcudedEdges[startnode.getName()] is endnode.getName():
                                            print "Edge already exits"
                                            continue
                                    if InlcudedEdges.has_key(endnode.getName()):
                                        if InlcudedEdges[endnode.getName()] is startnode.getName():
                                            print "Edge already exits"
                                            continue"""
                                    e = city.addEdge(pl[numberOfPoints - offset], pl[numberOfPoints],  self.EdgeView)
                                    el.append(e)
                                    InlcudedEdges[startnode.getName()] = endnode.getName()
                                    offset = 0
                                    for j in range(len(fieldDefinitions)):                           
                                        attr = Attribute(fieldDefinitions[j][0])
                                        if type(fieldPacks[i][j]).__name__ == 'str':                            
                                            attr.setString(str(fieldPacks[i][j]))
                                        if type(fieldPacks[i][j]).__name__ == 'float' or type(fieldPacks[i][j]) == 'int':                                                          
                                            attr.setDouble(fieldPacks[i][j])
                                        e.addAttribute(attr)
                                        
                        if shapelyGeometry.type is 'LineString':
                            continue
                        #Create Faces
                        
                        city.addFace(el, self.vec)
            print "Imported points: " + str( len(city.getAllNodes()))
            print "Imported elements: " + str( len(city.getNamesOfComponentsInView( self.EdgeView)) )
            print "Edges Created: " + str( counter )
            