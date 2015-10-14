from osgeo import ogr
import csv
from pydynamind import *

from datetime import datetime, timedelta

import pdb

class ScaleOutdoorDemand(Module):
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

            self.parcel = ViewContainer("parcel", EDGE, READ)
            self.parcel.addAttribute("outdoor_demand_daily", Attribute.DOUBLEVECTOR, MODIFY)
            #self.parcel.addAttribute("scale_pattern", Attribute.DOUBLEVECTOR, WRITE)

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


            print scaling_factor




            for p in self.parcel:

                scaled_values = []

                s = str(p.GetFieldAsString("outdoor_demand_daily"))
                floats = map(float, s.split())

                for i in range(len(floats)):
                    scaled_values.append(floats[i] * scaling_factor[i])



                p.SetField("outdoor_demand_daily", ' '.join(str(d) for d in scaled_values))
            self.parcel.finalise()



















__author__ = 'christianurich'
