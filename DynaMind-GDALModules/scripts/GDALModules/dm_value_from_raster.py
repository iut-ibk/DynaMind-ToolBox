

from pydynamind import *
import gdal, osr
from gdalconst import *
import struct

class DM_ValueFromRaster(Module):

        display_name = "Value From Raster"
        group_name = "Network Generation"

        def getHelpUrl(self):
            return "/DynaMind-GDALModules/dm_value_from_raster.html"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("view_name", STRING)
            self.view_name = "node"

            self.createParameter("attribute_name", STRING)
            self.attribute_name = "value"

            self.createParameter("raster_file", FILENAME)
            self.raster_file = ""

        def init(self):
            self.node_view = ViewContainer(self.view_name, NODE, READ)
            self.node_view.addAttribute(self.attribute_name, Attribute.DOUBLE, WRITE)
            self.catchment_view = ViewContainer("city", FACE, READ)


            self.registerViewContainers([self.node_view, self.catchment_view])

        def run(self):
            dataset = gdal.Open( self.raster_file, GA_ReadOnly)

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

            #(353136,5776456)
            ct = osr.CoordinateTransformation(srsLatLong, srs)
            inMemory = True
            if inMemory:
                for c in self.catchment_view:
                    geom = c.GetGeometryRef()
                    env = geom.GetEnvelope()
                    p1 = ct.TransformPoint(env[0], env[2])
                    p2 = ct.TransformPoint(env[1], env[3])
                    minx = int((p1[0]-gt[0])/gt[1])
                    miny = int((p1[1]-gt[3])/gt[5])
                    maxx = int((p2[0]-gt[0])/gt[1])
                    maxy = int((p2[1]-gt[3])/gt[5])

                    if miny > maxy:
                        min_y_tmp = miny
                        miny = maxy
                        maxy = min_y_tmp
                        log("swapsy", Standard)
                    minx -= 5
                    miny -= 5
                    maxx += 5
                    maxy += 5
                    print str(minx) + "/" + str(miny)+ "/" + str(maxx)+ "/" + str(maxy), maxx-minx, maxy-miny
                    log(str(minx) + "/" + str(miny)+ "/" + str(miny)+ "/" + str(maxy), Standard)

                    values = band.ReadAsArray(minx, miny, maxx-minx, maxy-miny)



            for node in self.node_view:
                geom = node.GetGeometryRef()
                point = ct.TransformPoint(geom.GetX(), geom.GetY())
                x = (point[0]-gt[0])/gt[1]
                y = (point[1]-gt[3])/gt[5]

                if inMemory:
                    val = values[int(y)-miny][int(x)-minx]

                #print val.item()
                    node.SetField(self.attribute_name, val.item())
                else
                    datatype = band.DataType

                    scanline = band.ReadRaster( int(x), int(y), 1, 1, 1, 1, datatype)
                    if not scanline:
                        log("No value found for " + str(x) + "," + str(y), Warning)
                        continue

                    if datatype == GDT_Int32:
                        tuple_of_floats = struct.unpack('i' * 1, scanline)
                    elif datatype == GDT_Float32:
                        tuple_of_floats = struct.unpack('f' * 1, scanline)
                    else:
                        log("Datatype " + str(datatype) + " not supported", Error)
                        self.node_view.finalise()
                        return


            self.node_view.finalise()
            self.catchment_view.finalise()
