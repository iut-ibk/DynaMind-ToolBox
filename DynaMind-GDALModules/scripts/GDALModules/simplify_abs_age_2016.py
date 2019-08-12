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


#Age_20_24, Age_25_29, Age_30_34, Age_35_39, Age_40_44, Age_45_49, Age_50_54, Age_55_59, Age_60_64, Age_65_69, Age_70_74, Age_75_79, Age_80_84, Age_85_89

class SimplifyABSAge2016(Module):
        display_name = "Categories Age from Census (AU)"
        group_name = "Urban Form"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

        def init(self):

            self.name_list = []

            self.name_list.append("age_20_24_yr_p")
            self.name_list.append("age_25_34_yr_p")
            self.name_list.append("age_35_44_yr_p")
            self.name_list.append("age_45_54_yr_p")
            self.name_list.append("age_55_64_yr_p")
            self.name_list.append("age_65_74_yr_p")
            self.name_list.append("age_75_84_yr_p")



            self.translation_table = {}
            #B04a
            self.translation_table["age_20_24_yr_p"] = "age_20_24"
            self.translation_table["age_25_34_yr_p"] = "age_25_34"
            self.translation_table["age_35_44_yr_p"] = "age_35_44"
            self.translation_table["age_45_54_yr_p"] = "age_45_54"
            self.translation_table["age_55_64_yr_p"] = "age_55_64"
            self.translation_table["age_65_74_yr_p"] = "age_65_74"
            self.translation_table["age_75_84_yr_p"] = "age_75_84"



            self.__b04 = ViewContainer("g01", NODE, READ)

            for k in self.translation_table.keys():
                self.__b04.addAttribute(k, Attribute.INT, READ)



            self.__ress = ViewContainer("hh_age_2016", NODE, WRITE)
            # self.__ress.addAttribute("low", Attribute.INT, WRITE)
            # self.__ress.addAttribute("medium", Attribute.INT, WRITE)
            # self.__ress.addAttribute("high", Attribute.INT, WRITE)

            self.__ress.addAttribute("cdf_age_20_24", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_age_25_34", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_age_35_44", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_age_45_54", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_age_55_64", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_age_65_74", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_age_75_84", Attribute.DOUBLE, WRITE)

            views = []
            views.append(self.__b04)
            views.append(self.__ress)

            self.registerViewContainers(views)

        def run(self):
            self.__b04.reset_reading()

            for b04 in self.__b04:
                values = {}
                for k in self.name_list:
                    values[k] = 0

                for k in self.name_list:
                    values[k] += b04.GetFieldAsInteger(k)

                f = self.__ress.create_feature()
                f.SetGeometry(b04.GetGeometryRef())

                sum = 0.0

                for vk in self.name_list:
                    sum += float(values[vk])

                if sum < 1.0:
                    continue

                #Create CDF
                cdf_sum = 0
                for k in self.name_list:
                    cdf_sum += values[k]
                    f.SetField("cdf_" + self.translation_table[k], float(cdf_sum) / sum)
            self.__b04.finalise()
            self.__ress.finalise()








