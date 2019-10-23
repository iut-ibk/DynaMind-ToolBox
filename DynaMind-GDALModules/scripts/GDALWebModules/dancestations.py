
from osgeo import ogr, osr
from pydynamind import *
import psycopg2

class LoadDAnCEStations(Module):

        display_name = "Load Rain Station"
        group_name = "DAnCE Platform"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("host", STRING)
            self.host = "localhost"

            self.createParameter("database", STRING)
            self.database = "danceplatform"

            self.createParameter("username", STRING)
            self.username = ""

            self.createParameter("password", STRING)
            self.password = ""

            self.createParameter("view_name", STRING)
            self.view_name = "station"

            self.createParameter("append", BOOL)
            self.append = False

            self.createParameter("filter", STRING)
            self.filter = ""


        def init(self):

            viewvector = []
            self.node_station = ViewContainer(self.view_name, NODE, WRITE)
            self.node_station.addAttribute("dance_station_id", Attribute.INT, WRITE)
            self.node_station.addAttribute("name", Attribute.STRING, WRITE)
            self.node_station.addAttribute("description", Attribute.STRING, WRITE)
            self.node_station.addAttribute("short_description", Attribute.STRING, WRITE)
            self.node_station.addAttribute("start_date", Attribute.STRING, WRITE)
            self.node_station.addAttribute("end_date", Attribute.STRING, WRITE)
            viewvector.append(self.node_station)
            if self.append:
                self.dummy = ViewContainer("dummy", SUBSYSTEM, MODIFY)
                viewvector.append(self.dummy)

            self.registerViewContainers(viewvector)

        def run(self):
            try:
                conn = psycopg2.connect("dbname=" + str(self.database) + " user='" + str(self.username) + "' host='" + str(self.host) + "' password='" + str(self.password) + "'")
            except:
                print("I am unable to connect to the database")


            source_osr = osr.SpatialReference()
            source_osr.ImportFromEPSG(4326)
            target_osr = osr.SpatialReference()
            target_osr.ImportFromEPSG(self.getSimulationConfig().getCoorindateSystem())
            transformation = osr.CreateCoordinateTransformation(source_osr, target_osr)

            cur = conn.cursor()

            filter_query = ""
            if self.node_station.get_attribute_filter_sql_string(""):
                filter_query = "WHERE " + self.node_station.get_attribute_filter_sql_string("")

            if filter_query == "" and self.filter != "":
                filter_query = "WHERE " + self.filter
            elif self.filter != "":
                filter_query = filter_query + " AND " + self.filter

            log(filter_query, Standard)

            cur.execute("SELECT * from station " + filter_query)
            rows = cur.fetchall()
            for r in rows:
                station = self.node_station.create_feature()

                station.SetField("dance_station_id", r[0])
                station.SetField("name", r[1])

                station.SetField("description", r[2])
                station.SetField("short_description", r[3])

                station.SetField("start_date", r[8].strftime("%m-%d-%Y %H:%M:%S"))
                station.SetField("end_date", r[9].strftime("%m-%d-%Y %H:%M:%S"))
                print(r)
                pt = ogr.Geometry(ogr.wkbPoint)
                pt.SetPoint_2D(0, r[4], r[5])
                pt.Transform(transformation)
                station.SetGeometry(pt)

            self.node_station.finalise()