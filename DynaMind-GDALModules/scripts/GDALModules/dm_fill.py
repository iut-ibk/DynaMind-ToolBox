from pydynamind import *
import gdal, osr
from gdalconst import *
import struct
import numpy as np
import compiler
import paramiko
import os
import richdem as rd


class DM_Fill(Module):
    display_name = "Min Value"
    group_name = "Data Import and Export"

    def getHelpUrl(self):
        return "/DynaMind-GDALModules/dm_import_landuse.html"

    def __init__(self):
        Module.__init__(self)
        self.setIsGDALModule(True)
        self.createParameter("view_name", STRING)
        self.view_name = "node"

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

        self.transport = None
        self.sftp = None

        self.downloaded_file = ""

        self.real_file_name = ""

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
        self.city = ViewContainer("city", FACE, READ)

        self.node_view.addAttribute("elevation", Attribute.DOUBLE, WRITE)


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
            self.real_file_name = self.getSimulationConfig().getWorkingDir() + "/" + self.raster_file

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
        inMemory = True
        if inMemory:
            for c in self.city:
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
                    log("swapsy", Standard)
                minx -= 50
                miny -= 50
                maxx += 50
                maxy += 50
                print(str(minx) + "/" + str(miny) + "/" + str(maxx) + "/" + str(maxy), maxx - minx, maxy - miny)
                log(str(minx) + "/" + str(miny) + "/" + str(miny) + "/" + str(maxy), Standard)

                values = band.ReadAsArray(minx, miny, maxx - minx, maxy - miny)
                gminy = miny
                gminx = minx

                if not band.GetNoDataValue():
                    no_data_value = 9999
                else:
                    no_data_value = band.GetNoDataValue()

                rda = rd.rdarray(values, no_data=no_data_value)
                rda.projection = dataset.GetProjectionRef()
                rda.geotransform = gt
                rd.FillDepressions(rda, in_place=True)

        for node in self.node_view:
            geom = node.GetGeometryRef()
            env = geom.GetEnvelope()
            p1 = ct.TransformPoint(env[0], env[2])
            p2 = ct.TransformPoint(env[1], env[3])
            minx = int((p1[0] - gt[0]) / gt[1])
            miny = int((p1[1] - gt[3]) / gt[5])
            maxx = int((p2[0] - gt[0]) / gt[1])
            maxy = int((p2[1] - gt[3]) / gt[5])
            # print env print gt print minx, miny, maxx, maxy
            if miny > maxy:
                min_y_tmp = miny
                miny = maxy
                maxy = min_y_tmp
            # print minx, miny, maxx, maxy

            min_val = None

            for x in range(minx, maxx + 1):
                for y in range(miny, maxy + 1):
                    if inMemory:
                        if x < 0 or y < 0 or x > band.XSize - 1 or y > band.YSize - 1:
                            continue
                        # print y, gminy, x, gminx
                        # print int(y) - gminy, int(x) - gminx
                        val = rda[int(y) - gminy][int(x) - gminx]

                        if val < -0.01:
                            continue

                        if not min_val:
                            min_val = val
                            continue

                        if min_val > val:
                            min_val = val

            if min_val:
                node.SetField("elevation", float(min_val))
        print("syncronise")
        self.node_view.finalise()
        self.city.finalise()
