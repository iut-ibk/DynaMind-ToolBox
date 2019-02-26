
from osgeo import ogr, osr
from pydynamind import *
import psycopg2
import datetime



class LoadExtremeTemperature(Module):
        display_name = "Load Extreme Temperature"
        group_name = "DAnCE Platform"

        def __init__(self):
            Module.__init__(self)
            self.setIsGDALModule(True)

            self.createParameter("host", STRING)
            self.host = "localhost"

            self.createParameter("database", STRING)
            self.database = "danceplatform"

            self.createParameter("table", STRING)
            self.table = "measurment"

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
            self.node_station = ViewContainer(self.view_name, COMPONENT, READ)
            self.node_station.addAttribute("dance_station_id", Attribute.INT, READ)

            self.timeseries = ViewContainer("timeseries", COMPONENT, WRITE)
            self.timeseries.addAttribute(self.view_name+"_id", Attribute.INT, WRITE)
            self.timeseries.addAttribute("data", Attribute.DOUBLEVECTOR, WRITE)
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

        def get_extreme_date(self, cur, station_id):
            cur.execute("Select  *, avg(at) over(rows 2 preceding) as rolling_avg  from (SELECT  date_trunc('day', date) AS d, avg(value) as at,max(value) as mt, min(value) as mint, count(value) from measurment where station_id = " + str(station_id) + " and value < 9999 GROUP BY d order by d) temp order by rolling_avg DESC LIMIT 1")
            rows = cur.fetchall()
            for r in rows:
                print r[0]
                return r[0]

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


            # for id, name in self.measurement_types(cur):
            self.createTimeseries(cur)

            if self.from_city:
                self.city.finalise()
            self.node_station.finalise()
            self.timeseries.finalise()

        def createTimeseries(self, cur):
            filter_query = ""
            if self.node_station.get_attribute_filter_sql_string(""):
                filter_query = " AND " + self.node_station.get_attribute_filter_sql_string("")

            # 2010-1-1 00:00:00
            # if start_date:
            #     filter_query += " AND date > '" + start_date + "'"
            #
            # if end_date:
            #     filter_query += " AND date <= '" +end_date + "'"

            log(filter_query, Debug)

            for station in self.node_station:
                station_id = station.GetFieldAsInteger("dance_station_id")
                end_date = self.get_extreme_date(cur, station_id)
                start_date = end_date - datetime.timedelta(days=3)

                print start_date, end_date

                if start_date:
                    filter_query += " AND date >= '" + str(start_date) + "'"

                if end_date:
                    filter_query += " AND date < '" + str(end_date) + "'"


                cur.execute("SELECT date, value from " + str(self.table) + " WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(6) + filter_query + " ORDER BY date ASC")
                rows = cur.fetchall()
                result_temp = []
                inserted = 0
                for entry in rows:
                    val = float(entry[1])
                    result_temp.append(val)
                    inserted+=1
                    print entry[0], entry[1]



                # Find start and end date
                # cur.execute("SELECT date from " + str(self.table) + " WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date DESC LIMIT 1 ")
                # rows = cur.fetchall()
                # for r in rows:
                #     end = r[0].strftime('%d.%m.%Y %H:%M:%S')
                #
                # cur.execute("SELECT date  from " + str(self.table) + " WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date ASC LIMIT 1 ")
                #
                # rows = cur.fetchall()
                # for r in rows:
                #    start = r[0].strftime('%d.%m.%Y %H:%M:%S')
                #
                # cur.execute("SELECT date from " + str(self.table) + " WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date ASC LIMIT 2 ")
                # rows = cur.fetchall()
                # times = []
                # for r in rows:
                #     times.append(r[0])
                # if len(times) != 2:
                #     log("No data for this slice found ", Warning)
                #     continue
                # db_timestep = (times[1] - times[0]).total_seconds()
                #
                timeseries = self.timeseries.create_feature()
                timeseries.SetField("start", str(start_date))
                timeseries.SetField("end", str(end_date))
                timeseries.SetField(self.view_name+"_id", station.GetFID())
                # delta_t = self.deltaT
                # if self.deltaT < db_timestep:
                #     log( "used timestep " + str(db_timestep), Warning)
                #     delta_t = db_timestep
                #
                # if self.deltaT % db_timestep != 0:
                #     log("non integer multiplier for timestep use nearest timestep " + str(int(delta_t / db_timestep) * db_timestep), Warning)
                #
                # skipper = int(delta_t / db_timestep)
                #
                # timeseries.SetField("timestep", int(db_timestep * skipper))
                #
                # cur.execute("SELECT date, value from " + str(self.table) + " WHERE station_id = " + str(station_id) + " AND measurment_type_id=" + str(measurment_type_id) + filter_query + " ORDER BY date ASC")
                # rows = cur.fetchall()
                # counter = 0
                # r = 0
                # result_rain = []

                # for entry in rows:
                #     val = float(entry[1])
                #     if val < 0.0:
                #         val = 0
                #     r += val
                #     if counter % skipper == 0:
                #         result_rain.append(r)
                #         r = 0
                #         inserted+=1
                #
                #
                #     counter+=1
                log(str(inserted), Standard)
                dm_set_double_list(timeseries, "data", result_temp)
                timeseries.SetField("type", "temperature")
                timeseries.SetField("timestep", 60*30)


