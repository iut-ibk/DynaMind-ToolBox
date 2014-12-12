"""
@file
@author  Chrisitan Urich <christian.urich@gmail.com>
@version 1.0
@section LICENSE

This file is part of DynaMind
Copyright (C) 2014 Christian Urich

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


class RWHTCostModel(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

        def init(self):
            self.__rwht = ViewContainer("rwht", NODE, WRITE)
            self.__rwht.addAttribute("volume", Attribute.DOUBLE, READ)
            self.__rwht.addAttribute("annual_water_savings", Attribute.DOUBLE, READ)
            self.__rwht.addAttribute("construction_costs", Attribute.DOUBLE, WRITE)
            self.__rwht.addAttribute("maintenance_costs", Attribute.DOUBLE, WRITE)
            self.__rwht.addAttribute("annual_savings", Attribute.DOUBLE, WRITE)

            #Data base values are from CRC-WSC A4
            self.__construction_cost_db = {"melbourne": {2 : 2375, 5: 2485, 10: 2560 }}

            views = []
            views.append(self.__rwht)

            self.registerViewContainers(views)

        def __construction_costs(self, location, size):
            """
            Returns construction costs based on the tank size
            :param size:
            :return:
            """
            try:
                return self.__construction_cost_db[location][int(size)]
            except:
                return -1.0


        def run(self):
            self.__rwht.reset_reading()
            for rwht in self.__rwht:
                #Create CDF
                volume = rwht.GetFieldAsDouble("volume")
                rwht.SetField("construction_costs", self.__construction_costs("melbourne", volume))








