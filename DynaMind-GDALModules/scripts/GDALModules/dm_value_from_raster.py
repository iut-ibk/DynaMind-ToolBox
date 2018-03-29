

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


            self.registerViewContainers([self.node_view])

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

            for node in self.node_view:
                geom = node.GetGeometryRef()
                point = ct.TransformPoint(geom.GetX(), geom.GetY())
                x = (point[0]-gt[0])/gt[1]
                y = (point[1]-gt[3])/gt[5]
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
                node.SetField(self.attribute_name, tuple_of_floats[0])

            self.node_view.finalise()
