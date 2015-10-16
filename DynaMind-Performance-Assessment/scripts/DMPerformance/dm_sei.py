__author__ = 'christianurich'

import swmmread
import numpy as np
import math
from ctypes import *

import ogr

import uuid
import os

import datetime

from pydynamind import *


class DMSEI(Module):
    display_name = "Stream Erosion Index (AU)"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.createParameter("swmm_rain_file", DM.FILENAME)
        self.swmm_rain_file = ""

        self.createParameter("time_delta", DM.INT)
        self.time_delta = 360

        self.createParameter("rain_vector_from_city", DM.STRING)
        self.rain_vector_from_city = ""

    def init(self):
        self.view_catchments = ViewContainer("catchment", DM.COMPONENT, DM.READ)
        self.view_catchments.addAttribute("area", DM.Attribute.DOUBLE, DM.READ)
        self.view_catchments.addAttribute("impervious_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.view_catchments.addAttribute("stream_erosion_index", DM.Attribute.DOUBLE, DM.WRITE)

        view_register = [self.view_catchments]

        if self.rain_vector_from_city != "":
            self.city = ViewContainer("city", DM.COMPONENT, DM.READ)
            self.city.addAttribute(self.rain_vector_from_city, DM.Attribute.DOUBLEVECTOR, DM.READ)
            view_register.append(self.city)

        self.registerViewContainers(view_register)


    def peakflow(self, catchment):
        filename = str(uuid.uuid4())
        self.write_rain_file(filename, self.getRainVec(), 60 * 5)

        fo = open("/tmp/" + filename + ".inp", 'w')
        self.init_swmm_model(fo, rainfile='/tmp/'+filename+'.dat', start='01/01/2000', stop='01/02/2000',
                             intervall='00:05:00', sub_satchment=catchment)
        fo.close()

        swmm = cdll.LoadLibrary(self.getSWMMLib())
        if swmm.swmm_run("/tmp/" + filename + ".inp", "/tmp/" + filename + ".rep", "/tmp/" + filename + ".out") != 0:
            print "something went wrong"
            return -1

        f = swmmread.open("/tmp/" + filename + ".out")
        natural_peaks = {}
        for c in catchment.keys():
            timeseries = f.get_values('subcatchments', c)
            a = np.array([row[1][4] for row in timeseries])
            natural_peaks[c] = a.max()

        os.remove("/tmp/" + filename + ".inp")
        os.remove("/tmp/" + filename + ".rep")
        os.remove("/tmp/" + filename + ".out")
        os.remove("/tmp/" + filename + ".dat")

        return natural_peaks

    def SEI(self, catchment, thresholds):
        filename = str(uuid.uuid4())

        fo = open("/tmp/" + filename + ".inp", 'w')
        intervall = (datetime.datetime(2000, 1, 1) + datetime.timedelta(seconds=self.time_delta)).strftime('%H:%M:%S')
        dt = datetime.timedelta(seconds=self.time_delta)

        swmm_rain_filename = self.swmm_rain_file
        if self.rain_vector_from_city != "":
            self.city.reset_reading()
            for c in self.city:
                s = str(c.GetFieldAsString(self.rain_vector_from_city))
                self.write_rain_file(filename, map(float, s.split()), 60 * 5)
                swmm_rain_filename = "/tmp/" + filename + ".dat"
        self.init_swmm_model(fo, rainfile=swmm_rain_filename, start='01/01/2000', stop='12/30/2009', intervall=intervall,
                             sub_satchment=catchment)
        fo.close()

        swmm = cdll.LoadLibrary(self.getSWMMLib())
        e_code = swmm.swmm_run("/tmp/" + filename + ".inp", "/tmp/" + filename + ".rep", "/tmp/" + filename + ".out")
        if e_code != 0:
            print e_code, "something went wrong"
            return -1
        f = swmmread.open("/tmp/" + filename + ".out")
        SEIs = {}
        for c in catchment.keys():
            timeseries = f.get_values('subcatchments', c)
            sum = 0
            threshold = thresholds[c] / 2.0
            for row in timeseries:
                if row[1][4] > threshold:
                    sum = sum + row[1][4]
            # print c, sum

            SEIs[c] = sum

        os.remove("/tmp/" + filename + ".inp")
        os.remove("/tmp/" + filename + ".rep")
        os.remove("/tmp/" + filename + ".out")
        if self.rain_vector_from_city != "":
            os.remove("/tmp/" + filename + ".dat")
        return SEIs

    def transform_rain_file(self):
        fo = open("/tmp/test.dat", 'w')
        counter = 0
        with open("/Users/christianurich/Documents/test_stuff/melb_rain.ixx", "rU") as f:
            for line in f:
                l = line.split('     ')
                # STA01 2000 1 1 01 00 0.496684
                d = datetime.datetime.strptime(l[0], '%d.%m.%Y %H:%M:%S')
                val = float(l[1])
                fo.write(
                    "STA01" + ' ' + d.strftime('%Y %m %d').replace(' 0', ' ') + ' ' + d.strftime('%H %M') + ' ' + str(
                        val) + '\n')
        fo.close()

    def write_rain_file(self, filename, vector, timestep):
        dt = datetime.timedelta(seconds=timestep)
        start = datetime.datetime(2000, 1, 1)
        with open("/tmp/" + filename + ".dat", 'w') as f:
            for val in vector:
                # STA01 2000 1 1 01 00 0.496684
                f.write("STA01" + ' ' + start.strftime('%Y %m %d').replace(' 0', ' ') + ' ' + start.strftime('%H %M') + ' ' + str(val) + '\n')
                start += dt
        f.close()

    def init_swmm_model(self, out_file, rainfile, start, stop, intervall, sub_satchment):
        out_file.write('[TITLE]\n')
        out_file.write('\n')
        out_file.write('[OPTIONS]\n')
        out_file.write('FLOW_UNITS           CMS\n')
        out_file.write('INFILTRATION         HORTON\n')
        out_file.write('FLOW_ROUTING         KINWAVE\n')
        out_file.write('START_DATE           ' + start + '\n')
        out_file.write('START_TIME           00:00:00\n')
        out_file.write('REPORT_START_DATE    01/01/2000\n')
        out_file.write('REPORT_START_TIME    00:00:00\n')
        out_file.write('END_DATE             ' + stop + '\n')
        out_file.write('END_TIME             06:00:00\n')
        out_file.write('SWEEP_START          01/01\n')
        out_file.write('SWEEP_END            12/31\n')
        out_file.write('DRY_DAYS             0\n')
        out_file.write('REPORT_STEP          ' + intervall + '\n')
        out_file.write('WET_STEP             ' + intervall + '\n')
        out_file.write('DRY_STEP             01:00:00\n')
        out_file.write('ROUTING_STEP         0:00:30\n')
        out_file.write('ALLOW_PONDING        NO\n')
        out_file.write('INERTIAL_DAMPING     PARTIAL\n')
        out_file.write('VARIABLE_STEP        0.75\n')
        out_file.write('LENGTHENING_STEP     0\n')
        out_file.write('MIN_SURFAREA         0\n')
        out_file.write('NORMAL_FLOW_LIMITED  BOTH\n')
        out_file.write('SKIP_STEADY_STATE    NO\n')
        out_file.write('FORCE_MAIN_EQUATION  H-W\n')
        out_file.write('LINK_OFFSETS         DEPTH\n')
        out_file.write('MIN_SLOPE            0\n')
        out_file.write('\n')
        out_file.write('[EVAPORATION]\n')
        out_file.write(';;Type       Parameters\n')
        out_file.write(';;---------- ----------\n')
        out_file.write('CONSTANT     2.0\n')
        out_file.write('DRY_ONLY     NO\n')
        out_file.write('\n')
        out_file.write('[RAINGAGES]\n')
        out_file.write(';;               Rain      Time   Snow   Data\n')
        out_file.write(';;Name           Type      Intrvl Catch  Source\n')
        out_file.write(';;-------------- --------- ------ ------ ----------\n')
        out_file.write('RG1              VOLUME    0:06   1.0    FILE       "' + rainfile + '"       STA01      MM\n')
        out_file.write('\n')
        out_file.write('[SUBCATCHMENTS]\n')
        out_file.write(
            ';;                                                 Total    Pcnt.             Pcnt.    Curb     Snow\n')
        out_file.write(
            ';;Name           Raingage         Outlet           Area     Imperv   Width    Slope    Length   Pack\n')
        out_file.write(
            ';;-------------- ---------------- ---------------- -------- -------- -------- -------- -------- --------\n')
        for c in sub_satchment.keys():
            out_file.write(c + '               RG1              ' + c + '                ' + str(
                sub_satchment[c]["area"]) + '        ' + str(sub_satchment[c]["imp"]) + '       ' + str(
                math.sqrt(sub_satchment[c]["area"]) * 100) + '      0.5      0\n')

        out_file.write('\n')
        out_file.write('[SUBAREAS]\n')
        out_file.write(';;Subcatchment   N-Imperv   N-Perv     S-Imperv   S-Perv     PctZero    RouteTo    PctRouted\n')
        out_file.write(
            ';;-------------- ---------- ---------- ---------- ---------- ---------- ---------- ----------\n')
        for c in sub_satchment.keys():
            out_file.write(c + '                0.01       0.2        2.0       15       25         OUTLET\n')
        out_file.write('\n')
        out_file.write('[INFILTRATION]\n')
        out_file.write(';;Subcatchment   MaxRate    MinRate    Decay      DryTime    MaxInfil\n')
        out_file.write(';;-------------- ---------- ---------- ---------- ---------- ----------\n')
        for c in sub_satchment.keys():
            out_file.write(c + '                 3.0        0.5        4          7          0\n')
        out_file.write('\n')
        out_file.write('[OUTFALLS]\n')
        out_file.write(';;               Invert     Outfall    Stage/Table      Tide\n')
        out_file.write(';;Name           Elev.      Type       Time Series      Gate\n')
        out_file.write(';;-------------- ---------- ---------- ---------------- ----\n')
        # out_file.write('4                0          FREE                        NO\n')
        out_file.write('\n')
        out_file.write('[REPORT]\n')
        out_file.write('INPUT      NO\n')
        out_file.write('CONTROLS   NO\n')
        out_file.write('SUBCATCHMENTS ALL\n')
        out_file.write('NODES ALL\n')
        out_file.write('LINKS ALL\n')
        out_file.write('\n')
        out_file.write('[TAGS]\n')
        out_file.write('\n')
        out_file.write('[MAP]\n')
        out_file.write('DIMENSIONS 0.000 0.000 10000.000 10000.000\n')
        out_file.write('Units      Meters\n')
        out_file.write('\n')
        out_file.write('[COORDINATES]\n')
        out_file.write(';;Node           X-Coord            Y-Coord\n')
        out_file.write(';;-------------- ------------------ ------------------\n')
        out_file.write('4                2626.582           3607.595\n')
        out_file.write('\n')
        out_file.write('[VERTICES]\n')
        out_file.write(';;Link           X-Coord            Y-Coord\n')
        out_file.write(';;-------------- ------------------ ------------------\n')
        out_file.write('\n')
        out_file.write('[Polygons]\n')
        out_file.write(';;Subcatchment   X-Coord            Y-Coord\n')
        out_file.write(';;-------------- ------------------ ------------------\n')
        out_file.write('1                3904.382           4721.116\n')
        out_file.write('1                3904.382           4721.116\n')
        out_file.write('1                9143.426           4143.426\n')
        out_file.write('1                4840.637           7988.048\n')
        out_file.write('1                2749.004           4780.876\n')
        out_file.write('\n')
        out_file.write('[SYMBOLS]\n')
        out_file.write(';;Gage           X-Coord            Y-Coord\n')
        out_file.write(';;-------------- ------------------ ------------------\n')

    def getSWMMLib(self):
        sc = self.getSimulationConfig()
        return sc.getDefaultModulePath() + "/Modules/libswmm5.dylib"

    def getRainVec(self):
        return [0.5236, 1.2614, 0.7378, 1.1662, 2.2848, 1.2376, 4.284, 2.9512, 1.3328, 0.7378, 0.7854, 0.9996, 1.0234,
                0.4998, 0.5236, 0.8092, 0.4522, 0.2856, 0.238, 0.5474, 0.2142, 0.3094, 0.2618, 0.3332]

    def run(self):
        self.view_catchments.reset_reading()
        for c in self.view_catchments:
            area = c.GetFieldAsDouble("area") / 10000. # Convert area to ha for SWMM
            imp = c.GetFieldAsDouble("impervious_fraction") * 100. # Convert to %
            peak_flows = self.peakflow({"1": {"id": 1, "area": area, "imp": 0}})
            log(str(peak_flows), Standard)

            SEIs_0 = self.SEI({"1": {"id": 1, "area": area, "imp": 0}}, peak_flows)
            log(str(SEIs_0), Standard)
            #
            # SEIs = self.SEI({"1": {"id": 1, "area": area, "imp": 0}}, peak_flows)
            # for i in SEIs.keys():
            #     print SEIs[i], SEIs[i] / SEIs_0[i]
            #
            SEIs = self.SEI({"1": {"id": 1, "area": area, "imp": imp}}, peak_flows)
            for i in SEIs.keys():
                stream_index = SEIs[i] / SEIs_0[i]
                c.SetField("stream_erosion_index", stream_index)
                log(str(stream_index), Standard)
            # SEIs = self.SEI({"1": {"id": 1, "area": area, "imp": 50}}, peak_flows)
            # for i in SEIs.keys():
            #     print SEIs[i], SEIs[i] / SEIs_0[i]
            # SEIs = self.SEI({"1": {"id": 1, "area": area, "imp": 75}}, peak_flows)
            # for i in SEIs.keys():
            #     print SEIs[i], SEIs[i] / SEIs_0[i]
        self.view_catchments.finalise()
