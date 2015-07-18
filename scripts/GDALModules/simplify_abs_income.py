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


class SimplifyABSIncome(Module):
        display_name = "Categories Income from Census (AU)"
        group_name = "Urban Form"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

        def init(self):

            self.translation_table = {}
            #B40b
            self.translation_table["negative_nil_income_total"] = "low"
            self.translation_table["1_199_total"] = "low"
            self.translation_table["200_299_total"] = "low"
            self.translation_table["300_399_total"] = "low"
            self.translation_table["400_599_total"] = "low"
            self.translation_table["600_799_total"] = "low"
            self.translation_table["800_999_total"] = "medium"
            self.translation_table["1000_1249_total"] = "medium"
            self.translation_table["1250_1499_total"] = "medium"
            self.translation_table["1500_1999_total"] = "medium"
            self.translation_table["2000_2499_total"] = "high"
            self.translation_table["2500_2999_total"] = "high"
            self.translation_table["3000_3499_total"] = "high"
            self.translation_table["3500_3999_total"] = "high"
            self.translation_table["2500_2999_total"] = "high"
            self.translation_table["3000_3499_total"] = "high"
            self.translation_table["4000_or_more_total"] = "high"


            self.__b28 = ViewContainer("b28", NODE, READ)

            for k in self.translation_table.keys():
                self.__b28.addAttribute(k, Attribute.INT, READ)



            self.__ress = ViewContainer("hh_income", NODE, WRITE)
            self.__ress.addAttribute("low", Attribute.INT, WRITE)
            self.__ress.addAttribute("medium", Attribute.INT, WRITE)
            self.__ress.addAttribute("high", Attribute.INT, WRITE)

            self.__ress.addAttribute("cdf_low", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_medium", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_high", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_mean", Attribute.DOUBLE, WRITE)


            views = []
            views.append(self.__b28)
            views.append(self.__ress)

            self.registerViewContainers(views)


        def run(self):

            self.__b28.reset_reading()

            for b40 in self.__b28:
                values = {}
                values["low"] = 0
                values["medium"] = 0
                values["high"] = 0



                for k in self.translation_table.keys():
                    values[self.translation_table[k]] += b40.GetFieldAsInteger(k)

                f = self.__ress.create_feature()

                f.SetGeometry(b40.GetGeometryRef())

                sum = 0.0
                for vk in values.keys():
                    sum += float(values[vk])
                    f.SetField(vk, int(values[vk]))

                if sum < 1.0:
                    continue
                #Create CDF
                f.SetField("cdf_low", float(values["low"]) / sum)
                f.SetField("cdf_medium", float(values["low"] + values["medium"]) / sum)
                f.SetField("cdf_high", float(values["low"] + values["medium"] + values["high"]) / sum )
                f.SetField("cdf_mean", float(values["low"]*1 + values["medium"]*2 + values["high"]*3)  / sum )









