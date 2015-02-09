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

            self.__construction_cost_db = {"melbourne": {2: 2525, 5: 2985, 10: 3560}}
            discount_rate = 0.05
            self.__years = []
            self.__discount_factor = []
            for y in xrange(1, 21, 1):
                self.__years.append(y)
                self.__discount_factor.append(1./(1.+discount_rate)**(y-1))


        def init(self):
            self.__rwht = ViewContainer("rwht", COMPONENT, WRITE)
            self.__rwht.addAttribute("volume", Attribute.DOUBLE, READ)
            self.__rwht.addAttribute("annual_water_savings", Attribute.DOUBLE, READ)
            self.__rwht.addAttribute("non_potable_savings", Attribute.DOUBLE, READ)
            self.__rwht.addAttribute("outdoor_water_savings", Attribute.DOUBLE, READ)
            self.__rwht.addAttribute("parcel_id", Attribute.INT, READ)
            self.__rwht.addAttribute("active", Attribute.INT, WRITE)
            self.__rwht.addAttribute("pv_total_costs", Attribute.DOUBLE, WRITE)
            self.__rwht.addAttribute("pv_non_potable_saving", Attribute.DOUBLE, WRITE)
            self.__rwht.addAttribute("pv_outdoor_savings", Attribute.DOUBLE, WRITE)
            self.__rwht.addAttribute("pv", Attribute.DOUBLE, WRITE)

            self.__parcel = ViewContainer("parcel", FACE, READ)
            self.__parcel.addAttribute("pv", Attribute.DOUBLE, WRITE)
            self.__parcel.addAttribute("annual_water_savings", Attribute.DOUBLE, WRITE)

            # self.__building = ViewContainer("building", FACE, READ)
            # self.__building.addAttribute("parcel_id", Attribute.INT, READ)

            # self.__household = ViewContainer("household", NODE, READ)
            # self.__household.addAttribute("building_id", Attribute.INT, READ)
            # self.__household.addAttribute("wtp_water_security", Attribute.DOUBLE, WRITE)

            #self.__household = ViewContainer("age", Attribute.INT, READ)
            #self.__household = ViewContainer("education", Attribute.STRING, READ)
            #self.__household = ViewContainer("bedrooms", Attribute.INT, READ)
            #self.__household, self.__building,



            self.registerViewContainers([self.__rwht, self.__parcel])

        def construction_costs(self, location, size):
            """
            Returns construction costs based on the tank size
            :param size:
            :return:
            """
            try:
                return self.__construction_cost_db[location][int(size)]
            except:
                raise LookupError("No such tank found")

        def maintenance_costs(self):
            maintenance_cost = []
            for y in self.__years:
                maintenance_cost.append(20.)

            discount_maintenance_cost = [d * m for d, m in zip(self.__discount_factor, maintenance_cost)]

            return sum(discount_maintenance_cost)

        def pv_total_costs(self, location, volume):
            try:
                return self.construction_costs(location,  volume) + self.maintenance_costs()
            except (LookupError):
                raise ValueError("can't calculate total costs")

        def pv_non_potable_saving(self, annual_water_savings):
            indoor_water_price = []
            for y in self.__years:
                indoor_water_price.append(2.5)
            discount_non_potable_savings = [d * m * annual_water_savings for d, m in zip(self.__discount_factor, indoor_water_price)]

            return sum(discount_non_potable_savings)

        def pv_outdoor_savings(self, age, education, bedroom, outdoor_demand, non_potable_savings):
            wtp_water_stress = self.agent_wtp(age, education, bedroom)
            if outdoor_demand > non_potable_savings:
                wtp_water_stress = 0

            return sum([d * wtp_water_stress for d in self.__discount_factor])

        def agent_wtp(self, age, education, bedroom):
            return 3.208593 - 0.6362358*bedroom + 0.356186*age - 0.0027977*age*age - 1.883466*education

        def get_household(self, p):
            for b in self.__building.get_linked_features(p):
                for h in self.__household.get_linked_features(b):
                    return h

        def run(self):
            # self.__building.create_index("parcel_id")
            # self.__household.create_index("building_id")
            self.__rwht.create_index("parcel_id")

            self.__parcel.reset_reading()
            counter = 0
            counter_parcel = 0
            pv_2000 =0
            pv_1500 =0
            pv_750 =0
            for p in self.__parcel:
                counter_parcel+=1
                #h = self.get_household(p)
                pv_current = -1000000.
                annual_water_savings = 0
                for r in self.__rwht.get_linked_features(p):

                    pv_total_costs = self.pv_total_costs("melbourne", r.GetFieldAsDouble("volume"))
                    pv_non_potable_saving = self.pv_non_potable_saving(r.GetFieldAsDouble("annual_water_savings"))
                    pv_outdoor_savings = self.pv_outdoor_savings(31,1,4, 10,20)
                    pv = pv_non_potable_saving + pv_outdoor_savings - pv_total_costs
                    r.SetField("pv_total_costs", pv_total_costs)
                    r.SetField("pv_outdoor_savings",pv_outdoor_savings)
                    r.SetField("pv_non_potable_saving",pv_non_potable_saving)
                    r.SetField("pv", pv)
                    counter+=1

                    if pv > pv_current:
                        pv_current = pv
                        annual_water_savings = r.GetFieldAsDouble("annual_water_savings")

                if counter % 1000 == 0:
                    print counter
                if pv >= -1500.:
                    pv_1500 +=1
                if pv >= -750.:
                    pv_750 +=1
                if pv >= -2000.:
                    pv_2000 +=1
                p.SetField("pv", pv)
                p.SetField("annual_water_savings", annual_water_savings)

                #self.__parcel.sync()
                #self.__rwht.sync()



            self.__rwht.sync()
            self.__parcel.sync()

            print pv_2000 / float(counter_parcel)
            print pv_1500 / float(counter_parcel)
            print pv_750 / float(counter_parcel)
            print counter


            # for rwht in self.__rwht:
            #     # Create CDF
            #     volume = rwht.GetFieldAsDouble("volume")
            #     rwht.SetField("construction_costs", self.construction_costs("melbourne", volume))
            #
            #     annual_water_savings = rwht.GetFieldAsDouble("annual_water_savings")
            #     rwht.SetField("annual_savings", annual_water_savings * 2.50)


if __name__ == "__main__":
    r = RWHTCostModel()
    r.init()

    print r.pv_total_costs("melbourne", 2), r.pv_non_potable_saving(50), r.pv_outdoor_savings(31,1,4,20,40)
    print r.pv_total_costs("melbourne", 5), r.pv_non_potable_saving(100), r.pv_outdoor_savings(31,1,4,20,10)
    print r.pv_total_costs("melbourne", 10), r.pv_non_potable_saving(150), r.pv_outdoor_savings(31,0,4,20,20)







