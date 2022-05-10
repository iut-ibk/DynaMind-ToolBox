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


class CreateGDALComponentLinks(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)
            self.createParameter("elements", INT, "Number of elements")
            self.elements = 100000


        def init(self):
            self.__container = ViewContainer("component", COMPONENT, READ)

            self.__container_links = ViewContainer("component_link", COMPONENT, WRITE)
            self.__container_links.addAttribute("component_id", Attribute.INT, WRITE)

            self.registerViewContainers([self.__container, self.__container_links])
            #self.features = []

        def run(self):
            self.__container.reset_reading()
            for cmp in self.__container:
                f = self.__container_links.create_feature()
                f.SetField("component_id", cmp.GetFID())

            self.__container.finalise()
            self.__container_links.finalise()





