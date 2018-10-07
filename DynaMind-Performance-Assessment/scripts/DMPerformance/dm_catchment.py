#!/usr/bin/env python
# -*- coding: utf-8 -*- 

__author__ = 'christianurich'

import swmmread
import numpy as np
import math
from ctypes import *
from sys import platform as _platform
import pydynamind
import ogr

import uuid
import os

import datetime

from pydynamind import *

from pyswmm import Simulation, Subcatchments, Nodes


def result_dict(elements, data_dict, id_name):
    catchment_r = {}
    catchments = []
    for e in elements:
        result = {}
        for cd in data_dict.keys():
            result[cd] = 0
        catchment_r[getattr(e, id_name)] = result
        catchments.append(e)
    return catchments, catchment_r


class DMCatchment(Module):
    display_name = "WSUD Catchment"
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

        self.createParameter("view_name", DM.STRING)
        self.view_name = "city"

        self.createParameter("view_name_catchment", DM.STRING)
        self.view_name_catchment = "city"

    def init(self):
        self.view_catchments = ViewContainer(self.view_name_catchment, DM.COMPONENT, DM.READ)
        self.view_catchments.addAttribute("area", DM.Attribute.DOUBLE, DM.READ)
        self.view_catchments.addAttribute("impervious_fraction", DM.Attribute.DOUBLE, DM.READ)

        self.view_catchments.addAttribute("runoff", DM.Attribute.DOUBLE, DM.WRITE)
        self.view_catchments.addAttribute("runoff_treated", DM.Attribute.DOUBLE, DM.WRITE)
        self.view_catchments.addAttribute("peak_flows", DM.Attribute.DOUBLEVECTOR, DM.WRITE)

        self.rwht = None
        self.tree_pit = None

        view_register = [self.view_catchments]

        if self.rain_vector_from_city != "":
            self.city = ViewContainer(self.view_name, DM.COMPONENT, DM.READ)
            self.city.addAttribute(self.rain_vector_from_city, DM.Attribute.DOUBLEVECTOR, DM.READ)


        if self.isViewInStream("city", "rwht"):
            self.rwht = ViewContainer("rwht", DM.COMPONENT, DM.READ)
            self.rwht.addAttribute("volume", DM.Attribute.DOUBLE, DM.READ)
            self.rwht.addAttribute("connected_area", DM.Attribute.DOUBLE, DM.READ)
            view_register.append(self.rwht)

        if self.isViewInStream("city", "tree_pit"):
            self.tree_pit = ViewContainer("tree_pit", DM.COMPONENT, DM.READ)
            self.view_catchments.addAttribute("tree_pit_area", DM.Attribute.DOUBLE, DM.READ)
            self.view_catchments.addAttribute("tree_pit_storage_depth", DM.Attribute.DOUBLE, DM.READ)
            view_register.append(self.tree_pit)

        view_register.append(self.city)
        self.registerViewContainers(view_register)

    def SEI(self, catchment, tree_pit_storage_depth):
        filename = str(uuid.uuid4())

        fo = open("/tmp/" + filename + ".inp", 'w')
        intervall = (datetime.datetime(2000, 1, 1) + datetime.timedelta(seconds=self.time_delta)).strftime('%H:%M:%S')
        dt = datetime.timedelta(seconds=self.time_delta)

        swmm_rain_filename = self.swmm_rain_file
        if self.rain_vector_from_city != "":
            self.city.reset_reading()
            for c in self.city:
                import ogr
                self.write_rain_file(filename, dm_get_double_list(c, self.rain_vector_from_city), 60 * 6)
                swmm_rain_filename = "/tmp/" + filename + ".dat"
        self.init_swmm_model(fo, rainfile=swmm_rain_filename, start='01/01/2000', stop='12/30/2000',
                             intervall=intervall,
                             sub_satchment=catchment,
                             tree_pit_storage_depth = tree_pit_storage_depth)
        fo.close()

        results = {}
        with Simulation("/tmp/" + filename + ".inp") as sim:
            s = 360.
            sim.step_advance(s)

            # catchment
            catchment_data = {
                "rainfall": s / (60 * 60),
                "runoff": s
            }

            catchments, catchment_r = result_dict(Subcatchments(sim), catchment_data, "subcatchmentid")
            results['catchment'] = [catchments, catchment_r, catchment_data, "subcatchmentid"]

            # nodes
            node_data = {
                "total_inflow": s
            }

            catchments, catchment_r = result_dict(Nodes(sim), node_data, "nodeid")
            results['nodes'] = [catchments, catchment_r, node_data, "nodeid"]

            catch = Subcatchments(sim)["1"]
            peak_flows = []
            max_runoff = 0
            for idx, step in enumerate(sim):
                if (idx+1) * s % (60*60*40) == 0:
                    if idx > 0:
                        peak_flows.append(max_runoff)
                    max_runoff = 0
                if catch.runoff > max_runoff:
                    max_runoff = catch.runoff
                for k in results.keys():
                    r = results[k]
                    for c in r[0]:
                        id = getattr(c, r[3])
                        for c_r in r[1][id].keys():
                            r[1][id][c_r] += getattr(c, c_r) * r[2][c_r]

                # sim.report()
            peak_flows.append(max_runoff)
            # sim.report()
        results["peak_flows"] = peak_flows


        tree_pit_inflow = self.getTreePitInflow("/tmp/" + filename + ".rpt")
        results["tree_pit_inflow"] = tree_pit_inflow
        os.remove("/tmp/" + filename + ".inp")


        os.remove("/tmp/" + filename + ".rpt")
        os.remove("/tmp/" + filename + ".out")
        if self.rain_vector_from_city != "":
            os.remove("/tmp/" + filename + ".dat")
        return results

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
        # print vector
        dt = datetime.timedelta(seconds=timestep)
        start = datetime.datetime(2000, 1, 1)
        with open("/tmp/" + filename + ".dat", 'w') as f:
            for val in vector:
                # STA01 2000 1 1 01 00 0.496684
                f.write("STA01" + ' ' + start.strftime('%Y %m %d').replace(' 0', ' ') + ' ' + start.strftime(
                    '%H %M') + ' ' + str(val) + '\n')
                start += dt
        f.close()
    def write_barrel(self, out_file):
        if not self.rwht:
            return
        self.rwht.reset_reading()
        for r in self.rwht:
            out_file.write('barrel' + r.GetFID() + '           RB\n')
            out_file.write('barrel' + r.GetFID() + '          STORAGE    2000       0.75       0.5        0  \n')
            out_file.write('barrel' + r.GetFID() + '         DRAIN      20.        0          0          24  \n')

    def init_swmm_model(self, out_file, rainfile, start, stop, intervall, sub_satchment, tree_pit_storage_depth):
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
            out_file.write(c + '               RG1              ' + 'o' + c + '                ' + str(
                sub_satchment[c]["area"]) + '        ' + str(sub_satchment[c]["imp"]) + '       ' + str(
                math.sqrt(sub_satchment[c]["area"]) * 10000) + '      0.1      0\n')

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
        out_file.write(';;Name           Elevation  MaxDepth   InitDepth  SurDepth   Aponded\n')
        out_file.write(';;-------------- ---------- ---------- ---------- ---------- ---------\n')
        for c in sub_satchment.keys():
            out_file.write('o' + c + '                 0        FREE       NO          \n')
        for c in sub_satchment.keys():
            out_file.write('n' + c + '                 0        FREE       NO         \n')
        for c in sub_satchment.keys():
            out_file.write('r' + c + '                 0        FREE       NO         \n')

        out_file.write('\n')
        out_file.write('[LID_CONTROLS]\n')
        out_file.write(';;Name           Type/Layer Parameters\n')
        out_file.write(';;-------------- ---------- ----------\n')
        out_file.write('barrel           RB\n')
        out_file.write('barrel           STORAGE    2000       0.75       0.5        0  \n')
        out_file.write('barrel           DRAIN      20.        0          0          24  \n')
        out_file.write('bc              BC\n')
        out_file.write('bc              SURFACE    300        0.15       0.24       0.5        5   \n')
        out_file.write('bc              SOIL       500        0.5        0.2        0.1        0.5        10.0       3.5\n')
        out_file.write('bc              STORAGE    200        0.75       0.5        0      \n')
        out_file.write('bc              DRAIN      200        0         0          0     \n')

        out_file.write('tree_pit              BC\n')
        out_file.write('tree_pit              SURFACE    ' + str(tree_pit_storage_depth) + '       0.15       0.24       0.5        5   \n')
        out_file.write('tree_pit              SOIL       500        0.5        0.2        0.1        5.0       10.0       3.5\n')
        out_file.write('tree_pit              STORAGE    200        0.75       0.5        0      \n')
        out_file.write('tree_pit              DRAIN      200        0         0          0     \n')

        out_file.write('\n')
        out_file.write('[LID_USAGE]\n')
        out_file.write(
            ';;Subcatchment   LID Process      Number  Area       Width      InitSat    FromImp    ToPerv     RptFile                  DrainTo    \n')
        out_file.write(
            ';;-------------- ---------------- ------- ---------- ---------- ---------- ---------- ---------- ------------------------ ----------------\n')
        for c in sub_satchment.keys():
            if 'rwht' in sub_satchment[c]:
                out_file.write(c + '                barrel           ' + str(
                    sub_satchment[c]["rwht"]["number"]) + '   1          0.5        0          ' + str(
                    sub_satchment[c]["rwht"][
                        "connected_imp_fraction"]) + '        0          *                         r' + c + '     \n')
            if 'bc' in sub_satchment[c]:
                out_file.write(c + '                bc           ' + str(
                    sub_satchment[c]["bc"]["number"]) + '   25          1        0          ' + str(
                    sub_satchment[c]["bc"][
                        "connected_imp_fraction"]) + '        0          *                        n' + c + '     \n')
            if 'tree_pits' in sub_satchment[c]:
                out_file.write(c + '                tree_pit           ' + str(
                    sub_satchment[c]["tree_pits"]["number"]) + '   ' + str(sub_satchment[c]["tree_pits"]["tree_pit_area"])  +'           2        0          ' + str(
                    sub_satchment[c]["tree_pits"][
                        "connected_imp_fraction"]) + '        0          *                       n' + c + '     \n')


        out_file.write('\n')
        out_file.write('[REPORT]\n')
        out_file.write('INPUT      NO\n')
        out_file.write('CONTROLS   NO\n')
        out_file.write('CONTINUITY YES\n')
        out_file.write('FLOWSTATS YES\n')
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

    def getTreePitInflow(self, rptfile):
        inflow = 0
        with open(rptfile) as f:
            for l in f:
                if "tree_pit" in l:
                    inflow = float(l.split()[2])
        return inflow



    def getRainVec(self):
        return [0.5236, 1.2614, 0.7378, 1.1662, 2.2848, 1.2376, 4.284, 2.9512, 1.3328, 0.7378, 0.7854, 0.9996, 1.0234,
                0.4998, 0.5236, 0.8092, 0.4522, 0.2856, 0.238, 0.5474, 0.2142, 0.3094, 0.2618, 0.3332]

    def run(self):
        self.view_catchments.reset_reading()

        connected_area = 0
        number_rwht = 0
        connected_area_tree = 0
        number_tree = 0
        if self.rwht:
            for r in self.rwht:
                connected_area += r.GetFieldAsDouble("connected_area")
                number_rwht += 1

        if self.tree_pit:
            for r in self.tree_pit:
                connected_area_tree += r.GetFieldAsDouble("connected_area")
                number_tree += 1


        for c in self.view_catchments:

            area = c.GetFieldAsDouble("area") / 10000.  # Convert area to ha for SWMM
            imp = c.GetFieldAsDouble("impervious_fraction") * 100.  # Convert to %


            #
            # natural = self.SEI({"1": {"id": 1, "area": area, "imp": 0,
            #                           "rwht": {"number": 0, "connected_imp_fraction": 0},
            #                           "bc": {"number": 0, "connected_imp_fraction": 0}}})
            #
            # urbanised = self.SEI({"1": {"id": 1, "area": area, "imp": imp,
            #                             "rwht": {"number": 0, "connected_imp_fraction": 0},
            #                             "bc": {"number": 0, "connected_imp_fraction": 0}}})
            tree_pit_area = 2.5
            tree_pit_storage_depth = 150
            if self.tree_pit:
                tree_pit_area = c.GetFieldAsDouble("tree_pit_area")
                tree_pit_storage_depth = c.GetFieldAsDouble("tree_pit_storage_depth")



            wsud = self.SEI({"1": {"id": 1, "area": area, "imp": imp,
                                   "rwht": {"number": number_rwht,
                                            "connected_imp_fraction": connected_area / (imp/100 * area  *10000.) * 100},
                                   "tree_pits": {"number": number_tree,
                                            "connected_imp_fraction": connected_area_tree / (imp/100 * area  *10000.) * 100,  "tree_pit_area": tree_pit_area}

                                   }}, tree_pit_storage_depth)
            #print wsud
            c.SetField("runoff", wsud['catchment'][1]['1']['runoff'] + wsud['nodes'][1]['n1']['total_inflow'])
            c.SetField("runoff_treated", wsud['nodes'][1]['n1']['total_inflow'])
            pydynamind.dm_set_double_list(c, "peak_flows", wsud["peak_flows"])





        self.view_catchments.finalise()
        if self.rain_vector_from_city != "":
            self.city.finalise()
        if self.rwht:
            self.rwht.finalise()
        if self.tree_pit:
            self.tree_pit.finalise()

