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


class SimplifyABSEducation(Module):
        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

        def init(self):

            self.translation_table = {}
            #B40b
            self.translation_table["persons_postgraduate_degree_level_total"] = "tertiary"
            self.translation_table["persons_graduate_diploma_and_graduate_certificate_level_total"] = "tertiary"
            self.translation_table["persons_bachelor_degree_level_total"] = "tertiary"
            self.translation_table["persons_advanced_diploma_and_diploma_level_total"] = "technical"
            # self.translation_table["persons_certificate_level_certificate_level_nfd_total"]= "technical"
            # self.translation_table["persons_certificate_level_certificate_iii_and_iv_level_total"]= "technical"
            # self.translation_table["persons_certificate_level_certificate_i_and_ii_level_total"]= "technical"
            self.translation_table["persons_certificate_level_total_total"] = "technical"
            self.translation_table["persons_level_of_education_inadequately_described_total"] = "other"
            self.translation_table["persons_level_of_education_not_stated_total"] = "other"
            #self.translation_table["persons_total_total"]

            self.translation_table_1 = {}
            self.translation_table_1["total_persons_persons"] = "other"
            self.translation_table_1["highest_year_of_school_completed_year_12_or_equivalent_persons"] = "secondary"
            self.translation_table_1["highest_year_of_school_completed_year_11_or_equivalent_persons"] = "secondary"
            self.translation_table_1["highest_year_of_school_completed_year_10_or_equivalent_persons"] = "secondary"
            self.translation_table_1["highest_year_of_school_completed_year_9_or_equivalent_persons"] = "secondary"
            self.translation_table_1["highest_year_of_school_completed_year_8_or_below_persons"] = "secondary"
            self.translation_table_1["highest_year_of_school_completed_did_not_go_to_school_persons"] = "other"


            self.__b40b = ViewContainer("b40b", NODE, READ)

            for k in self.translation_table.keys():
                self.__b40b.addAttribute(k, Attribute.INT, READ)

            self.__b01 = ViewContainer("b01", NODE, READ)

            for k in self.translation_table_1.keys():
                self.__b01.addAttribute(k, Attribute.INT, READ)

            self.__ress = ViewContainer("education", NODE, WRITE)
            self.__ress.addAttribute("tertiary", Attribute.INT, WRITE)
            self.__ress.addAttribute("technical", Attribute.INT, WRITE)
            self.__ress.addAttribute("secondary", Attribute.INT, WRITE)
            self.__ress.addAttribute("other", Attribute.INT, WRITE)

            self.__ress.addAttribute("cdf_tertiary", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_other", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_technical", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_secondary", Attribute.DOUBLE, WRITE)
            self.__ress.addAttribute("cdf_mean", Attribute.DOUBLE, WRITE)


            views = []
            views.append(self.__b40b)
            views.append(self.__ress)
            views.append(self.__b01)

            self.registerViewContainers(views)


        def run(self):

            self.__b40b.reset_reading()

            for b40 in self.__b40b:
                values = {}
                values["tertiary"] = 0
                values["secondary"] = 0
                values["technical"] = 0
                values["other"] = 0



                for k in self.translation_table.keys():
                    values[self.translation_table[k]] = b40.GetFieldAsInteger(k)

                id = b40.GetFID()
                #print id
                b01 = self.__b01.get_feature(id)
                #print b01

                for k in self.translation_table_1.keys():
                     values[self.translation_table_1[k]] += b01.GetFieldAsInteger(k)


                f = self.__ress.create_feature()

                f.SetGeometry(b40.GetGeometryRef())

                sum = 0.0
                for vk in values.keys():
                    sum += float(values[vk])
                    f.SetField(vk, int(values[vk]))

                if sum < 1.0:
                    continue
                #Create CDF
                f.SetField("cdf_secondary", float(values["secondary"]) / sum)
                f.SetField("cdf_other", float(values["secondary"] + values["other"]) / sum)
                f.SetField("cdf_technical", float(values["secondary"] + values["other"] + values["technical"]) / sum )
                f.SetField("cdf_tertiary", float(values["secondary"] + values["other"] + values["technical"] + values["tertiary"]) / sum )
                f.SetField("cdf_mean", float(values["secondary"]*1 + values["other"]*2 + values["technical"]*3 + values["tertiary"]*4) / sum)









