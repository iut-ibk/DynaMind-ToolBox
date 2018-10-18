__author__ = 'christianurich'

from pydynamind import *
import sqlite3
import sys
import os

import numpy as np
import uuid


class TargetInegration(Module):
    display_name = "Target"
    group_name = "Performance Assessment"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)

        self.micro_climate_grid = ViewContainer("micro_climate_grid", DM.COMPONENT, DM.READ)
        self.micro_climate_grid.addAttribute("roof_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("road_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("water_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("concrete_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("tree_cover_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("grass_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("irrigated_grass_fraction", DM.Attribute.DOUBLE, DM.READ)
        self.micro_climate_grid.addAttribute("taf", DM.Attribute.DOUBLE, DM.WRITE)
        self.micro_climate_grid.addAttribute("taf_period", DM.Attribute.DOUBLEVECTOR, DM.WRITE)
        self.micro_climate_grid.addAttribute("ts", DM.Attribute.DOUBLE, DM.WRITE)
        self.micro_climate_grid.addAttribute("ucan", DM.Attribute.DOUBLE, DM.WRITE)

        self.createParameter("target_path", STRING)
        self.target_path = ""

        self.registerViewContainers([self.micro_climate_grid])

    def fixNulls(self, text):
        if (text == None):
            text = 0.0
        return round(text, 2)

    def run(self):
        sys.path.append(self.target_path + "/Toolkit2-Runs/bin/")
        import runToolkit as target
        landuse_file = "/tmp/" + str(uuid.uuid4()) + "lc.csv"
        result_file = "/tmp/" + str(uuid.uuid4()) + "output"

        self.write_input_file(landuse_file)


        # Run target
        target.run_target(
            self.target_path + "/Sunbury1Extreme/controlfiles/Sunbury1Extreme/Sunbury1Extreme.txt", landuse_file,
            self.target_path + "/Sunbury1Extreme/input/Sunbury1Extreme/MET/Sunbury1Extreme.csv", result_file)

        self.read_output(result_file)

        os.remove(result_file+ ".npy")
        os.remove(landuse_file )

    def read_output(self, result_file):
        img_array = np.load(result_file + ".npy")
        l1, l2, l3 = img_array.shape
        DTE_ITEM = 8
        QH_ITEM = 7
        QE_ITEM = 6
        QG_ITEM = 5
        RN_ITEM = 4
        TAF_ITEM = 3
        TS_ITEM = 2
        UCAN_ITEM = 1
        time = 29
        n = l2
        Z = np.zeros((n))
        UCAN = np.zeros((n))
        UCAN[:] = np.NAN
        TS = np.zeros((n))
        TS[:] = np.NAN
        TAF = np.zeros((n))
        TAF[:] = np.NAN
        RN = np.zeros((n))
        RN[:] = np.NAN
        QG = np.zeros((n))
        QG[:] = np.NAN
        QE = np.zeros((n))
        QE[:] = np.NAN
        QH = np.zeros((n))
        QH[:] = np.NAN
        # DTE=np.zeros((n))
        TAF[:] = np.NAN

        taf_vec = []
        for count in range(n):
            taf_vec.append(np.zeros(l1))
        for t in range(l1):
            for count in range(n):
                # if (count == n-1):
                #  count = n-2
                # print (i,j,count)
                Z[count] = img_array[time, count, 0][UCAN_ITEM]
                UCAN[count] = img_array[time, count, 0][UCAN_ITEM]
                TS[count] = img_array[time, count, 0][TS_ITEM]
                TAF[count] = img_array[time, count, 0][TAF_ITEM]
                RN[count] = img_array[time, count, 0][RN_ITEM]
                QG[count] = img_array[time, count, 0][QG_ITEM]
                QE[count] = img_array[time, count, 0][QE_ITEM]
                QH[count] = img_array[time, count, 0][QH_ITEM]

                taf_vec[count][t] = img_array[t, count, 0][TAF_ITEM]
                if TAF[count] > 50:
                    TAF[count] = np.nan

        self.micro_climate_grid.reset_reading()

        for (idx, m) in enumerate(self.micro_climate_grid):
            m.SetField("taf", taf_vec[idx].max())
            m.SetField("ts", TS[idx])
            m.SetField("ucan", UCAN[idx])
            dm_set_double_list(m, "taf_period", taf_vec[idx])
        self.micro_climate_grid.finalise()

    def write_input_file(self, landuse_file):
        text_file = open(landuse_file, "w")
        text_file.write("FID,roof,road,watr,conc,Veg,dry,irr,H,W" + '\n')
        for m in self.micro_climate_grid:

            # for high rise H/W is > 2
            # for compact high rise H/W = 0.75-2
            # compact low rise 0.75-1.5
            #
            # probably bush/scrub, so H/W = 0.25-1.0
            # and open low rise H/W 0.3-0.75

            ogc_fid = self.fixNulls(m.GetFID())
            roof_fraction = self.fixNulls(m.GetFieldAsDouble("roof_fraction"))
            road_fraction = self.fixNulls(m.GetFieldAsDouble("road_fraction"))
            water_fraction = self.fixNulls(m.GetFieldAsDouble("water_fraction"))
            concrete_fraction = self.fixNulls(m.GetFieldAsDouble("concrete_fraction"))
            tree_cover_fraction = self.fixNulls(m.GetFieldAsDouble("tree_cover_fraction"))
            grass_fraction = self.fixNulls(m.GetFieldAsDouble("grass_fraction"))
            irrigated_grass_fraction = self.fixNulls(m.GetFieldAsDouble("irrigated_grass_fraction"))

            H = '0.5'
            W = '1.0'

            if (roof_fraction > 0.3):
                H = '1'
                W = '1'
            else:
                if (grass_fraction + irrigated_grass_fraction > 0.5):
                    H = '0.3'
                    W = '1.0'

            # row_text =fixNulls(row[0]) + ',' + fixNulls(row[1]) + ',' + fixNulls(row[2])+ ',' + fixNulls(row[3])+ ',' + fixNulls(row[4])+ ',' + fixNulls(row[5])+ ',' + fixNulls(row[6])+ ',' + fixNulls(row[7]) +  ',' + H +  ',' + W + '\n'
            row_text = str(ogc_fid) + ',' + str(roof_fraction) + ',' + str(road_fraction) + ',' + str(
                water_fraction) + ',' + str(concrete_fraction) + ',' + str(tree_cover_fraction) + ',' + str(
                grass_fraction) + ',' + str(irrigated_grass_fraction) + ',' + H + ',' + W + '\n'
            text_file.write(row_text)
        text_file.close()
        self.micro_climate_grid.finalise()

