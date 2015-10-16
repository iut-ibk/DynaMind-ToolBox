
from osgeo import ogr, osr
from pydynamind import *
import psycopg2



class LoadDAnCETimeseries(Module):
        display_name = "Load Timeseries Data"
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

            # date format
            self.createParameter("start", STRING)
            self.start = ""

            self.createParameter("end", STRING)
            self.end = ""

            self.createParameter("deltaT", INT)
            self.deltaT = 0

            self.createParameter("from_city", BOOL)
            self.from_city = False

            self.createParameter("view_name", STRING)
            self.view_name = "station"



        def init(self):
            datastream = []
            self.node_station = ViewContainer(self.view_name, NODE, READ)
            self.node_station.addAttribute("dance_station_id", Attribute.INT, READ)

            self.timeseries = ViewContainer("timeseries", COMPONENT, WRITE)
            self.timeseries.addAttribute(self.view_name+"_id", Attribute.INT, WRITE)
            self.timeseries.addAttribute("values", Attribute.DOUBLEVECTOR, WRITE)
            self.timeseries.addAttribute("start", Attribute.STRING, WRITE)
            self.timeseries.addAttribute("end", Attribute.STRING, WRITE)
            self.timeseries.addAttribute("timestep", Attribute.INT, WRITE)
            self.timeseries.addAttribute("type", Attribute.STRING, WRITE)

            datastream.append(self.node_station)
            datastream.append(self.timeseries)

            if self.from_city:
                self.city = ViewContainer("city", COMPONENT, READ)
                self.city.addAttribute("start_date", Attribute.STRING, READ)
                self.city.addAttribute("end_date", Attribute.STRING, READ)
                datastream.append(self.city)

            #self.node_station.addAttribute("station_id", Attribute.DOUBLEVECTOR, WRITE)
            self.registerViewContainers(datastream)

        def measurement_types(self, cur):
            measurment_types = []
            cur.execute("SELECT id, name from measurment_type")
            rows = cur.fetchall()
            for r in rows:
                measurment_types.append((r[0], r[1]))
            return measurment_types

        def run(self):
            try:
                conn = psycopg2.connect("dbname=" + str(self.database) + " user='" + str(self.username) + "' host='" + str(self.host) +  "' password='" + str(self.password) + "'")
            except:
                log("Unable to connect to database ", Error)
                self.setStatus(MOD_CHECK_ERROR)
                return
            cur = conn.cursor()

            start_date = self.start
            end_date = self.end

            if self.from_city:
                for c in self.city:
                    start_date = c.GetFieldAsString("start_date")
                    end_date = c.GetFieldAsString("end_date")


            for id, name in self.measurement_types(cur):
                self.createTimeseries(cur, id, name, start_date, end_date)

            if self.from_city:
                self.city.finalise()
            self.node_station.finalise()
            self.timeseries.finalise()

        def createTimeseries(self, cur, measurment_type_id, name, start_date, end_date):
            filter_query = ""
            if self.node_station.get_attribute_filter_sql_string(""):
                filter_query = " AND " + self.node_station.get_attribute_filter_sql_string("")

            # 2010-1-1 00:00:00
            if start_date:
                filter_query += " AND date > '" + start_date + "'"

            if end_date:
                filter_query += " AND date <= '" +end_date + "'"

            log(filter_query, Debug)

            for station in self.node_station:
                station_id = station.GetFieldAsInteger("dance_station_id")

                # Find start and end date
                cur.execute("SELECT date from measurment WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date DESC LIMIT 1 ")
                rows = cur.fetchall()
                for r in rows:
                    end = r[0].strftime('%d.%m.%Y %H:%M:%S')

                cur.execute("SELECT date  from measurment WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date ASC LIMIT 1 ")

                rows = cur.fetchall()
                for r in rows:
                   start = r[0].strftime('%d.%m.%Y %H:%M:%S')

                cur.execute("SELECT date from measurment WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date ASC LIMIT 2 ")
                rows = cur.fetchall()
                times = []
                for r in rows:
                    times.append(r[0])
                if len(times) != 2:
                    log("No data for this slice found ", Warning)
                    continue
                db_timestep = (times[1] - times[0]).total_seconds()

                timeseries = self.timeseries.create_feature()
                timeseries.SetField("start", start)
                timeseries.SetField("end", end)
                timeseries.SetField(self.view_name+"_id", station.GetFID())
                delta_t = self.deltaT
                if self.deltaT < db_timestep:
                    log( "used timestep " + str(db_timestep), Warning)
                    delta_t = db_timestep

                if self.deltaT % db_timestep != 0:
                    log("non integer multiplier for timestep use nearest timestep " + str(int(delta_t / db_timestep) * db_timestep), Warning)

                skipper = int(delta_t / db_timestep)

                timeseries.SetField("timestep", int(db_timestep * skipper))

                cur.execute("SELECT date, value from measurment WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date ASC")
                rows = cur.fetchall()
                counter = 0
                r = 0
                result_rain = []
                inserted = 0
                for entry in rows:
                    r += float(entry[1])
                    if counter % skipper == 0:
                        result_rain.append(r)
                        r = 0
                        inserted+=1


                    counter+=1
                print counter, inserted
                timeseries.SetField("values", ' '.join(str(d) for d in result_rain))
                timeseries.SetField("type", name)


