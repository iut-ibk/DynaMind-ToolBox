from pydynamind import * 
import gdal, osr 
from gdalconst import * 
import struct 
import numpy as np 
import compiler

class DM_ValueFromRasterPoly(Module):
        display_name = "Value From Raster Poly"
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
            self.createParameter("filter_statement", STRING)
            self.filter_statement = "value"
            

        def init(self):
            self.node_view = ViewContainer(self.view_name, FACE, READ)
            self.node_view.addAttribute(self.attribute_name, Attribute.DOUBLE, WRITE)
            self.registerViewContainers([self.node_view])
        def run(self):
            #log("Hello its me", Standard)
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
            ct = osr.CoordinateTransformation(srsLatLong, srs)
            inMemory = True
            if inMemory:
                values = band.ReadAsArray(0, 0, band.XSize, band.YSize)
            compiled_exp = compiler.compile(self.filter_statement, '<string>', 'eval')
            log(self.filter_statement, Standard) 
            for node in self.node_view:
                
                geom = node.GetGeometryRef()
                env = geom.GetEnvelope()
                p1 = ct.TransformPoint(env[0], env[2])
                p2 = ct.TransformPoint(env[1], env[3])
                minx = int((p1[0]-gt[0])/gt[1])
                miny = int((p1[1]-gt[3])/gt[5])
                maxx = int((p2[0]-gt[0])/gt[1])
                maxy = int((p2[1]-gt[3])/gt[5])
                #print env print gt print minx, miny, maxx, maxy
                if miny > maxy:
                    min_y_tmp = miny
                    miny = maxy
                    maxy = min_y_tmp
                #print minx, miny, maxx, maxy
                datatype = band.DataType
                sum_val = 0
                for x in range(minx, maxx+1):
                    for y in range(miny, maxy+1):
                     if inMemory:
                         if x < 0 or y < 0 or x > band.XSize -1 or y > band.YSize - 1:
                             continue
                         value =  values[int(y)][int(x)]
                         sum_val += eval(compiled_exp) 
                         
                     if not inMemory:
                         scanline = band.ReadRaster( int(x), int(y), 1, 1, 1, 1, datatype)
                         if not scanline:
                             log("No value found for " + str(x) + "," +str(y), Warning )
                             continue
                         if datatype == GDT_Int32:
                             tuple_of_floats = struct.unpack('i' * 1, scanline)
                         elif datatype == GDT_Float32:
                             tuple_of_floats = struct.unpack('f' * 1, scanline)
                         else:
                             log("Datatype " + str(datatype) + " not supported", Error)
                             self.node_view.finalise()
                             return
                         val = tuple_of_floats[0]
                node.SetField(self.attribute_name, float(sum_val))
            print("syncronise")
            self.node_view.finalise()
