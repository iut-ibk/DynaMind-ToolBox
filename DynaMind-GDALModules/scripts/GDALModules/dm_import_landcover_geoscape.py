from pydynamind import *
import gdal, osr
from gdalconst import *
import struct
import numpy as np
import compiler
import paramiko
import os
import gc


class DM_ImportLandCoverGeoscape(Module):
    display_name = "Import Landcover Geoscape"
    group_name = "Data Import and Export"

    def getHelpUrl(self):
        return "/DynaMind-GDALModules/dm_import_landuse.html"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)
        self.createParameter("view_name", STRING)
        self.view_name = "node"

        self.createParameter("view_name_grid", STRING)
        self.view_name_grid = "city"

        self.createParameter("raster_file", STRING)
        self.raster_file = ""

        self.createParameter("username", STRING)
        self.username = ""

        self.createParameter("password", STRING)
        self.password = ""

        self.createParameter("port", INT)
        self.port = 22

        self.createParameter("host", STRING)
        self.host = ""

        self.createParameter("big_raster_file", BOOL)
        self.big_raster_file = False

        self.createParameter("transform", BOOL)
        self.transform = True

        self.transport = None
        self.sftp = None

        self.downloaded_file = ""

        self.real_file_name = ""

        self.index_max  = 0

        self.geoscape_landclass = None

    def generate_downloaded_file_name(self):
        return self.raster_file + self.username + self.password + self.host

    def connect(self):
        established = False
        try:
            log(str(self.host) + " " + str(self.port) + " " + str(self.username) + " " + str(self.password), Standard)
            self.transport = paramiko.Transport((self.host, self.port))

            self.transport.connect(username=self.username, password=self.password)
            established = True
        except:
            return False

        log("connected", Standard)
        self.sftp = paramiko.SFTPClient.from_transport(self.transport)
        return True

    def close(self):
        log("close connection", Standard)
        self.sftp.close()
        self.transport.close()

        self.sftp = None
        self.transport = None

    def get_file(self, file_name):
        if self.real_file_name:
            os.remove(self.real_file_name)
        self.real_file_name = "/tmp/" + str(uuid.uuid4())
        try:
            self.sftp.get(file_name, self.real_file_name)
        except Exception as e:
            print(e)
            self.real_file_name = ""
            return False
        self.downloaded_file = self.generate_downloaded_file_name()
        return True

    def init(self):
        self.node_view = ViewContainer(self.view_name, FACE, READ)
        self.city = ViewContainer(self.view_name_grid, FACE, READ)

        if self.transform:
            self.geoscape_landclass = {
                2: 5, # grass
                3: 13,
                4: 7,
                5: 1,
                6: 7,
                7: 15,
                8: 2,
                9: 12,
                10: -1,
                11: -1,
                12: 2
            }
            self.index_max = 12
        else:
            self.geoscape_landclass = {
                1: 1,  # trees
                2: 2,
                3: 3,
                4: 4,
                5: 5,
                6: 6,
                7: 7,
                8: 8,
                9: 9,
                10: 10,
                11: 11,
                12: 12,
                13: 13,
                14: 14,
                15: 15
            }
            self.index_max = 15

        self.landuse_classes = {
            "tree_cover_fraction": 1,
            "water_fraction": 2,
            "pond_and_basin_fraction": 3,
            "wetland_fraction": 4,
            "grass_fraction": 5,
            "swale_fraction": 6,
            "irrigated_grass_fraction": 7,
            "bio_retention_fraction": 8,
            "infiltration_fraction": 9,
            "green_roof_fraction": 10,
            "green_wall_fraction": 11,
            "roof_fraction": 12,
            "road_fraction": 13,
            "porous_fraction": 14,
            "concrete_fraction": 15
        }
        for key in self.landuse_classes:
            self.node_view.addAttribute(key, Attribute.DOUBLE, WRITE)
        # self.node_view.addAttribute(key, Attribute.INT, WRITE)
        self.node_view.addAttribute("geoscape_count", Attribute.INT, WRITE)
        self.node_view.addAttribute("geoscape_missed", Attribute.INT, WRITE)



        self.registerViewContainers([self.node_view, self.city ])

    def run(self):

        if self.host:
            if not self.connect():
                log("Connection to host failed", Error)
                return
            if not self.get_file(self.raster_file):
                log("Failed to download file", Error)
                return
        else:
            self.real_file_name = self.raster_file

        # log("Hello its me", Standard)
        dataset = gdal.Open(self.real_file_name, GA_ReadOnly)
        if not dataset:
            log("Failed to open file", Error)
            self.setStatus(MOD_EXECUTION_ERROR)
            return
        band = dataset.GetRasterBand(1)
        gt = dataset.GetGeoTransform()
        srs = osr.SpatialReference()
        srs.ImportFromWkt(dataset.GetProjection())
        srsLatLong = osr.SpatialReference()
        srsLatLong.ImportFromEPSG(self.getSimulationConfig().getCoorindateSystem())
        ct = osr.CoordinateTransformation(srsLatLong, srs)


        areas = []

        for c_idx, c in enumerate(self.city):
            log(str(c.GetFID()), Standard )
            geom = c.GetGeometryRef()
            env = geom.GetEnvelope()
            p1 = ct.TransformPoint(env[0], env[2])
            p2 = ct.TransformPoint(env[1], env[3])
            minx = int((p1[0] - gt[0]) / gt[1])
            miny = int((p1[1] - gt[3]) / gt[5])
            maxx = int((p2[0] - gt[0]) / gt[1])
            maxy = int((p2[1] - gt[3]) / gt[5])

            if miny > maxy:
                min_y_tmp = miny
                miny = maxy
                maxy = min_y_tmp
                # log("swapsy", Standard)
            minx -= 100
            miny -= 100
            maxx += 100
            maxy += 100
            # print str(minx) + "/" + str(miny) + "/" + str(maxx) + "/" + str(maxy), maxx - minx, maxy - miny
            #
            # log(str(minx) + "/" + str(miny) + "/" + str(miny) + "/" + str(maxy), Standard)
            # values = band.ReadAsArray()

            # values = band.ReadAsArray(minx, miny, maxx - minx, maxy - miny)
            # print values
            # gminy = miny
            # gminx = minx
            areas.append([c.GetFID(), minx, miny, maxx - minx, maxy - miny])
            # print "start nodes"

        self.city.finalise()

        for a in areas:
            log(str(a), Standard)
            if self.big_raster_file:
                self.node_view.set_attribute_filter(self.view_name_grid + "_id = " + str(a[0]))
            else:
                self.node_view.reset_reading()
            values = band.ReadAsArray(a[1], a[2], a[3], a[4])
            # print values

            gminy = a[2]
            gminx = a[1]

            for node_idx, node in enumerate(self.node_view):
                geom = node.GetGeometryRef()
                env = geom.GetEnvelope()
                p1 = ct.TransformPoint(env[0], env[2])
                p2 = ct.TransformPoint(env[1], env[3])
                minx = int((p1[0] - gt[0]) / gt[1])
                miny = int((p1[1] - gt[3]) / gt[5])
                maxx = int((p2[0] - gt[0]) / gt[1])
                maxy = int((p2[1] - gt[3]) / gt[5])

                if miny > maxy:
                    min_y_tmp = miny
                    miny = maxy
                    maxy = min_y_tmp

                val_array = np.zeros(16)
                missed = 0
                count = 0

                for x in range(minx, maxx + 1):
                    for y in range(miny, maxy + 1):
                            if x < 0 or y < 0 or x > band.XSize - 1 or y > band.YSize - 1:
                                continue
                            try:
                                #print "y",int(y),  gminy, "x", int(x) , gminx, "val", int(values[int(y) - gminy][int(x) - gminx])
                                idx = int(values[int(y) - gminy][int(x) - gminx])

                            except IndexError:
                                missed+=1
                                continue
                            if idx < 0 or idx > self.index_max:
                                continue
                            val = self.geoscape_landclass[idx]
                            if val < 1:
                                continue

                            val_array[val] += 1
                            count+=1
                print(count, missed)
                node.SetField("geoscape_count", count)
                node.SetField("geoscape_missed", missed)
                for key in self.landuse_classes:
                    if val_array.sum() < 1:
                        continue
                    node.SetField(key, float(val_array[self.landuse_classes[key]] / val_array.sum()))

            self.node_view.sync()
            self.node_view.set_next_by_index(0)
            # gc.collect()

        self.node_view.finalise()
        print("finalise city")

