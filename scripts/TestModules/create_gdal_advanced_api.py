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


class CreateGDALComponentsAdvanced(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)
            self.createParameter("elements", INT, "Number of elements")
            self.elements = 100000

            self.createParameter("append", BOOL, "true if append")
            self.append = False
            self.__container = ViewContainer()

        def init(self):
            if self.append:
                self.__container = ViewContainer("component", NODE, MODIFY)
            else:
                self.__container = ViewContainer("component", NODE, WRITE)

            views = []
            views.append(self.__container)
            self.registerViewContainers(views)
            #self.features = []
        def run(self):

            for i in range(self.elements):
                f = self.__container.create_feature()
                pt = ogr.Geometry(ogr.wkbPoint)
                pt.SetPoint_2D(0, 1, 1)
                f.SetGeometry(pt)
                if i % 100000 == 0:
                    self.__container.sync()
            self.__container.sync()





