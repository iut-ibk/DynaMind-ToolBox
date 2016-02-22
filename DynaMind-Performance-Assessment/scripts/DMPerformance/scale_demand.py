from osgeo import ogr
import csv
from pydynamind import *

from datetime import datetime, timedelta

import pdb

class ScaleOutdoorDemand(Module):
        display_name = "Dry Weather Period"
        group_name = "Performance Assessment"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("start_date", STRING)
            self.start_date = "2000-Jan-01 00:00:00"

            self.createParameter("end_date", STRING)
            self.end_date = "2001-Jan-01 00:00:00"

            self.createParameter("start_dry_period", STRING)
            self.start_dry_period = "2000-Apr-30 00:00:00"

            self.createParameter("end_dry_period", STRING)
            self.end_dry_period = "2000-Nov-00 00:00:00"

            self.createParameter("scaling_factor", DOUBLE)
            self.scaling_factor = 1.0

            self.createParameter("view_name", STRING)
            self.view_name = "parcel"

            self.createParameter("attribute_name", STRING)
            self.attribute_name = "outdoor_demand_daily"

        def init(self):
            self.parcel = ViewContainer(self.view_name, EDGE, READ)
            self.parcel.addAttribute(self.attribute_name, Attribute.DOUBLEVECTOR, MODIFY)

            self.registerViewContainers([self.parcel])

        def run(self):
            start_date = "2000-Jan-01 00:00:00"
            end_date = "2001-Jan-01 00:00:00"

            start_dry_period = "2000-Apr-30 00:00:00"
            end_dry_period = "2000-Nov-01 00:00:00"

            start = datetime.strptime(start_date, '%Y-%b-%d %H:%M:%S')
            end = datetime.strptime(end_date, '%Y-%b-%d %H:%M:%S')

            start_dry = datetime.strptime(start_dry_period, '%Y-%b-%d %H:%M:%S')
            end_dry = datetime.strptime(end_dry_period, '%Y-%b-%d %H:%M:%S')

            day = timedelta(days=1)
            next_day = start

            scaling_factor = []
            while next_day < end:

                val = self.scaling_factor
                if next_day >= start_dry and next_day < end_dry:
                    val = 0
                next_day = next_day + day
                scaling_factor.append(val)
                #print next_day, val, next_day.month #, start_wet.month , next_day.month , end_wet.month

            for p in self.parcel:

                scaled_values = []


                floats = dm_get_double_list(p, "outdoor_demand_daily")

                for i in range(len(floats)):
                    scaled_values.append(floats[i] * scaling_factor[i])
                dm_set_double_list(p, "outdoor_demand_daily", scaled_values)

            self.parcel.finalise()
