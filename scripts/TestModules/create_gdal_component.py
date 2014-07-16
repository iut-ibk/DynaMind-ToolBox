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

from osgeo import ogr
from pydynamind import *


class CreateGDALComponents(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)
            self.createParameter("Height",LONG,"Sample Description")
            self.createParameter("Width",LONG,"Sample Description")
            self.createParameter("CellSize",DOUBLE,"Sample Description")
            self.Height = 200
            self.Width = 200
            self.CellSize = 20

            views = []
            self.n = ViewContainer("OUT", NODE, WRITE)
            print self.n
            views.append(self.n)

            self.addGDALData("OUT", views)

    
        def run(self):
            system = self.getGDALData("OUT")
            print system
            print self.n
            self.n.setCurrentGDALSystem(system)
            #system.updateViewContainer(self.n)
            
            #ogr.Feature.GetFieldAsInteger("dynamind_id")
            #f1 = Feature()
            #print f1
            f = self.n.create_feature()
            print f
            print f.keys()
            pt = ogr.Geometry(ogr.wkbPoint)
            pt.SetPoint_2D(0, 1, 1)
            f.SetGeometry(pt)
            self.n.syncAlteredFeatures()

            


