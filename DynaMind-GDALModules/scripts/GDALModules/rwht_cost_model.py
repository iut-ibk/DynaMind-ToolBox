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

from pydynamind import *


class RWHTCostModel(Module):
    display_name = "Rainwater Harvesting Tanks (AU)"
    group_name = "Economic Evaluation"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("rwht_view_name", STRING)
        self.rwht_view_name = "rwht"

        self.__construction_cost_db = {"melbourne": {2: 2525, 5: 2985, 10: 3560}}
        discount_rate = 0.05
        self.__years = []

        self.__discount_factor = []
        for y in xrange(1, 21, 1):
            self.__years.append(y)
            self.__discount_factor.append(1. / (1. + discount_rate) ** (y - 1))


    def init(self):
        self.__rwht = ViewContainer(self.rwht_view_name, COMPONENT, READ)
        self.__rwht.addAttribute("volume", Attribute.DOUBLE, READ)
        self.__rwht.addAttribute("annual_water_savings", Attribute.DOUBLE, READ)
        self.__rwht.addAttribute("non_potable_savings", Attribute.DOUBLE, READ)
        self.__rwht.addAttribute("outdoor_water_savings", Attribute.DOUBLE, READ)
        self.__rwht.addAttribute("pv_total_costs", Attribute.DOUBLE, WRITE)
        self.__rwht.addAttribute("pv_non_potable_saving", Attribute.DOUBLE, WRITE)
        self.__rwht.addAttribute("pv", Attribute.DOUBLE, WRITE)

        self.registerViewContainers([self.__rwht])

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
            return self.construction_costs(location, volume) + self.maintenance_costs()
        except (LookupError):
            raise ValueError("can't calculate total costs")

    def pv_non_potable_saving(self, annual_water_savings):
        indoor_water_price = []
        for y in self.__years:
            indoor_water_price.append(2.5)
        discount_non_potable_savings = [d * m * annual_water_savings for d, m in
                                        zip(self.__discount_factor, indoor_water_price)]

        return sum(discount_non_potable_savings)

    def run(self):
        self.__rwht.reset_reading()
        for r in self.__rwht:
            pv_total_costs = self.pv_total_costs("melbourne", r.GetFieldAsDouble("volume"))
            pv_non_potable_saving = self.pv_non_potable_saving(r.GetFieldAsDouble("annual_water_savings"))

            pv = pv_non_potable_saving - pv_total_costs
            r.SetField("pv_total_costs", pv_total_costs)
            r.SetField("pv_non_potable_saving", pv_non_potable_saving)
            r.SetField("pv", pv)

        self.__rwht.finalise()

if __name__ == "__main__":
    r = RWHTCostModel()
    r.init()

    print r.pv_total_costs("melbourne", 2), r.pv_non_potable_saving(50), r.pv_outdoor_savings(31, 1, 4, 20, 40)
    print r.pv_total_costs("melbourne", 5), r.pv_non_potable_saving(100), r.pv_outdoor_savings(31, 1, 4, 20, 10)
    print r.pv_total_costs("melbourne", 10), r.pv_non_potable_saving(150), r.pv_outdoor_savings(31, 0, 4, 20, 20)
